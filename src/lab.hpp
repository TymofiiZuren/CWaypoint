#pragma once
#include "process.hpp"
#include <filesystem>
namespace cw {
struct LabCase {std::string input,expected;};
struct LabExercise {std::string lessonId,language,starter,solution,instructions;int revision=1;std::vector<LabCase> cases;};
struct LabResult {bool compiled=false,cancelled=false;std::string diagnostics,sourceHash;int testRevision=1;std::vector<ProcessResult> runs;int passed=0;bool allPassed() const{return compiled && !runs.empty() && passed==static_cast<int>(runs.size()) && !cancelled;}};
LabExercise labFor(const std::string& lessonId);
std::string sourceHash(const std::string& source);
LabResult runLab(const LabExercise&,const std::string& source,const std::filesystem::path& helper,
 const std::filesystem::path& tools,const std::atomic_bool* cancel=nullptr);
}
