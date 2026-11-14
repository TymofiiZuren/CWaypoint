#include "report.hpp"
#include <cerrno>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>
namespace cw {
namespace {
std::string cell(std::string value) {
  auto first = value.find_first_not_of(" \t\r\n");
  if (first != std::string::npos && std::string("=+-@").find(value[first]) != std::string::npos)
    value.insert(0, "'");
  std::string result = "\"";
  for (char c : value) { if (c == '"') result += '"'; result += c; }
  return result + "\"";
}
}
std::string progressCsv(const std::vector<Lesson>& lessons, const Controller& app) {
  std::ostringstream out;
  out << "track,lesson,question_id,kind,revision,status,attempts,help_used,first_attempt_independent,history_recovered,save_state\r\n";
  for (size_t l=0; l<lessons.size(); ++l) {
    const auto& lesson=lessons[l];
    for (size_t q=0; q<lesson.questions.size(); ++q) {
      auto found=app.profile.evidence.find(lesson.questions[q].id);
      Evidence evidence;
      if (found!=app.profile.evidence.end() && found->second.revision==lesson.revision) evidence=found->second;
      out << cell(lesson.track) << ',' << cell(lesson.title) << ',' << cell(lesson.questions[q].id)
          << ',' << (q==2 ? "fresh_check" : "practice") << ',' << lesson.revision
          << ',' << cell(app.status(static_cast<int>(l),static_cast<int>(q))) << ',' << evidence.attempts
          << ',' << evidence.helped << ',' << (evidence.independent && !app.profile.recovered)
          << ',' << app.profile.recovered << ',' << (app.unsavedChanges ? "unsaved" : "saved") << "\r\n";
    }
  }
  return out.str();
}
ExportResult exportProgress(const std::filesystem::path& directory, const std::vector<Lesson>& lessons, const Controller& app) {
  std::error_code ec;
  std::filesystem::create_directories(directory, ec);
  if (ec) return {{}, "Cannot create the export folder. Check its location and permissions."};
  const auto bytes=progressCsv(lessons,app);
  for (int i=1; i<=10000; ++i) {
    auto path=directory / ("cwaypoint-progress-"+std::to_string(i)+".csv");
    int fd=open(path.c_str(),O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW,0600);
    if (fd<0) { if (errno==EEXIST) continue; return {{}, "Cannot create the report. Check folder permissions and free disk space."}; }
    size_t pos=0; bool ok=true;
    while (pos<bytes.size()) {
      auto n=write(fd,bytes.data()+pos,bytes.size()-pos);
      if (n<0 && errno==EINTR) continue;
      if (n<=0) { ok=false; break; }
      pos+=static_cast<size_t>(n);
    }
    if (ok) ok=fsync(fd)==0;
    if (close(fd)!=0) ok=false;
    if (!ok) return {path,"Report could not be completed. Do not use this partial file; check free disk space and export again."};
    return {path,{}};
  }
  return {{},"The export folder already contains 10,000 numbered reports. Choose another profile folder."};
}
}
