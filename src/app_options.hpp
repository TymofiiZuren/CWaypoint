#pragma once
#include <string>
#include <vector>
namespace cw {
struct AppOptions {
  std::string profile, capture, initial="home", resumeTest;
  bool smoke=false, guided=false, reviewTest=false, codeTest=false,
       undoTest=false, authoredTest=false, courseTest=false, conditionsTest=false,
       pointerTest=false, settingsTest=false, compact=false, largeText=false,
       checkContent=false, help=false;
};
AppOptions parseOptions(const std::vector<std::string>& arguments, const std::string& defaultProfile);
std::string commandHelp();
}
