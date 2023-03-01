#pragma once

extern "C" {
#include <libssh/libssh.h>
#include <stdlib.h>
}
#include <functional>

class SshSession
{
public:
   SshSession(std::function<void(const std::string &title, const std::string &message)> userMessageProc = nullptr);
   ~SshSession();

   bool Connect(const std::string &ip, int port, const std::string &username, 
	const std::string &password, std::function<bool()> keepWaiting);
   void Disconnect();
   bool ExecuteRemoteCommand(const std::string &cmd, 
      std::function<bool()> keepWaiting, 
      std::function <bool(const char *buffer, int nbytes)> stdoutfn = nullptr);
   ssh_session GetSession();


protected:
   ssh_session _session;
   bool _connected = false;
   std::function<void(const std::string &title, const std::string &message)> UserMessageProc;
};
