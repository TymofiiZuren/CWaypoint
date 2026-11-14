#include "process.hpp"
#include <array>
#include <chrono>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
namespace cw { namespace {
using Clock=std::chrono::steady_clock;
struct Pipes {
 std::array<int,6> fd{{-1,-1,-1,-1,-1,-1}};
 ~Pipes(){for(int f:fd)if(f>=0)close(f);}
 bool openAll(){for(int i=0;i<6;i+=2)if(pipe(fd.data()+i))return false;return true;}
 void release(int i){if(fd[i]>=0){close(fd[i]);fd[i]=-1;}}
};
pid_t launch(const std::vector<std::string>& argv,Pipes& p,bool ownGroup){
 std::vector<char*> args;for(auto& a:argv)args.push_back(const_cast<char*>(a.c_str()));args.push_back(nullptr);
 char path[]="PATH=/usr/bin:/bin",lang[]="LC_ALL=C";char* env[]={path,lang,nullptr};
 posix_spawn_file_actions_t actions;posix_spawn_file_actions_init(&actions);
 posix_spawn_file_actions_adddup2(&actions,p.fd[0],0);
 posix_spawn_file_actions_adddup2(&actions,p.fd[3],1);
 posix_spawn_file_actions_adddup2(&actions,p.fd[5],2);
 posix_spawnattr_t attr;posix_spawnattr_init(&attr);
 short flags=POSIX_SPAWN_CLOEXEC_DEFAULT;
 if(ownGroup){flags|=POSIX_SPAWN_SETPGROUP;posix_spawnattr_setpgroup(&attr,0);}
 posix_spawnattr_setflags(&attr,flags);
 pid_t pid=-1;int err=posix_spawn(&pid,args[0],&actions,&attr,args.data(),env);
 posix_spawnattr_destroy(&attr);posix_spawn_file_actions_destroy(&actions);
 if(err){errno=err;return -1;}return pid;
}
void drain(int fd,std::string& text,ProcessResult& result,size_t limit){
 char bytes[4096];ssize_t n;
 while((n=read(fd,bytes,sizeof(bytes)))>0){
  size_t available=limit-result.output.size()-result.error.size();
  auto take=std::min(available,static_cast<size_t>(n));text.append(bytes,take);
  if(take<static_cast<size_t>(n)){result.outputLimited=true;return;}
 }
}
void sendInput(Pipes& p,const std::string& input,size_t& sent){
 if(p.fd[1]<0)return;
 if(sent<input.size()){
  auto n=write(p.fd[1],input.data()+sent,input.size()-sent);
  if(n>0)sent+=static_cast<size_t>(n);
  else if(n<0 && errno!=EAGAIN && errno!=EINTR)p.release(1);
 }
 if(sent==input.size())p.release(1);
}
bool shouldStop(ProcessResult& r,Clock::time_point start,double timeout,const std::atomic_bool* cancel){
 r.cancelled=cancel && cancel->load();
 r.timedOut=std::chrono::duration<double>(Clock::now()-start).count()>timeout;
 return r.cancelled || r.timedOut || r.outputLimited;
}
void signalJob(pid_t pid,int sig,bool group){kill(group ? -pid:pid,sig);}
void waitStopped(pid_t pid,bool group,int& status){
 signalJob(pid,SIGTERM,group);
 for(int i=0;i<25;++i){if(waitpid(pid,&status,WNOHANG)==pid)return;poll(nullptr,0,10);}
 signalJob(pid,SIGKILL,group);while(waitpid(pid,&status,0)<0 && errno==EINTR){}
}
} // namespace
ProcessResult runProcess(const std::vector<std::string>& argv,const std::string& input,double timeout,
 const std::atomic_bool* cancel,size_t limit,bool ownGroup){
 ProcessResult r;Pipes p;
 static const bool ignored=[](){signal(SIGPIPE,SIG_IGN);return true;}();(void)ignored;
 if(cancel && cancel->load()){r.cancelled=true;return r;}
 if(argv.empty() || !p.openAll()){r.error="Cannot create job pipes.";return r;}
 auto pid=launch(argv,p,ownGroup);
 if(pid<0){r.error="Cannot launch tool: "+std::string(strerror(errno));return r;}
 p.release(0);p.release(3);p.release(5);
 for(int i:{1,2,4})fcntl(p.fd[i],F_SETFL,O_NONBLOCK);
 size_t sent=0;int status=0;const auto start=Clock::now();
 while(true){
  sendInput(p,input,sent);drain(p.fd[2],r.output,r,limit);drain(p.fd[4],r.error,r,limit);
  if(waitpid(pid,&status,WNOHANG)==pid)break;
  if(shouldStop(r,start,timeout,cancel)){waitStopped(pid,ownGroup,status);break;}
  poll(nullptr,0,5);
 }
 drain(p.fd[2],r.output,r,limit);drain(p.fd[4],r.error,r,limit);
 if(ownGroup)signalJob(pid,SIGKILL,true);
 if(WIFEXITED(status))r.exitCode=WEXITSTATUS(status);
 else if(WIFSIGNALED(status))r.exitCode=128+WTERMSIG(status);
 return r;
}
} // namespace cw
