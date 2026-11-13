#include "report.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
using namespace cw;
void check(bool b) { if (!b) throw std::runtime_error("report assertion failed"); }
int main() {
  auto root = std::filesystem::path(".scratch") / ("reports-" + std::to_string(getpid()));
  Store store(root);
  auto lessons = sampleLessons();
  Controller app(lessons, store);
  app.submit(0, 0, lessons[0].questions[0].answer);
  auto csv = progressCsv(lessons, app);
  check(csv.find("Solved independently on first attempt") != std::string::npos);
  check(csv.find("learner_answer") == std::string::npos);
  app.profile.evidence[lessons[0].questions[0].id].revision--;
  csv = progressCsv(lessons, app);
  check(csv.find("Solved independently on first attempt") == std::string::npos);
  lessons[0].title = "=formula,\"quoted\"";
  check(progressCsv(lessons, app).find("'=formula,\"\"quoted\"\"") != std::string::npos);
  app.profile.recovered = true;
  app.profile.evidence[lessons[0].questions[0].id].revision = lessons[0].revision;
  app.unsavedChanges = true;
  csv = progressCsv(lessons, app);
  check(csv.find("History recovered; first attempt unverified") != std::string::npos);
  check(csv.find(",1,unsaved") != std::string::npos);
  check(csv.find("Solved independently on first attempt") == std::string::npos);
  auto first = exportProgress(root / "exports", lessons, app);
  auto second = exportProgress(root / "exports", lessons, app);
  check(first.error.empty() && second.error.empty() && first.path != second.path);
  std::ifstream input(first.path); std::string bytes((std::istreambuf_iterator<char>(input)), {});
  check(bytes == progressCsv(lessons, app));
  std::ofstream(root / "blocked") << "preserve";
  check(!exportProgress(root / "blocked", lessons, app).error.empty());
  std::cout << "Progress report checks passed\n";
}
