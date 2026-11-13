#include "app_options.hpp"
#include <map>
#include <set>
#include <stdexcept>
namespace cw {
namespace {
void validateOptions(const AppOptions& o,const std::string& defaultProfile) {
  const std::set<std::string> screens{"home","lesson","review","settings","progress","compare"};
  const std::set<std::string> phases{"","seed","question","walk-seed","walk"};
  if(!screens.count(o.initial)) throw std::runtime_error("Unknown initial screen: "+o.initial);
  if(!phases.count(o.resumeTest)) throw std::runtime_error("Unknown resume test phase.");
  if(o.smoke && o.profile==defaultProfile) throw std::runtime_error("Smoke test requires an isolated --data-dir.");
}
bool hasJourney(const AppOptions& o) {
  const bool flags[]={o.guided,o.reviewTest,o.codeTest,o.undoTest,o.authoredTest,o.courseTest,o.conditionsTest,o.pointerTest,o.settingsTest};
  for(bool active:flags) if(active) return true;
  return !o.resumeTest.empty();
}
}
AppOptions parseOptions(const std::vector<std::string>& args,const std::string& defaultProfile) {
  AppOptions o; o.profile=defaultProfile;
  const std::map<std::string,bool AppOptions::*> flags{
    {"--check-content",&AppOptions::checkContent},{"--help",&AppOptions::help},
    {"--compact",&AppOptions::compact},{"--large-text",&AppOptions::largeText},
    {"--smoke-test",&AppOptions::smoke},{"--guided-test",&AppOptions::guided},
    {"--review-test",&AppOptions::reviewTest},{"--code-test",&AppOptions::codeTest},
    {"--undo-test",&AppOptions::undoTest},{"--authored-test",&AppOptions::authoredTest},
    {"--course-test",&AppOptions::courseTest},{"--conditions-test",&AppOptions::conditionsTest},
    {"--pointer-test",&AppOptions::pointerTest},{"--settings-test",&AppOptions::settingsTest}};
  const std::map<std::string,std::string*> values{{"--data-dir",&o.profile},{"--capture-dir",&o.capture},{"--screen",&o.initial},{"--resume-test",&o.resumeTest}};
  for(size_t i=0;i<args.size();++i) {
    auto flag=flags.find(args[i]);
    if(flag!=flags.end()) { o.*(flag->second)=true; continue; }
    auto value=values.find(args[i]);
    if(value==values.end() || i+1==args.size()) throw std::runtime_error("Unknown or incomplete option: "+args[i]);
    *value->second=args[++i];
  }
  o.smoke=o.smoke || hasJourney(o);
  if(!o.help && !o.checkContent) validateOptions(o,defaultProfile);
  return o;
}
std::string commandHelp() {
  return "CWaypoint [--check-content] [--data-dir PATH] [--capture-dir PATH] "
    "[--screen home|lesson|review|settings|progress|compare] "
    "[--settings-test|--pointer-test|--conditions-test|--course-test|--smoke-test|--guided-test|--review-test|--code-test|--undo-test|--authored-test] "
    "[--resume-test seed|question|walk-seed|walk]\n";
}
}
