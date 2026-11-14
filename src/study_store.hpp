#pragma once
#include "core.hpp"
namespace cw {
struct Preferences {std::string name;int goal=1,editorSize=14;bool lightEditor=false,nudge=true;};
struct StudyEntry {std::string note,source;bool bookmark=false,hasSource=false;size_t cursor=0;};
class StudyStore {
 Store& owner;unsigned long generation;bool supported=true;
 bool load();
 public:
 Preferences preferences;std::map<std::string,StudyEntry> entries;
 std::string notice="Changes stay on this Mac";
 bool unsaved=false;
 explicit StudyStore(Store&);
 bool writable()const{return supported && owner.writable();}
 bool save();bool saveSnapshot(const std::string& hash,const std::string& source);
 bool syncGeneration();
};
}
