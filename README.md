# CWaypoint

A native Apple Silicon macOS application for learning C and C++, written in C++17 with raylib.

## Run

Open `cmake-build-milestone/CWaypoint.app` in Finder. The bundle includes raylib, WASI SDK 34 and Wasmtime 50.0.1; using a built app requires neither Homebrew nor an internet connection. This is an ad hoc signed local build, not a notarized public release.

## Learning workspace

- **My learning / All courses:** resume the saved lesson, search the C/C++ catalogue, open course modules and revisit bookmarked lessons.
- **Learn:** read material, solve two practice questions, attempt an independently recorded fresh check, view authored hints and inspect assessment status.
- **Code:** edit a complete C17/C++17 program, run the bundled compiler, inspect output and two authored tests. Complete a lesson after solving both practice questions and passing the current lab.
- **Notes:** private multiline lesson notes, saved locally. Discussion explicitly reports that shared discussions are unavailable offline.
- **Settings:** display name, daily goal, editor light/dark theme, numeric font sizes, daily nudge and reading size. Preferences use Save/Cancel, with a guard when leaving an edited form.
- **Progress / Review / Compare:** available under Settings → Learning data, reading size & help. CSV export is absent from the interface.

The wide lesson screen has a module rail, material and editor side by side. Below 1280 px it uses a module drawer and Material/Code tabs. Minimum window size is 960 × 640. Scroll the material or page to reach additional controls. Shift-scroll moves horizontally in the editor. Escape releases editor focus; Tab then moves between controls. Standard Command-A/C/X/V/Z editor shortcuts are supported. The source editor highlights C/C++ syntax. Enter preserves block indentation; closing braces align with their opening block. Tab inserts spaces to the next four-column stop, Shift-Tab outdents, and selections can be indented as a block. Text input currently supports ASCII; native IME and VoiceOver integration remain future work.

The catalogue contains three C modules (variables, pointers, conditions) and one C++ strings module. Curriculum totals reflect those available modules. More languages, online accounts and live AI are outside this build.

## Execution boundary

Learner source compiles to WebAssembly, not a native macOS executable. An App Sandbox helper invokes the bundled compiler and Wasmtime. Guest programs receive no host directory, environment or network grants. Runs have fuel/time limits, a 64 MiB linear-memory ceiling, bounded output and cancellation. Compilation is separately timed and file-size limited. Filesystem, socket, process and macOS GUI exercises are unsupported.

Run code during practice/fresh checks records assistance before executing. Compilation alone cannot complete a lesson. Results become stale after source changes; completion stores the source SHA-256, assessment/test revisions and a distinct lesson/day receipt. A failed save is shown instead of claiming durable completion. Tests verify concrete denial cases; they do not constitute a formal security proof.

## Build

Prerequisites: Apple command line tools, CMake, Python 3 (build only), pkg-config and raylib. The approved pinned lab-tool download is approximately 190 MB compressed; the unpacked app is substantially larger.

```sh
python3 scripts/setup_lab_tools.py
cmake -S . -B cmake-build-milestone -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-milestone -j4
ctest --test-dir cmake-build-milestone --output-on-failure
open cmake-build-milestone/CWaypoint.app
```

The setup script checks official release SHA-256 digests. CMake packages tools and signs the helper with App Sandbox entitlements. Do not replace this with deep signing that erases helper entitlements. Dependencies and notices are included in `Contents/Resources`.

## Isolated validation

Use a fresh `.scratch/` directory for each journey. These commands leave the normal learner profile untouched:

```sh
APP=cmake-build-milestone/CWaypoint.app/Contents/MacOS/CWaypoint
"$APP" --check-content
"$APP" --check-runner
"$APP" --data-dir .scratch/reference-demo --reference-test
"$APP" --data-dir .scratch/reference-small --reference-test --compact
"$APP" --data-dir .scratch/screenshot --capture-dir .scratch/screens --screen lesson
make test
make quality PYTHON=.scratch/complexity-tools/bin/python
```

`--reference-test` drives the production reference screens and validates saved practice/lab completion, notes, bookmark, preferences and search. It requires an isolated data directory. Capture mode exports a screenshot and exits; combined with a reference journey it captures the final state. `--screen` also accepts home, settings, progress, review and compare.

Legacy `--smoke-test`, guided, authored, review, code, undo, course, conditions, pointer, settings and resume journeys retain the previous diagnostic renderer to check its engine adapters. They do **not** validate the new production layout. Core and integration tests are shared by both renderers.

Every application function under `src/` must have Lizard cyclomatic complexity ≤15, including Objective-C++. See `docs/code-quality.md` for pinned development tooling. Lizard and scc are not application runtime dependencies.

## Local data

Normal data lives under `~/Library/Application Support/CWaypoint`. The existing writer lock protects both progress and study data. A second app instance is temporary/read-only. Profile v4 reads versions 1–3 and retains an immutable `.pre-v4` snapshot on upgrade. A separate `study-v1.save` stores preferences, notes, bookmarks and source drafts. Completed source snapshots are immutable and scoped to the reset generation. Reset clears learning evidence and personal lesson state while retaining preferences. No data syncs across devices.

See `docs/reference-runtime.md` for implementation contracts and validation limits, and `docs/reference-design-build-plan.md` for the approved design decisions.
