#pragma once
#include "app_options.hpp"
#include "core.hpp"
namespace cw {
int runReferenceApp(const AppOptions&,const std::vector<Lesson>&,Store&,Controller&);
int checkPackagedRunner();
}
