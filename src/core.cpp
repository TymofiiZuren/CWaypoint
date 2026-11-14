#include "core.hpp"
#include "lab.hpp"
#include <algorithm>
#include <cerrno>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <sys/file.h>
#include <unistd.h>
namespace cw {
// Use the learner's local calendar date, encoded independently of DST.
int localStudyDay() {
  std::time_t now = std::time(nullptr);
  std::tm date{};
  localtime_r(&now, &date);
  date.tm_hour = date.tm_min = date.tm_sec = 0;
  return static_cast<int>(timegm(&date) / 86400);
}
int studyStreak(const std::set<int>& days, int today) {
  int end = days.count(today) ? today : today - 1;
  int streak = 0;
  while (days.count(end)) {
    ++streak;
    --end;
  }
  return streak;
}
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
namespace {
std::string validateQuestion(const Question& q,std::set<std::string>& ids) {

      if (q.id.empty() || !ids.insert(q.id).second || q.prompt.empty() ||
          q.answer.empty() || q.prompt.size() > 8192 || q.code.size() > 8192)
        return "Invalid or duplicate question";
      for (const auto &h : q.hints)
        if (h.empty() || h.size() > 8192)
          return "Invalid authored hints";
      if (q.type == QuestionType::Choice &&
          std::find(q.choices.begin(), q.choices.end(), q.answer) ==
              q.choices.end())
        return "Choice answer missing";
    return "";
}
}
std::string validate(const std::vector<Lesson> &ls) {
  std::set<std::string> ids;
  if (ls.empty() || ls.size() > 100)
    return "Invalid lesson count";
  for (const auto &l : ls) {
    if (l.id.empty() || !ids.insert(l.id).second || l.title.empty() ||
        l.questions.size() != 3 || l.revision < 1)
      return "Invalid lesson record";
    for (const auto& q:l.questions) {
      auto error=validateQuestion(q,ids);
      if(!error.empty()) return error;
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
namespace {
bool readManifest(const std::filesystem::path& path,unsigned long& generation) {
  std::ifstream f(path); std::string magic,extra; int version=0;
  return (f >> magic >> version >> generation) && magic=="CWAYPOINT" &&
         version==1 && generation!=0 && !(f >> extra);
}
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
    if (!readManifest(manifest,generation)) {
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
namespace {
bool readProfileHeader(std::istream& f, Profile& p, size_t& count) {
  if (!(f >> std::quoted(p.lesson) >> p.step >> p.textSize >> p.recovered >> count)) return false;
  return p.step>=0 && p.step<=3 && p.textSize>=16 && p.textSize<=24 && count<=5000;
}
bool validEvidence(const std::string& id,const Evidence& e,const Profile& p) {
  if (id.empty() || id.size()>128 || p.evidence.count(id)) return false;
  if (e.attempts<0 || e.attempts>1000000 || e.revision<1) return false;
  if (e.independent && (!e.solved || e.attempts<1)) return false;
  return true;
}
bool readEvidence(std::istream& f,Profile& p,size_t count) {
  for (size_t i=0;i<count;++i) {
    std::string id; Evidence e; bool review;
    if (!(f >> std::quoted(id) >> e.revision >> e.attempts >> e.issued >> e.helped >> e.solved >> e.independent >> review)) return false;
    if (!validEvidence(id,e,p)) return false;
    p.evidence[id]=e;
    if (review) p.review.insert(id);
  }
  return true;
}
bool validResume(const Profile& p) {
  if (p.question<0 || p.question>2 || p.walkthrough<0 || p.walkthrough>31 || p.resumeRevision<0) return false;
  if (p.step==1 && p.question>1) return false;
  if (p.step==2 && p.question!=2) return false;
  if (p.step!=0 && p.walkthrough!=0) return false;
  return !((p.step==0 || p.step==3) && p.question!=0);
}
bool readResume(std::istream& f,Profile& p,int version) {
  if (version<2) { p.question=p.step==2 ? 2 : 0; return true; }
  std::string tag;
  if (!(f >> tag >> p.question >> p.walkthrough >> p.resumeRevision) || tag!="RESUME") return false;
  return validResume(p);
}
bool readActivity(std::istream& f,Profile& p,int version) {
  if (version<3) return true;
  std::string tag; size_t count;
  if (!(f >> tag >> count) || tag!="ACTIVITY" || count>50000) return false;
  for(size_t i=0;i<count;++i) {
    int day;
    if (!(f >> day) || day<0 || day>100000 || !p.studyDays.insert(day).second) return false;
  }
  return true;
}
bool validHash(const std::string& hash){
 if(hash.size()!=64)return false;
 for(unsigned char c:hash)if(!std::isxdigit(c))return false;
 return true;
}
bool readAchievements(std::istream& f,Profile& p,int version){
 if(version<4)return true;
 std::string tag,id;size_t count;
 if(!(f>>tag>>p.labMode>>count) || tag!="LAB" || count>100)return false;
 for(size_t i=0;i<count;++i){
  Achievement a;
  if(!(f>>std::quoted(id)>>a.assessment>>a.testRevision>>a.day>>std::quoted(a.sourceHash)))return false;
  if(id.empty() || id.size()>128 || a.assessment<1 || a.testRevision<1 || a.day<0 || a.day>100000 || !validHash(a.sourceHash) || p.achievements.count(id))return false;
  p.achievements[id]=a;
 }
 return true;
}
bool readCompletions(std::istream& f,Profile& p,int version){
 if(version<4)return true;
 std::string tag,id;size_t count;int day;
 if(!(f>>tag>>count) || tag!="COMPLETIONS" || count>5000)return false;
 for(size_t i=0;i<count;++i){
  if(!(f>>day>>std::quoted(id)) || day<0 || day>100000 || id.empty() || id.size()>128)return false;
  if(!p.completionDays.insert({day,id}).second)return false;
 }
 return true;
}
}
namespace {
bool readLabState(std::istream& f,Profile& p,int version){
 return readAchievements(f,p,version) && readCompletions(f,p,version);
}
bool preserveLegacy(const std::filesystem::path& primary,const std::string& bytes){
 std::istringstream header(bytes);std::string magic;int version=0;header>>magic>>version;
 if(version>=4)return true;
 std::error_code ec;auto old=primary;old+=".pre-v4";
 if(std::filesystem::exists(old,ec))return !ec;
 return std::filesystem::copy_file(primary,old,std::filesystem::copy_options::none,ec) && !ec;
}
}
namespace {
bool readProfileBody(std::istream& f,Profile& p,int version){size_t count;return readProfileHeader(f,p,count)&&readEvidence(f,p,count)&&readResume(f,p,version)&&readActivity(f,p,version)&&readLabState(f,p,version);}
}
bool Store::readSnapshot(const std::filesystem::path& path,Profile& result) {
  std::error_code ec;
  const auto size=std::filesystem::file_size(path,ec);
  if (ec || size>1024*1024) return false;
  std::ifstream f(path);
  std::string magic; unsigned long gen; int version;
  if (!(f >> magic >> version >> gen) || magic!="CWPROFILE" || gen!=generation) return false;
  if (version<1) return false;
  if (version>4) return false;
  Profile p;if(!readProfileBody(f,p,version))return false;
  std::string end,extra;
  if (!(f >> end) || end!="END" || (f >> extra)) return false;
  result=std::move(p);
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
    if (header >> magic >> version && magic == "CWPROFILE" && (version < 1 || version > 4)) {
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
  s << "CWPROFILE 4 " << generation << '\n'
    << std::quoted(p.lesson) << ' ' << p.step << ' ' << p.textSize << ' '
    << p.recovered << ' ' << p.evidence.size() << '\n';
  for (const auto &[id, e] : p.evidence)
    s << std::quoted(id) << ' ' << e.revision << ' ' << e.attempts << ' '
      << e.issued << ' ' << e.helped << ' ' << e.solved << ' ' << e.independent
      << ' ' << p.review.count(id) << '\n';
  s << "RESUME " << p.question << ' ' << p.walkthrough << ' '
    << p.resumeRevision << "\nACTIVITY " << p.studyDays.size();
  for (int day : p.studyDays)
    s << ' ' << day;
  s << "\nLAB "<<p.labMode<<' '<<p.achievements.size()<<'\n';
  for(const auto& [id,a]:p.achievements)s<<std::quoted(id)<<' '<<a.assessment<<' '<<a.testRevision<<' '<<a.day<<' '<<std::quoted(a.sourceHash)<<'\n';
  s<<"COMPLETIONS "<<p.completionDays.size()<<'\n';
  for(const auto& [day,id]:p.completionDays)s<<day<<' '<<std::quoted(id)<<'\n';
  s<<"END\n";
  if(s.str().size()>1024*1024)return false;
  auto primary = root / ("generation-" + std::to_string(generation) + ".save");
  Profile previous;
  if (readSnapshot(primary, previous)) {
    std::ifstream in(primary, std::ios::binary);
    std::ostringstream bytes;
    bytes << in.rdbuf();
    if(!preserveLegacy(primary,bytes.str()))return false;
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
  fresh.labMode = p.labMode;
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
  unsavedChanges = !ok;
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
    if(!profile.labMode)profile.studyDays.insert(localStudyDay());
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
namespace {
bool validLessonPosition(const std::vector<Lesson>& lessons,int l,int step,int question,int walkthrough) {
  return !(l < 0 || l >= static_cast<int>(lessons.size()) || step < 0 || step > 3 ||
      question < 0 || question > 2 || (step == 1 && question > 1) ||
      walkthrough < 0 || walkthrough > 31 ||
      (step == 0 && walkthrough > 0 &&
       walkthrough >= static_cast<int>(lessons[l].steps.size())));
}
}
bool Controller::resume(int l, int step, int question, int walkthrough) {
  if (!validLessonPosition(lessons,l,step,question,walkthrough)) {
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
  unsavedChanges = false;
  notice = store.status();
  return true;
}
bool Controller::enableLabMode(){
 if(profile.labMode)return true;profile.labMode=true;return persist();
}
bool Controller::lessonCompleted(int l)const{
 const auto& lesson=lessons.at(l);auto it=profile.achievements.find(lesson.id);
 return it!=profile.achievements.end() && it->second.assessment==lesson.revision && it->second.testRevision==labFor(lesson.id).revision && completed(l)==2;
}
int Controller::completedToday(int day)const{
 int count=0;for(const auto& item:profile.completionDays)if(item.first==day)++count;return count;
}
bool Controller::completeLesson(int l,const LabResult& r,unsigned long gen,int day){
 if(gen!=store.activeGeneration() || !profile.labMode || !store.writable())return false;
 if(r.testRevision!=labFor(lessons.at(l).id).revision)return false;
 if(completed(l)!=2 || !r.allPassed() || !validHash(r.sourceHash))return false;
 if(day<0 || day>100000 || profile.completionDays.size()>=5000)return false;
 auto before=profile;const auto& lesson=lessons.at(l);
 profile.achievements[lesson.id]={lesson.revision,r.testRevision,day,r.sourceHash};
 profile.completionDays.insert({day,lesson.id});profile.studyDays.insert(day);
 if(persist())return true;
 profile=std::move(before);return false;
}
} // namespace cw
