#pragma once

extern "C" {
#include <libssh/libssh.h>
#include <libssh/scp.h>
#include <stdlib.h>
}

#include <functional>
#include <vector>

class ScpSession
{
public:
   ScpSession(ssh_session ssh);
   ~ScpSession();

   int ReadRemoteFile(const std::string &remoteFilePath, 
               std::function<bool()> quit,
               std::function<ssize_t(char* buffer, int bytes)>writeLocalFile);
   int WriteRemoteFile(const std::string &remoteFilePath, int fileSize, 
               std::function<bool()> quit,
               std::function<ssize_t(char* buffer, int bytes)>readLocalFile);

protected:
   bool Init(ssh_scp scp);
   void DeInit(ssh_scp scp);

   ssh_session _ssh = nullptr;
   ssh_scp _scp = nullptr;
};
