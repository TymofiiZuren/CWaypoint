#include "core.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace cw;
void check(bool ok, const char *s) {
  if (!ok)
    throw std::runtime_error(s);
}
int main() {
  check(studyStreak({}, 100) == 0, "empty streak");
  check(studyStreak({98, 99, 100}, 100) == 3, "consecutive days");
  check(studyStreak({98, 99}, 100) == 2, "today still available");
  check(studyStreak({98}, 100) == 0, "missed day expires streak");
  check(studyStreak({98, 100, 101}, 100) == 1, "gap and future day");
  auto activityRoot = std::filesystem::temp_directory_path() /
      ("cwaypoint-activity-" + std::to_string(getpid()));
  {
    Store store(activityRoot);
    auto lessons = sampleLessons();
    Controller controller(lessons, store);
    controller.submit(0, 0, "incorrect");
    check(controller.profile.studyDays.empty(), "wrong answer earns no activity");
    controller.retry();
    controller.submit(0, 0, lessons[0].questions[0].answer);
    controller.retry();
    controller.submit(0, 0, lessons[0].questions[0].answer);
    check(controller.profile.studyDays.size() == 1, "same day counted once");
  }
  {
    Store store(activityRoot);
    auto profile = store.load();
    check(profile.studyDays.count(localStudyDay()) == 1, "activity restored");
    check(store.reset(profile) && profile.studyDays.empty(), "reset clears activity");
  }
  std::filesystem::remove_all(activityRoot);

  auto dir = std::filesystem::temp_directory_path() /
             ("cwaypoint-test-" + std::to_string(getpid()));
  std::filesystem::create_directories(dir);
  try {
    auto lessons = sampleLessons();
    check(validate(lessons).empty(), "sample contract");
    check(validateWalkthrough(lessons[0].steps).empty(), "valid walkthrough");
    auto brokenSteps = lessons[0].steps;
    brokenSteps[0].state.clear();
    check(!validateWalkthrough(brokenSteps).empty(),
          "broken walkthrough isolated");
    auto isolated = lessons;
    isolated[0].steps = brokenSteps;
    check(validate(isolated).empty(),
          "bad optional walkthrough does not reject practice");
    auto comparison = comparisons()[0];
    comparison.cpp.clear();
    check(!validateComparison(comparison).empty(),
          "incomplete comparison rejected");
    auto bad = lessons;
    bad[0].questions[1].id = bad[0].questions[0].id;
    check(!validate(bad).empty(), "duplicate IDs");
    const auto &q = lessons[0].questions[0];
    check(evaluate(q, "").kind == Verdict::Invalid, "empty is invalid");
    check(evaluate(q, q.answer).kind == Verdict::Correct, "accepted answer");
    Question output{"out", "Output?",       "3",
                    {},    {"a", "b", "c"}, QuestionType::Output};
    check(evaluate(output, "3\r\n").kind == Verdict::Correct,
          "CRLF final newline");
    check(evaluate(output, " 3").kind == Verdict::Incorrect,
          "spaces significant");
    {
      Store store(dir);
      check(store.writable(), "first writable");
      Controller c(lessons, store);
      check(c.issue(0, 2), "fresh issuance");
      check(c.help(0, 2), "help persistence");
      check(c.submit(0, 2, "wrong").kind == Verdict::Incorrect,
            "wrong attempt");
      check(c.submit(0, 2, lessons[0].questions[2].answer).kind ==
                Verdict::Invalid,
            "double submission ignored");
      c.retry();
      check(c.submit(0, 2, lessons[0].questions[2].answer).kind ==
                Verdict::Correct,
            "correct retry");
      check(c.status(0, 2) == "Solved with practice", "assisted status");
      check(c.profile.review.empty(), "review cleared");
      Store second(dir);
      check(!second.writable(), "second process read only");
    }
    {
      Store store(dir);
      Controller c(lessons, store);
      check(c.status(0, 2) == "Solved with practice", "restart preserves help");
      check(c.issue(1, 2), "other fresh");
      check(c.submit(1, 2, lessons[1].questions[2].answer).kind ==
                Verdict::Correct,
            "independent success");
      check(c.status(1, 2) == "Solved independently on first attempt",
            "independent label");
      c.help(1, 2);
      c.retry();
      c.submit(1, 2, lessons[1].questions[2].answer);
      check(c.status(1, 2) == "Solved independently on first attempt",
            "later practice preserves first success");
      check(c.reset(), "reset");
      check(c.profile.evidence.empty(), "reset empty");
      Store locked(dir);
      Controller reader(lessons, locked);
      check(reader.profile.evidence.empty(),
            "locked reader follows current generation");
    }
    {
      Store store(dir);
      Controller c(lessons, store);
      check(c.profile.evidence.empty(), "reset restart");
      check(c.issue(0, 2), "issued for crash");
      c.submit(0, 2, "bad");
    }
    {
      Store store(dir);
      Controller c(lessons, store);
      check(c.profile.evidence.at(lessons[0].questions[2].id).attempts == 1,
            "attempt survives crash");
      c.help(0, 2); // Creates a same-generation backup.
    }
    {
      std::ofstream f(dir / "generation-2.save", std::ios::trunc);
      f << "broken";
    }
    {
      Store store(dir);
      Controller c(lessons, store);
      check(c.profile.recovered, "backup provenance");
      check(c.status(0, 2) == "History recovered; first attempt unverified",
            "uncertain history");
      check(c.save(), "recovered session saves");
      check(std::filesystem::exists(dir / "generation-2.save.preserved-1"),
            "damaged primary preserved before replacement");
    }
    auto reviewDir = dir / "review";
    {
      Store store(reviewDir);
      Controller c(lessons, store);
      check(c.reviewItems().empty(), "review initially empty");
      c.submit(0, 0, "wrong");
      c.retry();
      c.submit(0, 0, "wrong");
      c.submit(1, 2, "wrong");
      check(c.reviewItems().size() == 2, "one review item per question");
      check(c.reviewItems("C").size() == 1, "review track filtering");
      c.profile.review.insert("missing-question");
      check(c.reviewItems().size() == 2, "unknown historical IDs hidden");
      c.retry();
      c.help(0, 0);
      c.submit(0, 0, lessons[0].questions[0].answer);
      check(c.reviewItems().size() == 1, "successful review clears item");
      check(c.status(0, 0) == "Solved with practice",
            "review retains attempt and help history");
    }
    {
      Store store(reviewDir);
      Controller c(lessons, store);
      auto items = c.reviewItems();
      check(items.size() == 1 && items[0].lesson == 1 && items[0].question == 2,
            "remaining review survives restart");
      auto revised = lessons;
      revised[1].revision++;
      Controller updated(revised, store);
      check(updated.reviewItems().empty(),
            "old revision review not applied to new assessment");
      updated.issue(1, 2);
      check(updated.reviewItems().empty(),
            "new issuance must not resurrect old revision mistake");
    }
    auto exactResume = dir / "exact-resume";
    {
      Store store(exactResume);
      Controller c(lessons, store);
      check(c.resume(0, 1, 1), "save second practice position");
      check(!c.resume(0, 1, 2), "fresh index invalid in practice");
    }
    {
      Store store(exactResume);
      Controller c(lessons, store);
      check(c.profile.step == 1 && c.profile.question == 1,
            "second practice restored");
      check(c.resume(0, 0, 0, 2), "save walkthrough position");
    }
    {
      Store store(exactResume);
      Controller c(lessons, store);
      check(c.profile.walkthrough == 2, "walkthrough restored");
      auto shorter = lessons;
      shorter[0].steps.resize(1);
      Controller shortened(shorter, store);
      check(shortened.profile.walkthrough == 0,
            "changed walkthrough bounds checked at load");
      auto revised = lessons;
      revised[0].revision++;
      Controller updated(revised, store);
      check(updated.profile.step == 0 && updated.profile.walkthrough == 0,
            "updated lesson restarts explanation");
    }
    auto legacy = dir / "legacy";
    std::filesystem::create_directories(legacy);
    {
      std::ofstream f(legacy / "manifest");
      f << "CWAYPOINT 1 1\n";
    }
    {
      std::ofstream f(legacy / "generation-1.save");
      f << "CWPROFILE 1 1\n\"c-variables\" 1 18 0 1\n\"c-var-p1\" 1 1 1 0 1 1 "
           "0\nEND\n";
    }
    {
      Store store(legacy);
      Controller c(lessons, store);
      check(c.completed(0) == 1 && c.profile.question == 0,
            "version one progress remains readable");
      check(c.resume(0, 1, 1), "legacy upgrades on save");
    }
    {
      Store store(legacy);
      Controller c(lessons, store);
      check(c.completed(0) == 1 && c.profile.question == 1,
            "upgraded save retains progress and exact resume");
      check(std::filesystem::exists(legacy / "generation-1.save.bak"),
            "legacy backup retained");
      std::ifstream old(legacy / "generation-1.save.bak");
      std::string magic;
      int version = 0;
      old >> magic >> version;
      check(version == 1, "previous format backup kept during upgrade");
    }
    auto journey = dir / "journey";
    {
      Store store(journey);
      Controller c(lessons, store);
      for (int l = 0; l < 2; l++)
        for (int q = 0; q < 3; q++) {
          if (l == 0 && q == 0) {
            check(c.nextLesson("C") == 0, "first suggested C lesson");
            check(c.nextLesson("C", 0) == 2, "next same-track lesson");
            check(c.nextLesson("C++") == 1, "separate CPP recommendation");
            check(c.nextLesson("unknown") == -1, "no invented lessons");
          }
          check(c.issue(l, q), "journey issuance");
          c.retry();
          check(c.submit(l, q, lessons[l].questions[q].answer).kind ==
                    Verdict::Correct,
                "C and CPP full lesson");
        }
      check(c.completed(0) == 2 && c.completed(1) == 2,
            "both tracks practice complete");
      check(c.nextLesson("C") == 2, "completed practice skipped");
      check(c.nextLesson("C++") == -1, "finished track has no suggestion");
      c.resume(1, 3);
    }
    {
      Store store(journey);
      Controller c(lessons, store);
      check(c.completed(0) == 2 && c.completed(1) == 2,
            "both tracks completion restored");
      check(c.profile.lesson == lessons[1].id && c.profile.step == 3,
            "resume restored");
      check(c.status(0, 2) == "Solved independently on first attempt",
            "fresh evidence restored");
    }
    auto newer = dir / "newer";
    {
      Store store(newer);
      Controller c(lessons, store);
      c.issue(0, 2);
      c.help(0, 2);
    }
    {
      std::ofstream f(newer / "generation-1.save");
      f << "CWPROFILE 999 1\n";
    }
    {
      Store store(newer);
      Controller c(lessons, store);
      check(!store.writable(),
            "newer primary never overwritten through backup");
    }
    auto failing = dir / "failing";
    {
      Store store(failing);
      Controller c(lessons, store);
      check(c.issue(0, 2), "fresh before write failure");
      std::filesystem::create_directory(failing / "generation-1.save.tmp");
      check(!c.help(0, 2),
            "no hint displayed before failed assistance barrier");
      check(c.submit(0, 2, lessons[0].questions[2].answer).kind ==
                Verdict::Invalid,
            "no result displayed before failed save");
      c.retry();
      check(c.submit(0, 2, lessons[0].questions[2].answer).kind ==
                Verdict::Invalid,
            "pending save blocks retry submission");
      check(c.profile.evidence.at("c-var-fresh").attempts == 1,
            "failed-save duplicate does not inflate attempts");
      std::filesystem::rename(failing / "generation-1.save.tmp",
                              failing / "blocked-write-fixture");
      check(c.save(), "retry save persists pending result");
      Controller restored(lessons, store);
      check(restored.profile.evidence.at("c-var-fresh").attempts == 1 &&
                restored.solved(0, 2),
            "pending result recovers once");
      check(c.submit(0, 2, lessons[0].questions[2].answer).kind ==
                Verdict::Invalid,
            "saved pending result still requires explicit retry");
      c.retry();
      check(c.submit(0, 2, lessons[0].questions[2].answer).kind ==
                Verdict::Correct,
            "explicit retry admitted after save recovery");
    }
    auto broken = dir / "blocked";
    std::filesystem::create_directories(broken);
    {
      std::ofstream f(broken / "manifest");
      f << "CWAYPOINT 999 1";
    }
    {
      Store store(broken);
      Controller c(lessons, store);
      check(!store.writable(), "unsupported preserved");
      check(!c.issue(0, 2), "fresh withheld");
      check(c.submit(0, 0, q.answer).kind == Verdict::Correct,
            "temporary practice");
    }
    std::filesystem::remove_all(dir);
    std::cout << "All learning and persistence checks passed\n";
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
