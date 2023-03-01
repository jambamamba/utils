#include "ssh_session.h"

#include <debug_logger.h>
#include <libssh/callbacks.h>

LOG_CATEGORY(SSH, "SSH")

namespace {

bool WaitLoop(std::function<int()> cb, int success, std::function<bool()> keepWaiting)
{
   while(keepWaiting())
   {
      int rc = cb();
      if (rc == success)
      {
         return true;
      }
      else if(rc == SSH_AGAIN || rc == SSH_AUTH_AGAIN)
      {
		  continue;
      }
      break;
   }
   return false;
}

int VerifyKnownHost(ssh_session session,
                    std::function<void(const std::string &title, const std::string &message)> userMessageProc)
{
    enum ssh_known_hosts_e state;
    unsigned char *hash = nullptr;
    ssh_key srv_pubkey = nullptr;
    size_t hlen;
    char buf[10];
    char *hexa;
    char *p;
    int cmp;
    int rc;
    rc = ssh_get_server_publickey(session, &srv_pubkey);
    if (rc < 0) {
        return -1;
    }
    rc = ssh_get_publickey_hash(srv_pubkey,
                                SSH_PUBLICKEY_HASH_SHA1,
                                &hash,
                                &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0) {
        return -1;
    }
    state = ssh_session_is_known_server(session);
    switch (state) {
        case SSH_KNOWN_HOSTS_OK:
            /* OK */
            break;
        case SSH_KNOWN_HOSTS_CHANGED:
//            LOG(WARNING, SSH, "Host key for server changed: it is now:\n");
//            ssh_print_hexa("Public key hash", hash, hlen);
//            LOG(WARNING, SSH, "For security reasons, connection will be stopped\n");
//            return -1;
            hexa = ssh_get_hexa(hash, hlen);
            userMessageProc("Host key for server changed",
                             (std::string("Accepting new host key.\n\nPublic key hash:\n") + 
							 std::string(hexa)).c_str()
							 );
            ssh_string_free_char(hexa);
            ssh_clean_pubkey_hash(&hash);
            rc = ssh_session_update_known_hosts(session);
            if (rc < 0) {
                LOG(WARNING, SSH, "Error %s\n", strerror(errno));
                return -1;
            }
            break;
        case SSH_KNOWN_HOSTS_OTHER:
            LOG(WARNING, SSH, "The host key for this server was not found but an other"
                    "type of key exists.\n");
            LOG(WARNING, SSH, "An attacker might change the default server key to"
                    "confuse your client into thinking the key does not exist\n");
            ssh_clean_pubkey_hash(&hash);
            return -1;
        case SSH_KNOWN_HOSTS_NOT_FOUND:
            LOG(WARNING, SSH, "Could not find known host file.\n");
            LOG(WARNING, SSH, "If you accept the host key here, the file will be"
                    "automatically created.\n");
            /* FALL THROUGH to SSH_SERVER_NOT_KNOWN behavior */
        case SSH_KNOWN_HOSTS_UNKNOWN:
            hexa = ssh_get_hexa(hash, hlen);
//            fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
//            LOG(WARNING, SSH, "Public key hash: %s\n", hexa);
//            ssh_string_free_char(hexa);
//            ssh_clean_pubkey_hash(&hash);
//            p = fgets(buf, sizeof(buf), stdin);
//            if (p == nullptr) {
//                return -1;
//            }
//            cmp = strncasecmp(buf, "yes", 3);
//            if (cmp != 0) {
//                return -1;
//            }
//For testing
            userMessageProc("The server is unknown",
                             (std::string("Accepting untrustworthy the host key.\n\nPublic key hash:\n") + std::string(hexa)).c_str());
            ssh_string_free_char(hexa);
            ssh_clean_pubkey_hash(&hash);
//            QMessageBox::StandardButton reply;
//            reply = QMessageBox::question(nullptr, "The server is unknown",
//                                          std::string("Do you trust the host key?\n\nPublic key hash:\n") + 
//                                          std::string(hexa),
//                                          QMessageBox::Yes|QMessageBox::No);
//            if (reply != QMessageBox::Yes) {
//               return -1;
//            }
            rc = ssh_session_update_known_hosts(session);
            if (rc < 0) {
                LOG(WARNING, SSH, "Error %s\n", strerror(errno));
                return -1;
            }
            break;
        case SSH_KNOWN_HOSTS_ERROR:
            LOG(WARNING, SSH, "Error %s", ssh_get_error(session));
            ssh_clean_pubkey_hash(&hash);
            return -1;
    }
    ssh_clean_pubkey_hash(&hash);
    return 0;
}
void SshLoggingCallback(int priority,
                                      const char *function,
                                      const char *buffer,
                                      void *userdata)
{
   LOG(DEBUG, SSH, "%s:%s\n", function, buffer);
}

}//namespace

SshSession::SshSession(std::function<void(const std::string &title, const std::string &message)> userMessageProc)
   : _session(ssh_new())
   , UserMessageProc(userMessageProc)
{
   ssh_set_log_callback(SshLoggingCallback);
}

SshSession::~SshSession()
{
   if(_session)
   {
     ssh_free(_session);
     _session = nullptr;
   }
}

bool SshSession::Connect(const std::string &ip,
                         int port,
                         const std::string &username,
                         const std::string &password,
                         std::function<bool()> keepWaiting)
{
   if(!_session)
   {
      return false;
   }
   if(_connected)
   {
      Disconnect();
   }

   ssh_options_set(_session, SSH_OPTIONS_HOST, ip.c_str());

   int verbosity = SSH_LOG_FUNCTIONS;
   ssh_options_set(_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
   ssh_options_set(_session, SSH_OPTIONS_PORT, &port);
   ssh_set_blocking(_session, false);

   if(!WaitLoop([this](){
         return ssh_connect(_session);
      }, SSH_OK, keepWaiting))
   {
      return false;
   }

   if(!WaitLoop([this](){
                return VerifyKnownHost(_session, UserMessageProc);
      }, SSH_AUTH_SUCCESS, keepWaiting))
   {
      Disconnect();
      UserMessageProc("Error",
                      (std::string("Failed to verify host. Error: ")+std::string((ssh_get_error(_session)))).c_str());
      return false;
   }

   if(!WaitLoop([this,username,password](){
      return ssh_userauth_password(_session,
                                   username.c_str(),
                                   password.c_str());
      }, SSH_AUTH_SUCCESS, keepWaiting))
   {
      Disconnect();
      UserMessageProc("Authentication Failed",
                       (std::string("Could not authenticate ") + 
					   username + 
					   std::string(" with password: ") + 
					   password + 
					   std::string("\n\nError: ") + 
                   std::string(ssh_get_error(_session))).c_str());
     return false;
   }

   ssh_set_blocking(_session, true);
   _connected = true;
   return true;
}

void SshSession::Disconnect()
{
   if(_session)
   {
      ssh_disconnect(_session);
      _connected = false;
   }
}

bool SshSession::ExecuteRemoteCommand(const std::string &cmd, 
                                       std::function<bool()> keepWaiting,
                                       std::function <bool(const char *buffer, int nbytes)> stdoutfn)
{
   if(!_session){
      LOG(FATAL, SSH, "No ssh session\n");
      return false;
   }
   if(!_connected){
      LOG(FATAL, SSH, "Not connected\n");
      return false;
   }
   ssh_channel channel = ssh_channel_new(_session);
   if (channel == nullptr){
      LOG(FATAL, SSH, "Could not create channel\n");
      return false;
   }
   int rc = ssh_channel_open_session(channel);
   if (rc != SSH_OK){
      LOG(FATAL, SSH, "Could not open channel\n");
     ssh_channel_free(channel);
     return false;
   }
   rc = ssh_channel_request_exec(channel, cmd.c_str());
   if (rc != SSH_OK){
     LOG(FATAL, SSH, "Could not execute request on connected channel\n");
     ssh_channel_close(channel);
     ssh_channel_free(channel);
     return false;
   }
   char buffer[256] = {};
   constexpr int TIMEOUT_MS = 5000;
   constexpr int STDOUT = 0;
   constexpr int STDERR = 1;
   int nbytes = ssh_channel_read_timeout(channel, buffer, sizeof(buffer), STDERR, TIMEOUT_MS);
   while (nbytes > 0){
      if(!keepWaiting()){
         ssh_channel_send_eof(channel);
         ssh_channel_close(channel);
         ssh_channel_free(channel);
         return false;
      }
      else if(stdoutfn){
        if(!stdoutfn(buffer, nbytes)){
           break;
        }
      }
      else{
        LOG(DEBUG, SSH, "Recvd: %s\n", buffer);
      }
      nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
   }
   if (nbytes < 0 && ssh_get_error_code(_session) != 2/*Remote channel is closed*/){
      char message[256] = {0};
      snprintf(message, sizeof(message)-1, "Read error: %i:%s\n", ssh_get_error_code(_session), ssh_get_error(_session));

     if(stdoutfn){
        stdoutfn(message, strlen(message)+1);
     }
     else{
      LOG(DEBUG, SSH, "%s\n", message);
     }
     ssh_channel_close(channel);
     ssh_channel_free(channel);
     return false;
   }
   ssh_channel_send_eof(channel);
   ssh_channel_close(channel);
   ssh_channel_free(channel);

   return true;
}

ssh_session SshSession::GetSession()
{
   return _session;
}
