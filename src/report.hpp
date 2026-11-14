#pragma once
#include "core.hpp"
namespace cw {
struct ExportResult { std::filesystem::path path; std::string error; };
std::string progressCsv(const std::vector<Lesson>&, const Controller&);
ExportResult exportProgress(const std::filesystem::path&, const std::vector<Lesson>&, const Controller&);
}
