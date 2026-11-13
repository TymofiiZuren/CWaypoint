# CWaypoint

A C++17 / raylib desktop app for learning C and C++, built for macOS.

## Run the application

Open `build/CWaypoint.app` in Finder, or run:

```sh
open build/CWaypoint.app
```

The current build is for Apple Silicon. raylib is embedded in the app bundle, so the built application does not need Homebrew to run. The app uses macOS system fonts and bundled-in-code starter content, and can launch from any working directory. This is a local, ad hoc signed build; it is not notarized for public distribution.

## Build and test

Prerequisites: Apple command line tools, CMake, pkg-config and raylib. The implementation uses the raylib already installed on the development machine; no dependencies are downloaded by the build.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
ctest --test-dir build --output-on-failure
open build/CWaypoint.app
```

For isolated testing (leaves your normal learner profile untouched):

```sh
build/CWaypoint.app/Contents/MacOS/CWaypoint --data-dir .scratch/demo-profile
build/CWaypoint.app/Contents/MacOS/CWaypoint --data-dir .scratch/ui-test-profile --smoke-test
build/CWaypoint.app/Contents/MacOS/CWaypoint --data-dir .scratch/guided-test-profile --guided-test
build/CWaypoint.app/Contents/MacOS/CWaypoint --data-dir .scratch/review-test-profile --review-test
build/CWaypoint.app/Contents/MacOS/CWaypoint --data-dir .scratch/code-test-profile --code-test --compact
build/CWaypoint.app/Contents/MacOS/CWaypoint --data-dir .scratch/undo-test-profile --undo-test --compact
```

Use a fresh profile directory for each smoke-test run. `--help` lists diagnostic options. Capture mode renders into a logical-size texture to avoid multiplying Retina scaling during export. Native window screenshots are also used for interface checks.

## Working features

- Home, C/C++ lesson catalogue, reading, practice, fresh check, summary, progress, mistake review, comparisons and settings.
- Undo/redo for text-answer drafts, including selection/cursor restoration and per-question session history.
- Dedicated mistake-review queue with C/C++ filters, friendly question labels, sequential retries and cleared-item feedback.
- Same-track next-lesson suggestions on Home and completed summaries; per-question practice status in Progress.
- Invalid optional walkthrough/comparison records show an unavailable message while valid practice remains usable.
- Four starter lessons: C variables, C pointers, C conditions and C++ strings. Twelve questions with three offline hint levels each (36 hints).
- Two authored walkthroughs with Previous / Next / Restart, plus three C/C++ comparisons.
- Single-choice, output and token answers with feedback, retries and answer reveal.
- Historical first-attempt independence remains separate from assisted/repeated practice.
- Local progress, exact practice-question/walkthrough resume after restart, text-size preference, backup recovery, explicit generation-based reset and a single-writer profile lock.
- Mouse and keyboard controls, scrolling, adjustable lesson text and code, resizable window and system fonts.
- Independent code-panel scrolling, focusable code with Left/Right and Home/End, scroll buttons, and full-source Copy code / Command+C.

This is the application/UI owner's working increment. The full 14-lesson curriculum, reviewed content inventory, live AI provider/transport and native VoiceOver exposure are not implemented. The app makes no network requests and needs no credentials. Remaining integration work is recorded in [docs/implementation-status.md](docs/implementation-status.md).

## Controls and storage

Tab / Shift+Tab moves focus, Enter / Space activates a button, Escape returns, and Page Up / Down scrolls. In answer fields, click to position the cursor, drag or hold Shift with arrows to select, and use Command+A/C/X/V to select all, copy, cut or paste. Arrow keys, Home/End, Command+Left/Right and Option+Left/Right move the cursor; Backspace/Delete remove text. Use Command+Z / Shift+Command+Z or Undo / Redo to reverse or restore up to 32 text changes in that question. Press Enter to check. Undo changes the draft only; submitted results and attempt counts remain unchanged until an explicit retry. Use the mouse wheel to scroll; either Shift key + wheel scrolls overflowing code horizontally. Code panels also provide scroll buttons; focus a panel to use Left/Right, Home/End or Command+C to copy its full source. Drafts and the current practice question survive navigation within the running session. The catalogue restores its scroll position per track. Drafts are not saved across application restarts. Continue restores the saved question or walkthrough step, and states that position on Home. Version-one and version-two saves are read automatically; the next successful save writes version three with daily activity and preserves a compatible prior snapshot as a backup. Earlier builds preserve unsupported version-three files read-only.

Default progress location: `~/Library/Application Support/CWaypoint`. Use `--data-dir PATH` for disposable demos and tests. Save failures show a persistent notice and Save details; Settings exposes the full message and Retry save for writable profiles. Unsupported or unreadable profiles are preserved; temporary practice remains available, but new fresh checks require writable saved issuance. Reset creates a new generation and does not securely erase previous generations.

## Integration

`src/core.hpp` defines the lesson, question, walkthrough, evaluation and profile contracts. `src/sample_content.cpp` assembles the starter catalogue. The variables entry comes from Oleksandr’s authored `src/learning/lesson.cpp` through the validated `src/lesson_adapter.cpp`; the other starter records remain local. Oleksandr can extend the catalogue and evaluator without changing screen-specific correctness logic. See [docs/content-contracts.md](docs/content-contracts.md).

## Oleksandr's first learning core

The first authored C variables lesson and evaluator are also available as a standalone C++17 handoff in `src/learning/`. Run `make test` to verify that evaluator and compile-check its C example; run `make demo` for a terminal demonstration. See [integration notes](docs/integration.md) and [Friday record](docs/friday-2026-11-06.md). The raylib app now consumes that authored variables lesson through a bounded adapter. Its application assessment revision is 2, so earlier question evidence does not count as completion of the revised assessment. Run the executable with `--data-dir <isolated-folder> --authored-test` to verify the integrated handler journey; add `--compact` for the minimum window size.

### Dark study workspace

The current macOS build uses a dark theme, top navigation, logo Home link and gear menu. Courses use left module navigation, central material/practice and a right offline study assistant at both supported sizes. Module and assistant content have bounded scrolling; hints are requested through the assistant. Save details opens the full recovery message in Settings. See [DESIGN.md](DESIGN.md) for the implemented layout and native-input limitations. `--large-text` is an isolated screenshot/scenario helper for checking the 24px setting.

### Course catalogue and study workspace

Home and Courses now show C foundations and C++ foundations together. Open either course to find modules on the left, course material in the centre and an offline study assistant on the right. The assistant accepts messages and quick prompts, returning authored notes/hints rather than live generated answers; exercise help is recorded before display. Conversations are session-only and cleared on progress reset. Run `--data-dir <isolated-folder> --course-test` (optionally `--compact`) to exercise both courses, module switching, assistant help and reset isolation.

Daily learning streaks record one local-calendar day after correct answers and persist in version-three profiles. The attributed flame badge retains the partner contribution; visual acceptance on the teammate Mac remains pending. Use `--conditions-test` with an isolated profile for the conditions/streak journey.

### Progress reports and offline content checks

Open **Progress → Export progress CSV** to create a numbered report in the
active profile's `exports` folder. Each current question has a row containing
its track, lesson, assessment revision, status, attempts and assistance flags.
Reports exclude typed answers, ignore obsolete assessment evidence, mark
recovered history as unverified and label unsaved session changes. Existing
reports are never overwritten. CSV text is quoted and spreadsheet formula
prefixes are escaped. A report is for review; it cannot restore a profile.

Validate the compiled lesson catalogue without opening a window or creating a
profile:

```sh
./build/CWaypoint.app/Contents/MacOS/CWaypoint --check-content
```

Exit codes: `0` valid content, `1` required-content failure, `2` unavailable
optional walkthrough/comparison records. The command uses the same validators
as the application and prints the catalogue counts. This checks authored
structure, not pedagogical correctness.

If Homebrew has replaced raylib since an earlier build, configure a fresh build
directory (`cmake -S . -B cmake-build-fresh -DCMAKE_BUILD_TYPE=Release`) to avoid
cached paths to the removed library.

In **Settings → Appearance**, open the font-size selector and choose 16, 18, 20,
22 or 24 px. The preview updates immediately and the selected size is saved.
Use arrows or Tab to move through an open list, Enter/Space to select, and Escape
to cancel. Settings also links directly to progress and CSV export. The isolated
`--settings-test --data-dir PATH` journey checks all five selections through
pointer hit geometry and verifies each saved size.
