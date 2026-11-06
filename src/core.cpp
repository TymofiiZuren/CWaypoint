#include "core.hpp"
#include <algorithm>
#include <cerrno>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <sys/file.h>
#include <unistd.h>
namespace cw {
namespace {
std::string trim(std::string s) {
  auto a = s.find_first_not_of(" \t\r\n");
  return a == s.npos ? "" : s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}
std::string output(std::string s) {
  for (size_t i = 0; (i = s.find("\r\n", i)) != s.npos;)
    s.replace(i, 2, "\n");
  if (!s.empty() && s.back() == '\n')
    s.pop_back();
  return s;
}
bool atomicWrite(const std::filesystem::path &path, const std::string &bytes) {
  auto tmp = path;
  tmp += ".tmp";
  int fd = open(tmp.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0600);
  if (fd < 0)
    return false;
  size_t pos = 0;
  bool ok = true;
  while (pos < bytes.size()) {
    auto n = write(fd, bytes.data() + pos, bytes.size() - pos);
    if (n <= 0) {
      ok = false;
      break;
    }
    pos += static_cast<size_t>(n);
  }
  if (ok)
    ok = fsync(fd) == 0;
  if (close(fd) != 0)
    ok = false;
  if (ok)
    ok = rename(tmp.c_str(), path.c_str()) == 0;
  if (ok) {
    int dir = open(path.parent_path().c_str(), O_RDONLY);
    if (dir < 0)
      return false;
    ok = fsync(dir) == 0;
    close(dir);
  }
  return ok;
}
} // namespace
std::string validate(const std::vector<Lesson> &ls) {
  std::set<std::string> ids;
  if (ls.empty() || ls.size() > 100)
    return "Invalid lesson count";
  for (const auto &l : ls) {
    if (l.id.empty() || !ids.insert(l.id).second || l.title.empty() ||
        l.questions.size() != 3 || l.revision < 1)
      return "Invalid lesson record";
    for (const auto &q : l.questions) {
      if (q.id.empty() || !ids.insert(q.id).second || q.prompt.empty() ||
          q.answer.empty() || q.prompt.size() > 8192)
        return "Invalid or duplicate question";
      for (const auto &h : q.hints)
        if (h.empty() || h.size() > 8192)
          return "Invalid authored hints";
      if (q.type == QuestionType::Choice &&
          std::find(q.choices.begin(), q.choices.end(), q.answer) ==
              q.choices.end())
        return "Choice answer missing";
    }
  }
  return "";
}
std::string validateWalkthrough(const std::vector<Step> &steps) {
  if (steps.empty() || steps.size() > 32)
    return "Walkthrough needs between 1 and 32 steps.";
  std::size_t total = 0;
  for (const auto &step : steps) {
    if (trim(step.code).empty() || trim(step.state).empty() ||
        trim(step.explanation).empty())
      return "A walkthrough step is incomplete.";
    if (step.code.size() > 8192 || step.state.size() > 4096 ||
        step.explanation.size() > 4096)
      return "A walkthrough step exceeds its text limit.";
    total += step.code.size() + step.state.size() + step.explanation.size();
  }
  return total > 131072 ? "Walkthrough exceeds its total text limit." : "";
}
std::string validateComparison(const Comparison &record) {
  if (trim(record.title).empty() || trim(record.c).empty() ||
      trim(record.cpp).empty() || trim(record.explanation).empty())
    return "Both language examples and an explanation are required.";
  if (record.title.size() > 256 || record.c.size() > 8192 ||
      record.cpp.size() > 8192 || record.explanation.size() > 4096)
    return "Comparison exceeds its text limit.";
  return "";
}
Result evaluate(const Question &q, const std::string &answer) {
  if (trim(answer).empty())
    return {Verdict::Invalid, "Enter an answer before checking."};
  if (answer.size() > 1024)
    return {Verdict::Invalid, "Keep your answer under 1,024 characters."};
  bool correct = q.type == QuestionType::Output
                     ? output(answer) == output(q.answer)
                     : trim(answer) == q.answer;
  return {correct ? Verdict::Correct : Verdict::Incorrect,
          correct ? "Correct. " + q.explanation
                  : "Not quite. Try again or use an authored hint."};
}
Store::Store(std::filesystem::path p) : root(std::move(p)) {
  std::error_code ec;
  std::filesystem::create_directories(root, ec);
  if (ec) {
    message = "Cannot open progress folder. Temporary practice is available.";
    return;
  }
  lockFd = open((root / "profile.lock").c_str(), O_RDWR | O_CREAT | O_NOFOLLOW,
                0600);
  canWrite = lockFd >= 0 && flock(lockFd, LOCK_EX | LOCK_NB) == 0;
  if (!canWrite)
    message = "Progress is open elsewhere or locked. This window uses "
              "temporary practice.";
  auto manifest = root / "manifest";
  if (std::filesystem::exists(manifest, ec)) {
    std::ifstream f(manifest);
    std::string magic, extra;
    int version = 0;
    if (!(f >> magic >> version >> generation) || magic != "CWAYPOINT" ||
        version != 1 || generation == 0 || (f >> extra)) {
      canWrite = false;
      message = "Progress format is unsupported or damaged. Files preserved; "
                "temporary practice is available.";
    }
  } else if (ec) {
    canWrite = false;
    message = "Cannot read progress. Files preserved.";
  } else {
    // An absent manifest beside previous generations is damage, not first
    // launch.
    for (const auto &entry : std::filesystem::directory_iterator(root, ec))
      if (entry.path().extension() == ".save" ||
          entry.path().extension() == ".bak") {
        canWrite = false;
        message = "Progress manifest is missing. Existing files preserved.";
        break;
      }
    if (canWrite && !atomicWrite(manifest, "CWAYPOINT 1 1\n")) {
      canWrite = false;
      message =
          "Cannot create progress manifest. Temporary practice is available.";
    }
  }
}
Store::~Store() {
  if (lockFd >= 0)
    close(lockFd);
}
bool Store::readSnapshot(const std::filesystem::path &p, Profile &result) {
  std::error_code ec;
  auto size = std::filesystem::file_size(p, ec);
  if (ec || size > 1024 * 1024)
    return false;
  std::ifstream f(p);
  Profile v;
  std::string magic;
  unsigned long gen;
  int version;
  size_t count;
  if (!(f >> magic >> version >> gen) || magic != "CWPROFILE" ||
      (version != 1 && version != 2) || gen != generation)
    return false;
  if (!(f >> std::quoted(v.lesson) >> v.step >> v.textSize >> v.recovered >>
        count) ||
      v.step < 0 || v.step > 3 || v.textSize < 16 || v.textSize > 24 ||
      count > 5000)
    return false;
  for (size_t i = 0; i < count; i++) {
    std::string id;
    Evidence e;
    bool review;
    if (!(f >> std::quoted(id) >> e.revision >> e.attempts >> e.issued >>
          e.helped >> e.solved >> e.independent >> review) ||
        id.empty() || id.size() > 128 || e.attempts < 0 ||
        e.attempts > 1000000 || e.revision < 1 || v.evidence.count(id) ||
        (e.independent && (!e.solved || e.attempts < 1)))
      return false;
    v.evidence[id] = e;
    if (review)
      v.review.insert(id);
  }
  if (version == 2) {
    std::string tag;
    if (!(f >> tag >> v.question >> v.walkthrough >> v.resumeRevision) ||
        tag != "RESUME" || v.question < 0 || v.question > 2 ||
        v.walkthrough < 0 || v.walkthrough > 31 || v.resumeRevision < 0 ||
        (v.step == 1 && v.question > 1) || (v.step == 2 && v.question != 2) ||
        (v.step != 0 && v.walkthrough != 0) ||
        ((v.step == 0 || v.step == 3) && v.question != 0))
      return false;
  } else
    v.question = v.step == 2 ? 2 : 0;
  std::string end, extra;
  if (!(f >> end) || end != "END" || (f >> extra))
    return false;
  result = std::move(v);
  return true;
}
Profile Store::load() {
  Profile p;
  auto primary = root / ("generation-" + std::to_string(generation) + ".save");
  auto backup = primary;
  backup += ".bak";
  std::error_code ec;
  if (readSnapshot(primary, p)) {
    if (canWrite)
      message = "Progress saved on this Mac";
    return p;
  }
  // A newer primary is not ordinary corruption: never downgrade it through a
  // backup.
  {
    std::ifstream header(primary);
    std::string magic;
    int version = 0;
    if (header >> magic >> version && magic == "CWPROFILE" && version != 1 &&
        version != 2) {
      canWrite = false;
      message = "This progress needs a different app version. Files preserved; "
                "temporary practice is available.";
      return p;
    }
  }
  if (readSnapshot(backup, p)) {
    p.recovered = true;
    message = "Recovered backup. First-attempt history is unverified; damaged "
              "primary preserved.";
    return p;
  }
  if (std::filesystem::exists(primary, ec) ||
      std::filesystem::exists(backup, ec) || ec) {
    canWrite = false;
    message = "Cannot read progress or backup. Files preserved; temporary "
              "practice is available.";
  }
  return p;
}
bool Store::writeSnapshot(const Profile &p) {
  std::ostringstream s;
  s << "CWPROFILE 2 " << generation << '\n'
    << std::quoted(p.lesson) << ' ' << p.step << ' ' << p.textSize << ' '
    << p.recovered << ' ' << p.evidence.size() << '\n';
  for (const auto &[id, e] : p.evidence)
    s << std::quoted(id) << ' ' << e.revision << ' ' << e.attempts << ' '
      << e.issued << ' ' << e.helped << ' ' << e.solved << ' ' << e.independent
      << ' ' << p.review.count(id) << '\n';
  s << "RESUME " << p.question << ' ' << p.walkthrough << ' '
    << p.resumeRevision << "\nEND\n";
  auto primary = root / ("generation-" + std::to_string(generation) + ".save");
  Profile previous;
  if (readSnapshot(primary, previous)) {
    std::ifstream in(primary, std::ios::binary);
    std::ostringstream bytes;
    bytes << in.rdbuf();
    auto backup = primary;
    backup += ".bak";
    if (!atomicWrite(backup, bytes.str()))
      return false;
  } else {
    std::error_code ec;
    if (std::filesystem::exists(primary, ec)) {
      // Keep a separate copy before replacing damaged data after recovery.
      bool preserved = false;
      for (int i = 1; i <= 1000; ++i) {
        auto copy = primary;
        copy += ".preserved-" + std::to_string(i);
        if (std::filesystem::exists(copy, ec))
          continue;
        preserved = std::filesystem::copy_file(
            primary, copy, std::filesystem::copy_options::none, ec);
        break;
      }
      if (!preserved || ec)
        return false;
    } else if (ec)
      return false;
  }
  return atomicWrite(primary, s.str());
}
bool Store::save(const Profile &p) {
  if (!canWrite)
    return false;
  if (!writeSnapshot(p)) {
    message = "Not saved. Check the progress folder and choose Retry save.";
    return false;
  }
  message =
      p.recovered
          ? "Progress saved; recovered first-attempt history remains unverified"
          : "Progress saved on this Mac";
  return true;
}
bool Store::reset(Profile &p) {
  if (!canWrite) {
    message = "Reset unavailable while progress is read-only. Existing files "
              "preserved.";
    return false;
  }
  auto old = generation;
  generation++;
  Profile fresh;
  fresh.textSize = p.textSize;
  if (!writeSnapshot(fresh) ||
      !atomicWrite(root / "manifest",
                   "CWAYPOINT 1 " + std::to_string(generation) + "\n")) {
    generation = old;
    message = "Reset could not be saved. Your current progress is unchanged.";
    return false;
  }
  p = std::move(fresh);
  message = "Progress reset. A new profile generation is active.";
  return true;
}
Controller::Controller(const std::vector<Lesson> &ls, Store &s)
    : lessons(ls), store(s), profile(s.load()) {
  notice = s.status();
  if (!profile.lesson.empty()) {
    auto lesson = std::find_if(ls.begin(), ls.end(), [&](const Lesson &l) {
      return l.id == profile.lesson;
    });
    if (lesson == ls.end()) {
      profile.lesson.clear();
      profile.step = profile.question = profile.walkthrough =
          profile.resumeRevision = 0;
    } else if (profile.resumeRevision != 0 &&
               profile.resumeRevision != lesson->revision) {
      profile.step = profile.question = profile.walkthrough = 0;
      profile.resumeRevision = lesson->revision;
      notice = "Lesson updated. Continue starts at the current explanation; "
               "saved learning evidence is retained.";
    } else if (profile.step == 0 && profile.walkthrough > 0 &&
               (profile.walkthrough >= static_cast<int>(lesson->steps.size()) ||
                !validateWalkthrough(lesson->steps).empty())) {
      profile.walkthrough = 0;
      notice =
          "Walkthrough changed. Continue starts at the current explanation.";
    }
  }
}
bool Controller::persist() {
  bool ok = store.save(profile);
  notice = store.status();
  if (ok)
    pendingSubmission = false;
  return ok;
}
bool Controller::issue(int l, int q) {
  const auto &lesson = lessons.at(l);
  const auto &id = lesson.questions.at(q).id;
  auto it = profile.evidence.find(id);
  if (it != profile.evidence.end() && it->second.issued &&
      it->second.revision == lesson.revision)
    return true;
  if (q == 2 && !store.writable()) {
    notice = "Fresh check unavailable until progress can be saved. Practice "
             "and authored hints still work.";
    return false;
  }
  Profile before = profile;
  auto &e = profile.evidence[id];
  if (e.revision != lesson.revision) {
    e = Evidence{};
    profile.review.erase(id);
  }
  e.revision = lesson.revision;
  e.issued = true;
  bool ok = persist();
  if (q == 2 && !ok) {
    profile = std::move(before);
    return false;
  }
  return true;
}
bool Controller::help(int l, int q) {
  if (!issue(l, q))
    return false;
  auto &e = profile.evidence[lessons.at(l).questions.at(q).id];
  e.helped = true;
  bool saved = persist();
  return saved || !store.writable();
}
void Controller::retry() {
  if (pendingSubmission) {
    notice = "Retry save before starting another attempt.";
    return;
  }
  submitted.clear();
}
Result Controller::submit(int l, int q, const std::string &a) {
  if (pendingSubmission)
    return {Verdict::Invalid, "Retry save before starting another attempt."};
  const auto &question = lessons.at(l).questions.at(q);
  if (submitted == question.id)
    return {Verdict::Invalid,
            "Choose Try again before submitting another answer."};
  auto r = evaluate(question, a);
  if (r.kind == Verdict::Invalid)
    return r;
  if (!issue(l, q))
    return {Verdict::Invalid, notice};
  auto &e = profile.evidence[question.id];
  if (e.attempts >= 1000000)
    return {Verdict::Invalid, "Attempt limit reached."};
  e.attempts++;
  if (r.kind == Verdict::Correct) {
    e.solved = true;
    if (e.attempts == 1)
      e.independent = !e.helped && !profile.recovered && store.writable();
    profile.review.erase(question.id);
  } else
    profile.review.insert(question.id);
  submitted = question.id;
  pendingSubmission = store.writable();
  bool saved = persist();
  if (store.writable() && !saved) {
    return {Verdict::Invalid,
            "Could not save the result. Retry save before checking again."};
  }
  return r;
}
std::string Controller::status(int l, int q) const {
  const auto &lesson = lessons.at(l);
  auto it = profile.evidence.find(lesson.questions.at(q).id);
  if (it == profile.evidence.end() || it->second.revision != lesson.revision)
    return "Not attempted";
  const auto &e = it->second;
  if (profile.recovered)
    return "History recovered; first attempt unverified";
  if (e.solved)
    return e.independent ? "Solved independently on first attempt"
                         : "Solved with practice";
  return e.attempts ? "Keep practising"
         : e.helped ? "Help used"
                    : "Ready to try";
}
std::vector<ReviewItem>
Controller::reviewItems(const std::string &track) const {
  std::vector<ReviewItem> result;
  for (int l = 0; l < static_cast<int>(lessons.size()); ++l) {
    if (!track.empty() && lessons[l].track != track)
      continue;
    for (int q = 0; q < static_cast<int>(lessons[l].questions.size()); ++q) {
      const auto &id = lessons[l].questions[q].id;
      auto evidence = profile.evidence.find(id);
      if (profile.review.count(id) && evidence != profile.evidence.end() &&
          evidence->second.revision == lessons[l].revision)
        result.push_back({l, q});
    }
  }
  return result;
}
bool Controller::solved(int l, int q) const {
  const auto &lesson = lessons.at(l);
  auto it = profile.evidence.find(lesson.questions.at(q).id);
  return it != profile.evidence.end() &&
         it->second.revision == lesson.revision && it->second.solved;
}
int Controller::completed(int l) const {
  return static_cast<int>(solved(l, 0)) + static_cast<int>(solved(l, 1));
}
int Controller::nextLesson(const std::string &track, int after) const {
  // Suggest within this track, wrap to earlier gaps, and never gate on fresh
  // evidence.
  int count = static_cast<int>(lessons.size());
  if (count == 0)
    return -1;
  int start = after >= 0 && after < count ? after + 1 : 0;
  for (int offset = 0; offset < count; ++offset) {
    int index = (start + offset) % count;
    if (lessons[index].track == track && completed(index) < 2)
      return index;
  }
  return -1;
}
bool Controller::resume(int l, int step, int question, int walkthrough) {
  if (l < 0 || l >= static_cast<int>(lessons.size()) || step < 0 || step > 3 ||
      question < 0 || question > 2 || (step == 1 && question > 1) ||
      walkthrough < 0 || walkthrough > 31 ||
      (step == 0 && walkthrough > 0 &&
       walkthrough >= static_cast<int>(lessons[l].steps.size()))) {
    notice = "Cannot save an invalid lesson position.";
    return false;
  }
  profile.lesson = lessons.at(l).id;
  profile.step = step;
  profile.question = step == 2 ? 2 : step == 1 ? question : 0;
  profile.walkthrough = step == 0 ? walkthrough : 0;
  profile.resumeRevision = lessons[l].revision;
  return persist();
}
bool Controller::setTextSize(int s) {
  profile.textSize = std::clamp(s, 16, 24);
  return persist();
}
bool Controller::reset() {
  if (!store.reset(profile)) {
    notice = store.status();
    return false;
  }
  submitted.clear();
  pendingSubmission = false;
  notice = store.status();
  return true;
}
} // namespace cw
