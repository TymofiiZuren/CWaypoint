#include "app_options.hpp"
#include <iostream>
#include <stdexcept>
using namespace cw;
void check(bool value) { if(!value) throw std::runtime_error("option assertion failed"); }
int main() {
  auto o=parseOptions({"--data-dir","isolated","--settings-test","--compact"},"default");
  check(o.profile=="isolated" && o.smoke && o.settingsTest && o.compact);
  check(parseOptions({"--check-content"},"default").checkContent);
  check(parseOptions({"--help"},"default").help);
  for(const auto& args : std::vector<std::vector<std::string>>{{"--data-dir"},{"--screen","unknown"},{"--unknown"},{"--smoke-test"},{"--resume-test","bad"}}) {
    bool failed=false;
    try { parseOptions(args,"default"); } catch(const std::runtime_error&) { failed=true; }
    check(failed);
  }
  check(parseOptions({"--data-dir","isolated","--resume-test","walk"},"default").smoke);
  std::cout<<"Launch option contracts passed\n";
}
