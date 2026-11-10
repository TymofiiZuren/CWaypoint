# First lesson integration

Include src/learning/lesson.hpp and link lesson.cpp. Call variablesLesson() to obtain the authored record. Call evaluate(exercise, answer) for correctness only. The controller must validate catalogue/exercise identity, own completion and durable assistance/attempt evidence, and decide when explanations are revealed. This fixture does not implement those controller responsibilities.

Predict-output comparison normalises CRLF and removes at most one final newline from each side; meaningful spaces and case remain significant. Fill-token comparison trims surrounding ASCII whitespace. Empty or whitespace-only input is invalid. Single-choice is provisional exact-ID comparison; a choices catalogue and unknown-option validation are required before using it in production exercises.

The Makefile remains a standalone core harness alongside the integrated CMake build. Run make test and make demo from the repository root. No Raylib or network dependency is needed for this first handoff.

## Graphical application integration

CMake links `lesson.cpp` and `lesson_adapter.cpp` into the application core. `sampleLessons()` adapts `variablesLesson()` instead of maintaining a second variables record. The UI still calls `cw::evaluate()` through the controller; the standalone evaluator remains available to the authoring harness. Neither evaluator changes progress directly.

The bounded adapter accepts the checked-in author revision 1 and maps it to application lesson `c-variables`, assessment revision 2. Question IDs map in order from `c.variables.output`, `c.variables.token`, `c.variables.fresh` to `c-var-p1`, `c-var-p2`, `c-var-fresh`. Existing revision-1 application evidence is retained in storage but excluded from current completion and review. Revision-aware Continue returns to the explanation. No save-format change is needed.

This first adapter requires two practice records and one final fresh check, output/token types, one nonblank accepted answer no longer than the input limit, and exactly three nonblank hints per question. Expected IDs, types and author revision are checked explicitly. Multiple accepted answers and authored single-choice records are rejected rather than silently reduced. The existing walkthrough is retained only while the example source matches the statements it describes. Author changes require reviewing this mapping and advancing the application assessment revision when assessment meaning changes.

`authored_lesson_integration` in CTest covers rejected shapes, content preservation, output/token evaluator parity, old assessment evidence and restart. `--authored-test` exercises the actual graphical handlers with an isolated profile, including a wrong answer, hint, successful retry and separate independent fresh check. Automated text entry uses the application's input model; native keyboard and clipboard acceptance remains separate.
