# Reference-screen implementation — 14 November 2026

The approved reference design is the default native renderer in `src/reference_app.cpp`. `main.cpp` retains the previous renderer only for explicitly requested legacy diagnostic journeys. The new renderer shares the same Controller/Store assessment authority; it does not independently grade authored answers.

## Ownership and persistence

- `core`: profile v4, legacy evidence migration, assistance, revision-aware lab receipts, distinct lesson/day completion and reset generation. Answer correctness and course completion are separate concepts.
- `study_store`: versioned personal preferences, notes, bookmarks and drafts; owner writer lock; same-directory atomic replacement and fsync. Unsupported/corrupt records fail read-only. Generation changes clear lesson state and retain preferences.
- `code_editor`: bounded ASCII multiline editing, selection, undo/redo and indentation. UI owns focus, scroll offsets and clipboard integration.
- `lab`: four authored complete-program exercises, SHA-256 source identity, bounded helper protocol and output matching.
- `process`: direct argument-vector spawn, explicit environment, bounded pipe I/O, timeouts, owned process-group cancellation and cleanup.
- `lab_helper.mm`: App Sandbox workspace, compiler invocation and Wasmtime execution. No shell interpretation of learner text.

A lab receipt requires a passing compiler run, current source/test identity, writable storage and the current reset generation. The UI writes an immutable source snapshot and automatically asks the Controller to persist the result. A lab can be saved before the two knowledge checks are solved. Each solved knowledge check and the passed lab contributes one of three progress units. Completing all three records lesson completion and the distinct lesson/day activity automatically, regardless of which task is completed last. Changed knowledge-check answers can be submitted directly; repeated unchanged submissions are idempotent. Failed saves retain the pending-submission guard. Legacy activity days are preserved. In the new lab mode, individual correct answers do not add new streak days.

## Packaging

The explicitly approved WASI SDK 34 and Wasmtime 50.0.1 archives are fetched by `setup_lab_tools.py`, hash checked and unpacked under ignored `.scratch/approved-tools`. `package_lab_tools.py` embeds the offline tools, notices and child signatures. CMake signs the App Sandbox helper separately, then the app; it does not deep-sign over helper entitlements. The native-executable spike remains rejected: the app executes WebAssembly only.

Runtime limits: source 64 KiB, at most 16 test inputs of 4 KiB, helper request 270000 bytes, compiler 15 seconds, each run 3 seconds supervisor / 2 seconds Wasmtime timeout / 10 million fuel, 64 MiB guest linear memory, 64 KiB combined output per process. Supervisor timeout is 55 seconds. No inherited host environment, network or preopened directories are supplied. Compilation has an OS sandbox, CPU and file-size limits. Compiler and runtime errors remain visible in the output panel.

## UX and limits

The supplied screenshots define the dark neutral palette, blue navigation, warm streak panels, rounded cards and three-column workspace. C/C++ material replaces the reference's Python content. Compact widths switch to Material/Code tabs and a module drawer. Preferences have explicit Save/Cancel and leave confirmation; drafts autosave with visible failure notices. Run cancellation does not stop unrelated applications.

Discussion is explicitly offline. The catalogue reflects four available modules, not the example's hypothetical 20+ lesson counts. Notes/bookmarks are private local data. No hosted service, telemetry, account, CSV UI or live AI dependency was added. Accessibility is limited to keyboard focus/shortcuts and readable contrast; native VoiceOver and IME are not implemented. This Apple Silicon bundle has not been notarized or tested on Intel Macs.

## Evidence

CTest covers legacy assessment/storage, authored content, options/layout, multiline editing, bounded processes, all four actual C/C++ WASI solutions/starters, invalid programs, loop/output/memory bounds, guest filesystem/environment denial, compiler unrelated-file denial, cancellation, completion receipts, stale revisions, migration and personal data.

The production `--reference-test` journey covers catalogue → practice → real compile/run → complete → notes/bookmark → settings/dropdowns → search → My learning. Both wide and compact journeys assert persisted data by reloading storage. Screenshot review covers catalogue, settings, wide lesson and compact material screens. Pointer-location and keyboard/OS recording behavior still benefit from manual interaction on the target Mac; synthetic journeys are not a native accessibility or macOS screen-recorder certification.


Practice places the compiler exercise instructions, input/output examples and Run & check action next to the editable workbench. The two short knowledge checks are explicitly labelled separately from the coding exercise. The catalogue, module rail and progress screen consume the same partial-progress calculation; a separate Mark complete click is no longer required.
