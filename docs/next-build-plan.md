# CWaypoint next build plan

Date: 10 November 2026

## Outcome

Ship a local macOS build in which learners complete Oleksandr's authored C variables lesson through the existing graphical application, receive deterministic feedback and offline hints, and resume safely after restarting. Keep the existing C++ strings and C pointers lessons usable.

This is an implementation plan. The integration described below has not been built or validated yet.

## Why this build comes next

The executable obtains lessons from `sampleLessons()` in `src/sample_content.cpp`. Oleksandr's `variablesLesson()` and evaluator live separately in `src/learning/lesson.cpp`; CMake does not currently compile that module into the app. Both contributions therefore work independently, but the graphical application does not yet consume his authored record.

There are concrete contract differences: separate namespaces and lesson types, multiple accepted answers versus one answer, explicit fresh-check flags versus a fixed third-question position, and provisional single-choice validation in the standalone evaluator. Connecting the modules requires an explicit boundary rather than copying text and assuming the contracts match.

## Architecture decision

Recommended: add one small, validated adapter from the authored lesson to the application's existing `cw::Lesson` contract. Keep the controller, persistence and graphical evaluator as the runtime authorities. Preserve the standalone module and its tests for Oleksandr's authoring workflow.

Alternative: replace both lesson representations with a shared type now. This would remove duplication but change both teammates' modules, the standalone harness and the application together. Defer that wider migration until more authored lessons demonstrate the required shared shape.

The adapter must reject unsupported shapes explicitly. It must never silently drop accepted answers, hint levels or fresh-check flags. For this increment, support the actual variables fixture: output and token questions, one accepted answer, three hints, two practice questions and one final fresh check. Reject single-choice handoff records until the producer supplies an option catalogue. Existing graphical choice questions remain supported.

## Work sequence

### 1 Establish the integration contract

Write failing contract tests before changing the catalogue. Verify the current authored fixture adapts into one valid C lesson with its explanation, example, question prompts, accepted answers, feedback and all nine hints intact. Cover missing fields, duplicate IDs, invalid revision, unsupported question types, multiple accepted answers, wrong hint counts and invalid practice/fresh ordering.

Keep app-owned presentation metadata such as the learning objective and existing assignment walkthrough beside the adapter. Check that the walkthrough still describes the authored example. Do not invent author-owned teaching content to fill unsupported fields.

### 2 Connect the catalogue and protect progress

Add the authored source and adapter to the existing CMake core target. Replace only the variables entry returned by `sampleLessons()`; preserve catalogue order, its existing lesson ID `c-variables`, and stable application question IDs through an explicit mapping.

Because the first practice question changes from choice to output and the fresh-check wording changes, advance the graphical lesson's assessment revision from 1 to 2. Preserve the author's own revision separately in the mapping so future author changes require an intentional graphical revision decision. Do not equate the two revision counters automatically.

Use the existing controller's revision handling: old evidence must not count as completion or current review membership, and Continue must return to the revised explanation. Do not copy old independent-attempt evidence into the new assessment. Keep old saved records readable through existing storage logic; no snapshot-format migration is planned.

### 3 Verify grading and teaching content

Use `cw::evaluate()` for every answer submitted through the application. Keep `cwaypoint::evaluate()` confined to the standalone harness. For the supported adapted output/token records, add parity checks for correct, incorrect and empty inputs, CRLF, final newlines, significant spaces, case and outer token whitespace. If the evaluators disagree, resolve the documented contract at its owning layer before integration acceptance.

Compile the authored C example in a complete C17 harness with the required header and main function. Confirm its output is 5. Separately compile the fresh-check example and confirm its output is 4. Learner input remains data; it is not compiled or executed.

### 4 Exercise the learner journey

Extend the existing isolated executable scenario driver to open the integrated lesson, inspect its authored explanation, submit an incorrect answer, reveal an offline hint, retry correctly and complete both practice questions. Verify mistake review clears without creating independent success after help.

Exercise the fresh check separately, including saved issuance before display. Restart into the same isolated profile and verify progress and Continue. Also open a revision-1 fixture and verify stale completion, review and resume positions do not transfer to revision 2. Confirm C++ strings and C pointers still open and grade normally.

Manually check typing, selection, keyboard navigation and clipboard actions in the real macOS window. Record these separately from automated handler tests. A second-Mac check depends on a teammate device being available; do not claim cross-device support from this build alone.

### 5 Package and document the result

Build the existing app bundle and refresh the local ZIP using the current packaging process. Verify the bundled library path and signature, then launch the packaged copy outside the repository with an isolated profile.

Update `docs/content-contracts.md`, `docs/integration.md`, `docs/implementation-status.md` and the relevant README instructions to identify the authored source, adapter, revision mapping and remaining supported-shape limits. Retain historical planning documents and both existing standalone commands.

## Ownership

| Owner | Responsibility |
| --- | --- |
| Tymofii | Adapter, CMake integration, graphical catalogue mapping, revision handling tests, UI journey, macOS packaging and native input checks |
| Oleksandr | Review the adapted wording, answer semantics and hints; confirm stable authored IDs/revisions; maintain the standalone fixture and evaluator |
| Both | Agree the integration contract and inspect the complete learner journey before accepting the build |

Implementation can proceed against the checked-in fixture. Acceptance of the teaching wording requires Oleksandr's review; this plan does not claim that review has happened.

## Acceptance checks

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` and `cmake --build build -j4` succeed with existing dependencies.
- `ctest --test-dir build --output-on-failure` passes, including new adapter, grading-parity and revision-transition cases.
- `make test` passes for the preserved standalone contribution.
- The new integrated UI scenario passes in isolated standard and compact profiles; existing smoke, review and restart scenarios remain green.
- Authored C examples compile with `-std=c17 -Wall -Wextra -Werror` and produce the stated outputs.
- `codesign --verify --deep --strict build/CWaypoint.app` and `git diff --check` pass.
- Packaged launch and the manually observed native input checks are recorded with their actual outcomes.

Primary success is the authored lesson working through the executable with correct revision-aware progress. Passing unit tests alone does not establish that outcome.

## Boundaries and risks

No live AI, new production dependency, new persistence format, full curriculum expansion, redesign, CI/CD or release infrastructure is included. A provider contract, credentials, networking approval and cancellation semantics remain prerequisites for live AI. VoiceOver support remains unresolved.

The main risks are accidental assessment-history reuse and hidden loss when adapting incompatible records. Explicit revision tests and rejection of unsupported shapes address those risks. Keep changes in focused validated commits; pushing or opening another pull request is a separate delivery action.

Suggested implementation commits:

1. `feat: adapt authored variables lesson into the application catalogue`
2. `test: verify integrated lesson progress and restart journeys`
3. `docs: document authored lesson integration and build verification`
