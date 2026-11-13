# Working executable

Current delivery status: 10 November 2026. Four lessons, twelve questions, 36 authored hints, course workspace and version-three activity saves are integrated. Earlier dated increments below retain their historical counts; the final partner-first delivery entry describes the current integration.

## Delivered

`build/CWaypoint.app` is a runnable Apple Silicon macOS application. `build/CWaypoint-macOS-arm64.zip` contains the same app with its embedded raylib library and license notice. Application source is in `src/`; CMake builds the executable and headless core tests. No package downloads, new production dependencies or hosted automation were added.

The user-owned implementation includes Home/Continue, separate C and C++ catalogue filters, lesson Read/Practice/Fresh check/Summary, feedback and retries, progress and mistake review, walkthrough player, comparisons, text-size settings, focus outlines, keyboard navigation, vertical/code scrolling, save notices and explicit reset confirmation. Three starter lessons, nine questions and 27 local hint levels make the app usable before the full curriculum arrives.

The shared controller records issuance, attempts, assistance and historical first-attempt evidence. Practice completion counts two practice questions and stays separate from fresh evidence. Duplicate submit is rejected until explicit retry. Progress uses a current-generation manifest, versioned snapshots, same-generation backup, serialized synchronous writes, atomic replacements and a single-writer `flock`. Reset creates a new generation. Corrupt data is preserved before recovered progress replaces it; recovered history is labelled unverified. Newer primary formats remain read-only even when an older backup exists. Temporary practice is available when saved progress cannot be used; new fresh checks require writable saved issuance.

## Validation performed

- Release configure/build: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` and `cmake --build build -j4` — passed, AppleClang 21 / installed raylib 6.0.
- `ctest --test-dir build --output-on-failure` — passed. The learning/storage executable contains contract, normalization, duplicate submission, assistance/retry/restart, independent-history preservation, both C/C++ completion/resume, generation reset, second-writer lock, backup provenance/preservation, unsupported/newer formats and write-barrier failure checks. This is not all 22 planned test families.
- `CWaypoint --data-dir <isolated folder> --smoke-test` — passed at standard size and at `--compact` 960×640. The driver exercises actual UI button handlers and asserts resulting learning/settings state. It does not simulate physical mouse/keyboard events.
- Standard and compact logical renders inspected, plus a native macOS window screenshot. A separate interface reviewer found footer traversal and input focus-reveal issues; both were fixed and the reviewer scored those fixes resolved. Interactive OS input automation could not reliably target the window, so physical mouse/keyboard journeys remain a manual acceptance check.
- All three authored lesson code examples extracted from actual records, compiled as C17 or C++17 with `-Wall -Wextra -Werror`, executed, and their expected outputs verified.
- `codesign --verify --deep --strict build/CWaypoint.app` — passed. `otool -L` confirms the executable uses the bundled raylib path and system libraries.
- App launched from `/tmp` using an isolated profile and local capture output — passed. Relative project paths are not needed at runtime.
- `git diff --check` — passed for tracked changes.

Primary signal: met for the local executable and automated starter learning journeys. Overall planned application acceptance is partially validated, pending the remaining curriculum/provider work, manual input testing and another Mac.

## Next increment — editing and navigation

Added a tested `TextInput` model with bounded insertion, selection replacement, backspace/forward-delete, character/word movement and select-all. The raylib input field uses this model for mouse placement/drag selection, Shift+arrows, Home/End, Command+Left/Right, Option+Left/Right, and Command+A/C/X/V. Selection and caret are clipped to the field, with horizontal scrolling to keep the caret visible. These remain custom ASCII controls, not native Unicode/IME controls.

Drafts are keyed by stable question ID and remain separate from submitted learning evidence. Read/Practice and Home/Continue restore the same practice question and draft; reset clears session drafts and remembered catalogue/practice positions only after its durable reset succeeds. Each track retains its own catalogue scroll position.

Validation: `answer_editing` CTest covers middle insertion, selection replacement, deletion, selection collapse, bounded cursor/length, word navigation and single-line filtering. The UI smoke driver now edits a draft, switches through Read/Practice and Home/Continue, then asserts the retained text, practice question and restored catalogue offset. Compact and standard runs are recorded as automatic interaction-handler checks; native OS keyboard/clipboard events still need manual acceptance.

## Next increment — guided learning path

Home suggests the next unfinished lesson in the selected track. Completed practice summaries offer a direct next-lesson action. Suggestions are derived from revision-matched practice evidence, stay within the track, wrap to earlier incomplete lessons, and return no suggestion after all available starter practice is complete. Fresh-check evidence does not gate progression. Progress now lists each practice question's status separately from the fresh check.

Optional walkthrough and comparison validators check required fields and bounded text/step sizes at the consuming view. A bad walkthrough shows an unavailable message but keeps the explanation and practice usable. Missing walkthroughs remain optional. No save-format changes were required.

Validation: core checks cover same-track selection, skipped completed lessons, missing/finished tracks and invalid optional records. `--guided-test` completes variables practice and its independent fresh check, follows the next-lesson action to pointers, completes pointer practice with an intentionally malformed optional walkthrough, and verifies that C has no remaining starter suggestion while C++ still does. Compact captures include scrolled summary, unavailable walkthrough and completed progress states. Physical OS input testing and the full reviewed 22-family acceptance plan remain pending.

## Next increment — focused mistake review

The navigation now includes a dedicated Review mistakes screen, with all/C/C++ filters, a current-question count, descriptive lesson/question labels and Start review. A focused review question offers Next review question and Back to review list; Escape returns to the list. Correct answers clear the question, keep first-attempt/assistance history, and show a cleared confirmation. The final cleared item leads to an explanatory empty state rather than a dead end. Progress links to this queue.

`Controller::reviewItems()` owns the current-review projection. Unknown question IDs and obsolete assessment revisions are not displayed. Starting a new revision clears obsolete review membership at the same transition that creates its new evidence, preventing a prior mistake from reappearing as a new one. No save-format change was needed.

Validation: core checks cover unique membership, filters, retained history, restart, unknown IDs, obsolete revisions and new-revision issuance. `--review-test` uses an isolated profile with one wrong C practice answer and one wrong C++ fresh answer; it exercises filters, retries both through the UI handlers, verifies two attempts and supported-practice status, then checks the empty list. Compact list/cleared/empty captures were reviewed. Native physical keyboard and clipboard events remain unverified.

## Next increment — exact resume across restarts

Continue now states and restores the saved practice-question index or walkthrough step after reopening the executable. Question navigation, review entry, stage changes and walkthrough Previous/Next/Restart all write the same controller-owned resume fields. The saved assessment revision guards against restoring an obsolete position: revised lessons return to the current explanation; shortened or invalid walkthroughs reset their stored step safely.

Snapshot format two adds a RESUME record containing only question index, walkthrough index and assessment revision. It stores no draft answers. Readers accept format one, default its missing detail safely, and upgrade only on a successful save; the previous snapshot is retained as a backup. Unknown newer formats stay read-only, including when a compatible older backup exists. The generation manifest is unchanged. Older builds cannot edit a format-two primary; their existing unsupported-format path preserves it.

Validation: CTest covers question/step restart, invalid positions, content-revision changes, shorter walkthroughs, format-one upgrade and retained legacy backup. Four independent executable processes (`--resume-test seed`, `question`, `walk-seed`, `walk`) verify exact question and walkthrough restoration through the actual Continue handler; the reopened question also checks that an unsubmitted draft was not persisted. An archived earlier executable opened a copied format-two profile, and byte comparison confirmed its manifest, primary and backup were unchanged. Native physical input and another Mac remain unverified.

## Next increment — readable code panels

Each example, walkthrough and comparison panel now has its own clamped horizontal offset. Long lines keep the selected text size and expose scroll buttons plus an overflow-position indicator. Code bodies can receive keyboard focus; Left/Right and Home/End move within the line, and either Shift key + wheel pans under the pointer. Copy code and Command+C copy the full source, not only its visible portion; the temporary confirmation is tied to the exact source text and panel.

Shared content hit-testing now rejects pointer hits covered by the fixed header/footer, so clipped controls cannot activate through those surfaces. Answer cursor placement uses the same viewport clipping for its initial click. In-progress drag selection remains free to extend beyond the field.

Validation: `--code-test` at 960×640 and text size24 uses two long synthetic panels, exercises their actual scoped scroll buttons through route changes, and asserts distinct offsets. Synthetic pointer coordinates check header/footer hit exclusion. Compact captures show both overflow controls and the independent walkthrough panel; source review checked copy confirmation and both Shift keys. CTest and existing UI routes remain regression checks. Physical keyboard shortcuts and OS clipboard contents remain manual acceptance items; the automated driver deliberately does not overwrite the user's clipboard.

## Next increment — undo/redo for answer drafts

Text answers now support Command+Z / Shift+Command+Z and visible Undo / Redo buttons. A per-question history retains up to 32 text changes, restoring caret and selection as well as text. New edits discard the redo branch; filtered/no-op input leaves it intact. Returning from an editor toolbar action focuses the answer field, avoiding focus jumps when Undo/Redo enabled states change.

History stays in RAM with the session draft and is cleared when the draft baseline is replaced or the profile is reset. No draft/history fields enter the save format. Undo never calls the evaluator, controller retry or progress reset. If editing changes the text after a graded result, feedback explains that the previous result is unchanged and a new submission requires Try again.

Validation: the `answer_editing` test covers selection replacement, deletion undo, redo, branch invalidation, 32-change capacity, no-op behavior and baseline reset. `--undo-test` reverses an edit, visits Read/Practice, redoes and submits, undoes again, verifies duplicate checking cannot add an attempt, then explicitly retries. It asserts two total attempts and preserved independent-first-attempt evidence. Compact captures show enabled and disabled editor controls; physical Command-key events remain a manual acceptance item.

## Handoff and remaining work

Oleksandr's full 14-lesson curriculum, independent human teaching review, full evaluator fixtures, provider/credential/network dependency decisions, live AI worker and quality/failure tests remain outstanding. The executable states that AI is not configured and never sends requests. `docs/content-contracts.md` describes the compiling handoff; `src/core.hpp` is the authoritative contract.

Native VoiceOver exposure is not implemented. Answer fields support single-line printable ASCII cursor editing, selection, copy/cut/paste and word navigation. Per-question drafts and current practice position survive navigation within the running session, and catalogue scroll is restored per track. Drafts and catalogue scroll offsets are not persisted across application restarts. Resume restores the saved lesson, stage, exact question and walkthrough step; answer drafts remain session-only. Walkthroughs are bounded authored snapshots; arbitrary learner code is never executed.

Only this development Mac (macOS 27.0, arm64) was exercised. Minimum supported OS, Intel/universal build, other teammate devices, filesystem power-loss behavior, performance budgets and the full 22-family test plan remain unverified. The app is ad hoc signed for local use, not notarized for public download.

The Word design/work plans and reviewed Markdown plan remain planning snapshots; this file and the source record actual implementation status. Earlier task-list documents still contain historical, superseded language/platform decisions.

Suggested commit message: `feat: add runnable macOS learning app with offline practice and progress`

Pre-landing review: failed-save submissions now remain guarded until their result is durably saved. Repeated Check answer or Try again actions cannot inflate the pending attempt. Retry save persists that result once; a new attempt still requires an explicit retry. Regression coverage verifies both failure and recovery. At that pre-landing stage, Oleksandr's first lesson/evaluator handoff was tested independently under src/learning. The integration is delivered in the next increment below.

## Authored lesson integration — 10 November 2026

The graphical catalogue now consumes Oleksandr's checked-in C variables lesson through a bounded adapter. Authored teaching text, output/token questions, explanations and all nine hints are retained; the existing app-owned objective and matching walkthrough remain. The author revision-1 record maps explicitly to application assessment revision 2 with established application IDs. Old assessment evidence remains stored but is excluded from current completion/review, and revised Continue positions restart at the explanation. No storage-format change, production dependency or standalone author-module change was required.

The adapter rejects unsupported shapes, including multiple accepted answers, single-choice handoff records without options, incorrect practice/fresh ordering, missing teaching fields, changed author revisions and answers exceeding the input limit. Runtime grading remains in the graphical evaluator and controller; standalone evaluator parity is checked for the supported bounded output/token inputs.

Validation: release CMake build and all three CTest suites passed; the new adapter test includes a real persisted revision transition and restart. `make test` passed. The integrated UI scenario passed at standard and compact sizes; smoke, guided progression, mistake review, code scrolling, undo/redo and four independent resume phases passed. Authored example source was extracted, compiled in a C17 harness with `-Wall -Wextra -Werror`, and printed 5; the fresh-check statements compiled separately and printed 4. Signature verification, bundled-library inspection and `git diff --check` passed. Compact renders were inspected. The local ZIP was refreshed and its extracted executable was exercised outside the repository.

Primary signal: met for the automated authored lesson journey and local packaged executable. Native keyboard/clipboard testing remains unverified: the available native UI target showed an older running build, which was left running. Another Mac and Oleksandr's review of the adapted teaching content remain acceptance follow-ups. Live AI and the remaining full curriculum are still pending.

## Dark desktop redesign — 10 November 2026

Implemented the requested charcoal/periwinkle theme, top navigation, monogram Home link, vector gear menu and fixed rounded internal panels. Wide practice places code beside the answer; compact practice stacks matching code and input. Hints expand on demand, editing controls follow Check answer, and next/retry actions receive emphasis according to learning state. Home now leads with the logo and learning actions. The menu provides Preferences and a dedicated keyboard-help view.

Question-specific code is a bounded optional field across producer, adapter, validator and renderer. Existing fresh checks display their own examples; accepted answers, assessment IDs/revisions and grading semantics are unchanged. A controller-owned unsavedChanges flag drives save-recovery UI; failure details are readable in Settings, with retry available for writable profiles. Reset success clears the flag. Save barriers and snapshot format remain unchanged.

Verification: Release CMake configure/build and all four CTest suites passed (learning/storage, editor, authored integration and responsive workspace). make test passed. Isolated executable runs passed for authored practice at standard/compact/24px compact sizes, smoke including actual menu entries and keyboard-help route, guided progression, mistake review, code scrolling, undo/redo and four separate restart phases. Final packaged ZIP was extracted and its authored journey passed from /tmp. App/extracted-app signature verification and git diff --check passed. Native renders were inspected for Home, standard/compact practice, fresh-check code, menu and large text. Opaque text contrast pairs were calculated and recorded in DESIGN.md.

Primary signal: met for the requested visual layout and automated executable learning journeys. Physical OS keyboard/clipboard and screen-reader behavior remain unverified or unsupported; no learner usability study or second-Mac acceptance is claimed. Largest text and longer examples still need scrolling. The wider roadmap's compact progress rows, side-by-side Compare view and semantic focus restoration remain follow-ups. DESIGN.md and the generated native sidecar describe the delivered implementation, not those future features. The sidecar is regenerated with scripts/generate_design_metadata.py.

## Course catalogue and three-panel study workspace — 10 November 2026

Replaced the previous stacked Home/lesson composition with a common C/C++ course catalogue and a course shell: left module list, central material/practice and right Study assistant. Existing lessons are modules grouped by track; counts and completion come from real records. Cards and their Open actions enter the first unfinished module. Top navigation and secondary actions are quieter text controls, stage selection uses an underline, and Try again appears after a result. No placeholder courses or extra lessons were added.

The assistant supports typed messages and quick prompts, with a bounded independently scrolling transcript and fixed input/Send action. It returns existing authored notes or hints, explicitly labelled as offline guidance; it cannot answer arbitrary questions like a live model. Exercise responses go through the same durable help barrier and do not create answer attempts. Blocked fresh checks reveal no assistant content. Conversations/drafts are session-only and reset clears them to prevent assistance leaking into a new profile.

Validation: Release build, all four CTest suites and make test passed. Geometry tests cover centre minimum width and non-overlapping side panes at 960, 1280 and 1600. New standard/compact course scenarios open both tracks, switch modules, grade C/C++ answers, send an assistant message, verify help without an answer attempt, then reset and verify conversations/evidence are cleared. Smoke, guided progression, review, code scrolling, undo/redo and four separate restart phases passed. Standard/compact catalogue, material and assistant renders were inspected. Signature verification and git diff --check passed. The refreshed ZIP was extracted and its course scenario was run outside the repository.

Primary signal is met for automated course navigation, authored assistance and profile-reset isolation. Native OS keyboard/clipboard testing and learner usability testing remain pending. The assistant uses authored responses, not live AI; provider integration remains a separate prerequisite. Module navigation fits the current three-lesson catalogue; scrolling that list is needed before the full 14-lesson curriculum is delivered. No snapshot-format, dependency or grading change was required.

## Partner-first merge and delivery — 10 November 2026

Oleksandr's PR #3 was reviewed, built and tested separately, then merged first into main as 8e8bb34. Integration into this branch exposed real conflicts in main.cpp and sample_content.cpp. Commit be30d1a resolves them by retaining the course catalogue/three-panel interface and authored variables adapter, adding C conditions as the third C module, and preserving daily activity/streaks and version-three saves. Streak badge geometry was adapted to avoid the gear/navigation; module navigation now scrolls and reveals the selected module. Conditions questions have matching score-49 and height examples.

Four CTest suites and make test passed after integration. Added explicit version-two-to-three upgrade/backup checks and malformed ACTIVITY bounds, duplicate and truncation fixtures, verifying original files remain unchanged. Conditions code compiled as C17 with warnings treated as errors and printed Retry at 49, Pass at 50 and 51. The integrated conditions and course journeys passed. The partner-only build also passed both tests, make test and uncaptured smoke/guided journeys, resolving the previously reported uncaptured-smoke concern in this environment. Physical input/other-Mac testing and human acceptance of the inactive flame styling remain pending.

The three-hour development journal for Tymofii is under docs/journals, with Markdown source and a visually verified Word copy matching the earlier journal format. Hours are self-reported, not measured. Older untracked 6 November journal files were left outside the delivery commits.

Final delivery verification: all twelve isolated combined executable runs passed (course standard/compact, conditions compact, smoke, guided, review, code, undo and four resume phases). Both app and extracted-app signatures verified. The feature branch contains the partner merge and the tracked three-hour Word/Markdown journal; latest-base ancestry and normal-push ancestry were checked before publication.

## Wide-window button hit testing — 10 November 2026

User-reported unresponsive buttons were reproduced at a 2036-logical-pixel window width. The material panel is laid out across the course workspace, but UI::hit additionally intersected it with the centred, capped catalogue viewport. This excluded visible left-aligned course controls such as Start practice and Check answer on wide windows. The shared hit test now selects the active screen's owning viewport, retaining header/footer, menu and side-panel exclusions.

The new --pointer-test injects a coordinate at each rendered button centre through the normal visibility, clipping and click path, rather than using label-based activation. Before the fix it failed at Start practice; after the fix it completed both-track practice, next-question navigation and the settings menu. Geometry tests cover 960, 1280, 1600, 2036 and 2560 widths and reject side-panel/header/footer hits. Release build, all four CTest suites, the coordinate regression and the twelve existing isolated executable journeys passed. The refreshed packaged copy passed the coordinate journey from /tmp; both signatures and git diff --check passed.

Primary signal is met for the reproduced coordinate/clipping failure. Physical OS clicking remains partially validated: the native window screenshot was available, but the automation click call returned noWindowsAvailable. Running instances must be restarted to load the rebuilt executable. No learning, persistence or theme semantics changed.

## 13 November 2026 — progress reporting and offline validation

- Progress now exports non-overwriting CSV reports with current-revision
  question evidence, recovered-history and unsaved-state labels. Typed answers
  are excluded. Export errors remain separate from profile-save notices.
- `--check-content` checks required lessons and optional walkthroughs/comparisons
  before creating a profile or initializing raylib. Exit codes are documented in
  README. No network service or new production dependency was added.
- Six CTest suites pass, including report escaping, revision filtering,
  recovered history, unsaved state, file preservation and export failures.
- The automated smoke journey activated the export control, generated twelve
  rows and captured the success message. Headless checking reported four
  lessons, twelve questions, thirty-six hints and three comparisons with no
  warnings, and created no profile. macOS bundle signature verified.
- Reports are snapshots, not backups. Intel/second-Mac testing and recording-time
  keyboard verification remain outside this milestone.
