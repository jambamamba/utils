#include "scp_session.h"

#include <debug_logger.h>
#include <fcntl.h>
#include <sstream> 
#include <string>
#include <sys/stat.h>
#include <vector>

LOG_CATEGORY(SSH, "SSH")

namespace {

std::vector<std::string> splitString(const std::string &in, char delimiter)
{
    std::vector<std::string> strings;
    std::istringstream f(in.c_str());
    std::string s;    
    while (std::getline(f, s, delimiter)) {
        strings.push_back(s);
    }
	return strings;
}

}//namespace

ScpSession::ScpSession(ssh_session ssh)
   : _ssh(ssh)
   , _scp(nullptr)
{
}

ScpSession::~ScpSession()
{
}

bool ScpSession::Init(ssh_scp scp)
{
   int rc = ssh_scp_init(scp);
   if (rc != SSH_OK)
   {
      LOG(WARNING, SSH, "Error initializing scp session: %s\n",
              ssh_get_error(_ssh));
      ssh_scp_free(scp);
      return rc;
   }
   return true;
}

void ScpSession::DeInit(ssh_scp scp)
{
   if(scp)
   {
      ssh_scp_close(scp);
      ssh_scp_free(scp);
   }
}

int ScpSession::ReadRemoteFile(const std::string &remoteFilePath, 
                              std::function<bool()> keepWaiting,
                              std::function<ssize_t(char* buffer, int bytes)>writeLocalFile)
{
   ssh_scp scp = ssh_scp_new
         (_ssh, SSH_SCP_READ, remoteFilePath.c_str());
   if (scp == nullptr)
   {
      LOG(WARNING, SSH, "Error allocating scp session: %s\n",
              ssh_get_error(_ssh));
      return SSH_ERROR;
   }

   if(!Init(scp))
   {
      return -1;
   }
   int rc = ssh_scp_pull_request(scp);
   if (rc != SSH_SCP_REQUEST_NEWFILE)
   {
      LOG(WARNING, SSH, "Error receiving information about file: %s\n",
              ssh_get_error(_ssh));
      DeInit(scp);
      return SSH_ERROR;
   }
   size_t size = ssh_scp_request_get_size(scp);
   std::string filename(ssh_scp_request_get_filename(scp));
   int mode = ssh_scp_request_get_permissions(scp);
   LOG(DEBUG, SSH, "Receiving file %s, size %d, permissions 0%o\n",
          filename.c_str(), size, mode);
   ssh_scp_accept_request(scp);

   ssize_t totalWritten = 0;
   while(totalWritten < size && keepWaiting())
   {
      constexpr int MAX_XFER_BUF_SIZE = 1024 * 16;
      char buffer[MAX_XFER_BUF_SIZE];

      rc = ssh_scp_read(scp, buffer, sizeof(buffer));
      if (rc == SSH_ERROR)
      {
         LOG(WARNING, SSH, "Error receiving file data: %s\n",
                 ssh_get_error(_ssh));
         DeInit(scp);
         return rc;
      }

      ssize_t nwritten = writeLocalFile(buffer, rc);
      if(nwritten <= 0)
      {
         break;
      }
      totalWritten += nwritten;
   }

   rc = ssh_scp_pull_request(scp);
   if (rc != SSH_SCP_REQUEST_EOF)
   {
      LOG(WARNING, SSH, "Unexpected request: %s\n",
              ssh_get_error(_ssh));
      DeInit(scp);
      return SSH_ERROR;
   }
   DeInit(scp);
   return SSH_OK;
}

int ScpSession::WriteRemoteFile(const std::string &remoteFilePath, 
                                int fileSize,
                                std::function<bool()> quit, 
                                std::function<ssize_t(char* buffer, int bytes)>readLocalFile)
{
   ssh_scp scp = ssh_scp_new
         (_ssh, SSH_SCP_WRITE | SSH_SCP_RECURSIVE, "/");
   if (scp == nullptr)
   {
      LOG(WARNING, SSH, "Error allocating scp session: %s\n",
              ssh_get_error(_ssh));
      return SSH_ERROR;
   }

   if(!Init(scp))
   {
      return -1;
   }

   int rc = SSH_OK;
   auto pathTokens = splitString(remoteFilePath, '/');
   for(auto &token : pathTokens)
   {
      if(token == pathTokens.back())//last one is the filename, not directory
      {
         break;
      }
      if(token.size() == 0)
      {
         continue;
      }
      rc = ssh_scp_push_directory(scp, token.c_str(), S_IRWXU);
      if (rc != SSH_OK)
      {
         LOG(WARNING, SSH, "Can't create remote directory: %s\n",
                 ssh_get_error(_ssh));
         DeInit(scp);
         return false;
      }
   }

   rc = ssh_scp_push_file
         (scp, remoteFilePath.c_str(), fileSize, S_IRUSR |  S_IWUSR);
   if (rc != SSH_OK)
   {
      LOG(WARNING, SSH, "Can't open remote file: %s\n",
              ssh_get_error(_ssh));
      DeInit(scp);
      return rc;
   }
   ssize_t totalWritten = 0;
   while(true)
   {
      constexpr ssize_t MAX_XFER_BUF_SIZE = 48 * 1024;
      char buffer[MAX_XFER_BUF_SIZE];

      ssize_t nread = readLocalFile(buffer, sizeof(buffer));
      if(nread <= 0)
      {
         break;
      }

      rc = ssh_scp_write(scp, buffer, nread);
      if (rc != SSH_OK)
      {
         LOG(DEBUG, SSH, "Can't write to remote file: %1\n", ssh_get_error(_ssh));
         DeInit(scp);
         return rc;
      }
      if(quit())
      {
         DeInit(scp);
         return SSH_ERROR;
      }
      totalWritten += nread;
   }
   DeInit(scp);
   return SSH_OK;
}
