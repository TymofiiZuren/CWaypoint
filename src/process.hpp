#pragma once
#include <atomic>
#include <string>
#include <vector>
namespace cw {
struct ProcessResult {
 int exitCode=-1;
 bool timedOut=false,cancelled=false,outputLimited=false;
 std::string output,error;
};
// argv is always executed directly. Only the created child/job group is signalled.
ProcessResult runProcess(const std::vector<std::string>& argv,const std::string& input,
 double timeout,const std::atomic_bool* cancel=nullptr,size_t limit=65536,bool ownGroup=true);
}
