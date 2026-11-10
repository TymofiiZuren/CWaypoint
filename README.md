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
- Three starter lessons: C variables, C pointers and C++ strings. Nine questions with three offline hint levels each.
- Two authored walkthroughs with Previous / Next / Restart, plus three C/C++ comparisons.
- Single-choice, output and token answers with feedback, retries and answer reveal.
- Historical first-attempt independence remains separate from assisted/repeated practice.
- Local progress, exact practice-question/walkthrough resume after restart, text-size preference, backup recovery, explicit generation-based reset and a single-writer profile lock.
- Mouse and keyboard controls, scrolling, adjustable lesson text and code, resizable window and system fonts.
- Independent code-panel scrolling, focusable code with Left/Right and Home/End, scroll buttons, and full-source Copy code / Command+C.

This is the application/UI owner's working increment. The full 14-lesson curriculum, reviewed content inventory, live AI provider/transport and native VoiceOver exposure are not implemented. The app makes no network requests and needs no credentials. Remaining integration work is recorded in [docs/implementation-status.md](docs/implementation-status.md).

## Controls and storage

Tab / Shift+Tab moves focus, Enter / Space activates a button, Escape returns, and Page Up / Down scrolls. In answer fields, click to position the cursor, drag or hold Shift with arrows to select, and use Command+A/C/X/V to select all, copy, cut or paste. Arrow keys, Home/End, Command+Left/Right and Option+Left/Right move the cursor; Backspace/Delete remove text. Use Command+Z / Shift+Command+Z or Undo / Redo to reverse or restore up to 32 text changes in that question. Press Enter to check. Undo changes the draft only; submitted results and attempt counts remain unchanged until an explicit retry. Use the mouse wheel to scroll; either Shift key + wheel scrolls overflowing code horizontally. Code panels also provide scroll buttons; focus a panel to use Left/Right, Home/End or Command+C to copy its full source. Drafts and the current practice question survive navigation within the running session. The catalogue restores its scroll position per track. Drafts are not saved across application restarts. Continue restores the saved question or walkthrough step, and states that position on Home. Version-one saves are read automatically; the next successful save writes version two and preserves the prior snapshot as a backup. Earlier builds preserve version-two files read-only.

Default progress location: `~/Library/Application Support/CWaypoint`. Use `--data-dir PATH` for disposable demos and tests. Save failures show a persistent notice and Retry save. Unsupported or unreadable profiles are preserved; temporary practice remains available, but new fresh checks require writable saved issuance. Reset creates a new generation and does not securely erase previous generations.

## Integration

`src/core.hpp` defines the lesson, question, walkthrough, evaluation and profile contracts. `src/sample_content.cpp` assembles the starter catalogue. The variables entry comes from Oleksandr’s authored `src/learning/lesson.cpp` through the validated `src/lesson_adapter.cpp`; the other starter records remain local. Oleksandr can extend the catalogue and evaluator without changing screen-specific correctness logic. See [docs/content-contracts.md](docs/content-contracts.md).

## Oleksandr's first learning core

The first authored C variables lesson and evaluator are also available as a standalone C++17 handoff in `src/learning/`. Run `make test` to verify that evaluator and compile-check its C example; run `make demo` for a terminal demonstration. See [integration notes](docs/integration.md) and [Friday record](docs/friday-2026-11-06.md). The raylib app now consumes that authored variables lesson through a bounded adapter. Its application assessment revision is 2, so earlier question evidence does not count as completion of the revised assessment. Run the executable with `--data-dir <isolated-folder> --authored-test` to verify the integrated handler journey; add `--compact` for the minimum window size.
