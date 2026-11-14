<!-- /autoplan restore point: /Users/thomas/.gstack/projects/TymofiiZuren-CWaypoint/feat-macos-learning-app-autoplan-restore-20261113-215757.md -->
# Reference-design application version

Status: APPROVED on 14 November 2026. Implementation begins with the isolated-helper feasibility milestone.
User supplied three visual references on 13 November 2026: lesson/workbench, all courses, and settings.
Base branch: main (GitHub). Working branch: feat/macos-learning-app. Baseline: fc1f6b0.
UI scope: yes. Developer experience scope: yes (editable code, compiler/run output, checks and build commands).

## Product outcome

Implement a coherent version of the existing executable that follows the supplied reference composition, palette, typography hierarchy, navigation and interaction patterns. Adapt teaching content to the project's confirmed C/C++ focus unless the user changes that premise. All visible interactive affordances need an implemented, truthful outcome or an explicitly designed unavailable state; no fake run output, fabricated test passes or invented curriculum counts.

## Current implementation contract (review amendments take precedence)

- Native macOS C++17/raylib; C/C++ only; the three supplied images define the replacement visual world.
- Real local compilation/execution is required. First implementation milestone is a signed isolated helper spike; it is not yet proven and no unrestricted fallback is allowed.
- Recreate My learning, All courses/search, settings/profile/preferences, daily fire, lesson rail/material, multiline editor, Output/Tests, notes and bookmarks. Discussion is an explicit offline-unavailable tab with a Notes shortcut; shared hosting is not included without a separate service decision.
- One Controller projection owns current-revision completion for every consumer. Existing independent-attempt evidence stays distinct from lab success and historical achievement.
- Evidence schema migration adds generation-bound receipts/ledger with an immutable old-profile backup. Personal preferences/notes/drafts live in a separate bounded store under the same writer lock; immutable successful-source snapshots are distinct from mutable drafts.
- New goal/streak credit comes only from a successfully saved lesson completion and deduplicates by profile generation/day/lesson. Imported activity days are retained as historical days, not invented completion counts.
- Compact width uses a module drawer and Material/Code tabs; wide width matches the three-column reference. Editor theme/font are independent of reading size. Input retains an explicit ASCII limitation; native VoiceOver/IME is not claimed.
- Every application/helper function stays at Lizard CCN ≤15; existing and new real-run integration tests, migration tests and native captures are acceptance gates.

## Screenshot-to-feature inventory

| Reference | Existing code to reuse | New or changed work |
| --- | --- | --- |
| Shared shell | top navigation, streak, keyboard focus | geometric logo, My learning / All courses / Settings, avatar derived from saved display name, vivid blue action palette, inset-free page compositions |
| All courses | catalogue projections, current-revision completion, exact resume | separate My learning view, resume panel, searchable course-card grid, real counts, empty search state, daily-fire panel |
| Streak | localStudyDay, studyStreak, saved studyDays | 3/7/14/30 milestones, next milestone, Fuel your flame routing, daily target and dismissible in-app nudge |
| Lesson module rail | modulePanel, independent scrolling | grouped/collapsible modules, current/completed/pending states, course progress, help shortcuts; no invented prerequisites or fake locked courses |
| Lesson reading | explanation, stage dispatcher, walkthrough | reference-style material column, numbered learning instructions, previous/next lesson controls, Learn / Notes / Discussion tabs, bookmark |
| Code workbench | TextInput editing model, authored source, grading controller | multiline code editor, line numbers, code theme/size, reset, draft save status, Output / Tests panels, run/check action with explicit execution contract |
| Completion | deterministic checks, assessment revisions | Mark lesson complete contract, completion feedback, next challenge; retain independent-attempt semantics and optional fresh checks |
| Personal study tools | single local profile / generation storage | bounded persistent notes and bookmarks, display name/initials, daily goal, preference form with explicit save, editor-only light/dark theme, font size and nudge toggle |
| Discussion | offline hints only; no service exists | explicit offline-unavailable state with Notes shortcut; no fabricated people, comments or replies |
| Future-language cards | four real C/C++ lessons | C/C++ remain real courses; any other languages are truthful previews only if approved, with no made-up lesson/hour promises |

## Confirmed premises and remaining contracts

1. CONFIRMED: native macOS C++17/raylib remains the deliverable; Python/JS/TS examples in the references are adapted to C/C++.
2. CONFIRMED: implement real editable C/C++ compile/run. This is a new behaviour, distinct from the existing authored deterministic evaluator. Real execution needs an explicit compiler, isolation, timeout, output and filesystem/network contract. Guided authored checks are an alternative, but must not pretend arbitrary source was executed.
3. Local persistence remains the default. Notes, bookmarks, goals and preferences can work offline; a shared discussion service requires a separate service/dependency decision.
4. All application functions retain the agreed Lizard CCN limit of 15. Preserve saved progress and evidence; no existing profile files are removed.

## Existing-code leverage

- src/core.hpp/.cpp: Profile, Store, Controller, revision-aware evidence, activity and durable help barriers.
- src/main.cpp: named navigation, catalogue, module rail, stage renderers, assistant, settings, frame and journey driver functions.
- src/text_input.*: bounded ASCII editing, caret, selection, clipboard/history behaviour; multiline requires a distinct tested contract.
- src/ui_layout.hpp: shared drawing/hit-test geometry and independent course panels.
- src/app_options.*: isolated launch/scenario arguments and headless content check.
- scripts/check_complexity.py and Makefile quality: all-source CCN gate with pinned Lizard.
- Existing seven CTest suites and fourteen executable journeys form the regression baseline.

## Architectural alternatives for review

| Approach | Coverage / benefits | Costs and risks | Initial recommendation |
| --- | --- | --- | --- |
| Native full feature version | Reuse controller/storage/build; recreate all three layouts with personal tools and a separately specified workbench | Multiline editing, persistence migration and local execution need real design/testing | Preferred for the existing project |
| Native visual refresh with guided checks | Smallest runtime expansion; keeps existing safe evaluator | Cannot claim arbitrary Run code or shared Discussion works; less screenshot feature coverage | Valid only if user chooses guided checks |
| Browser or embedded-web rewrite | Web-style editor/layout reuse may be easier | Replaces existing stack, build, persistence and integration; new dependencies require approval | Not recommended without explicit platform change |

## Review order and implementation handoff

CEO → Design → Engineering → Developer experience, sequentially. Confirm premises before continuing. Record independent voice availability truthfully; unavailable Claude/Codex voices are N/A, never consensus. Log mechanical/taste/user-challenge decisions and produce error/failure registries, architecture/test diagrams, test plan and per-phase tasks. The final Autoplan gate precedes implementation.

After approval, first prove the isolated signed helper; then implement one complete C and C++ vertical slice with persisted source and truthful completion; then finish shell/catalogue, personal preferences/study tools, remaining labs and all screen states. Rebuild, run the complete regression/quality checks, inspect standard/compact native captures in bounded passes and package the executable. Preserve the two pre-existing untracked journals. No push, PR, external publishing, new production dependency or hosted service is implied.

## Decision Audit Trail

| # | Phase | Decision | Classification | Principle | Rationale | Rejected |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | Intake | Screenshots are visual authority for this version | Mechanical | P1 | User explicitly supplied the desired design | Another speculative style exploration |
| 2 | Intake | Keep CCN ≤15 and preserve assessment evidence | Mechanical | P4/P5 | Previously confirmed hard requirements; existing implementations can be reused | Resetting history or weakening the gate |

## Phase 1 — CEO review (SELECTIVE EXPANSION)

### Premise challenge and outcome

The actual problem is that the current executable looks and behaves less like a composed learning product than the supplied references. A colour-only pass would leave missing notes, editable practice and personal preferences unresolved. Doing nothing retains functioning grading but does not meet the user's explicit visual or feature request.

The user confirmed native macOS C/C++ and real compile/run. Native full-feature implementation is selected over a web rewrite: it retains the tested controller, local progress and delivered app rather than rebuilding them. Arbitrary program execution reverses the earlier non-execution product boundary; this reversal is explicit, and acceptance now depends on enforced isolation rather than an unrestricted subprocess.

### Existing-code leverage and scope check

The inventory above maps every screenshot region to existing ownership. Reuse Store for evidence, Controller for grading, TextInput for single-line controls and the current activity functions for legacy days. Introduce only three coherent capabilities: personal study storage, multiline editing and a runner; renderer helpers remain local functions with CCN ≤15.

The plan necessarily touches more than eight files because it changes presentation, local state and execution together. That size is justified by explicit requested features; no provider, web server, authentication system or generic plugin framework is added. Implement in vertical slices and retain executable acceptance after each slice.

### Dream-state mapping

```text
CURRENT                         THIS VERSION                       12-MONTH IDEAL
4 authored lessons + grading -> reference-style native study app -> complete reviewed C/C++ curriculum
local evidence and hints        notes, bookmarks, real C/C++ lab    author tools and supported execution
basic profile settings          personal goals and editor prefs   accessible, multi-device verified learning
```

This version closes the composition and personal-study gap, not the entire curriculum or service ecosystem. It deliberately leaves future authoring, native accessibility and broader device support visible as follow-ups. Those are not quietly represented by invented courses, fake comments or mastery badges.

### Alternatives and temporal interrogation

The alternatives table above was evaluated: the native full version has the highest useful coverage while preserving existing investments. Guided checks alone cannot meet the newly confirmed Run code behaviour. A web rewrite incurs a new runtime and migration without improving the core evidence contract.

Human implementation hours 1–2 establish isolated execution feasibility and persistence contracts; hours 3–5 build multiline editing and shell composition; hours 6+ integrate module completion and visual verification. With coding assistance the same decisions are sequenced into focused sessions; elapsed estimates remain unvalidated until the runner spike succeeds. The load-bearing decisions are compiler discovery, isolation support, source bounds, completion semantics and old-profile preservation; none is left to an accidental UI implementation.

### Selective expansion decisions

Five adjacent improvements were considered: search-clear/empty state, save/cancel preferences, jump from compiler diagnostics to a line, unsaved-editor navigation guard, and accessible keyboard shortcuts. Include them because they directly complete requested interactions; each is small compared with its owning feature. Reject analytics, cloud accounts and extra-language placeholder curricula because they add unrelated infrastructure or misleading content.

### Section 1 — Architecture

The UI sends immutable source snapshots to a single runner job and receives bounded results tagged with source revision. A separate personal-study store owns drafts, notes and preferences so evidence serialization does not absorb a large blob of editor text. The renderer reads these models; it cannot set independent-assessment flags from successful compilation.

```text
Native renderer -> Controller -> evidence Store (existing)
        |-------> StudyStore -> personal preferences / notes / drafts / bookmarks
        |-------> CodeEditor model -> immutable RunRequest
        |                              |
        |                         Runner worker -> isolated compiler -> isolated executable
        |                              |
        +<-------- bounded RunResult / diagnostics / output / authored test results
```

Happy path: source -> validated request -> isolated compile -> isolated test runs -> matching result -> visible success. Missing compiler produces setup instructions; empty source produces an inline validation error; process or save failure leaves learning data readable. Jobs cannot overlap, stale results cannot enable completion, and leaving the lesson cancels the owned job before the next request.

### Section 2 — Error and rescue map

| Codepath | Named failure | Recovery | User sees |
| --- | --- | --- | --- |
| Compiler discovery | CompilerUnavailable | Keep reading/practice available; offer setup steps | Install Command Line Tools to run C/C++; no fake output |
| Isolation preflight | IsolationUnavailable | Fail closed; do not spawn compiler or executable | Run unavailable on this system; authored practice still works |
| Source validation | SourceTooLarge / InvalidSource | Keep draft and reject request | Source limit / encoding guidance at editor |
| Compile | CompileFailed / CompileTimedOut | Return bounded diagnostics; terminate owned process group on timeout | Error line, cause and next edit; no completion success |
| Execute | RuntimeFailed / RuntimeTimedOut / OutputLimit | Stop owned job; retain truncated output labelled as such | Exit/signal/timeout/output-limit result |
| Personal save | StudySaveFailed / UnsupportedStudyVersion | Preserve old files; retain session draft; explicit retry | Unsaved changes with retry and location details |
| Evidence save | Existing Controller failure | Existing pending-submission/help barrier | Save details; do not reveal gated help or award completion |
| Search | NoMatches | Clear-query action | No matching C/C++ courses, with clear search |

### Section 3 — Security and privacy

The new attack surface is compiler input, executable execution and disk-backed personal text. Compiler arguments are constructed as an argv array without a shell; inherited environment is reduced to an allowlist, paths are internally generated, and diagnostics are displayed as text. Both compilation and execution require enforcement of filesystem/network boundaries, time/resource/output limits and ownership of child processes; an unavailable boundary must never fall back to unrestricted execution.

Notes and display names are local personal data and are not uploaded or inserted in diagnostics. Bound every field and aggregate stored size, preserve unknown formats, and exclude user text from debug logs and captures unless using synthetic fixtures. The app adds no account, credentials, network API or new production package in this plan.

### Section 4 — Data integrity and migrations

Advance evidence format explicitly for receipts and completion ledger, with an immutable pre-upgrade backup. A separately versioned study file holds personal fields without treating them as grading authority; use a new schema marker, atomic write and the existing single-writer ownership decision. Unsupported/damaged study files remain preserved and explicitly read-only rather than silently becoming empty valid preferences.

Lesson completion is separate from the historical first-attempt classification. Mark complete requires current-revision practice requirements, matching authored workbench test success when that lesson has tests, and a successful durable completion write. Existing correct-answer activity days are preserved as earned legacy days; new completed lessons add at most one day and never manufacture past completions.

### Section 5 — Code quality

The measured baseline is 230 source/header functions with maximum Lizard CCN 15 and zero violations. Put models and runner logic in named modules, use small plain records and avoid adding another giant UI switch. Reuse one dropdown, one multiline model and one save-notice path rather than creating separate inconsistent versions for each screen.

### Section 6 — Testing strategy

Test personal-state serialization first with current, unsupported, corrupt, read-only and interrupted writes. Test editor operations as a model, runner boundaries with synthetic authored fixtures, and result invalidation before wiring any success banner. Then exercise native executable journeys for all three reference screens, searching, notes/bookmarks, cancelled preferences, run/edit/rerun, completion, streak idempotence and restarts while retaining the fourteen existing journeys.

No live LLM change is included, so no model eval is required. The runner suite must prove denial cases as well as hello-world output, including forbidden reads/writes/network, shell-metacharacter source, timeout, excessive output and child cleanup. These automated tests are necessary but do not claim formal sandbox verification or native accessibility compliance.

### Section 7 — Performance

The app remains single-user and offline; database query and connection-pool concerns do not apply after inspecting the current flat-file storage and in-memory catalogue. Keep compilation off the render thread and cap one job, 64 KiB source, 64 KiB visible output, 64 KiB notes per lesson and 2 MiB aggregate study payload. Bound undo history and debounce draft autosaves; scrolling must not parse or write the full document every frame.

The slow paths are compiler launch, first SDK read and atomic disk sync. Display Compiling/Running status and Cancel, and keep text, notes and navigation responsive while the worker is busy. Wall-time limits, not optimistic timing estimates, define the failure behaviour.

### Section 8 — Observability

Use a job ID, stage, duration and reason code for local runner diagnostics, without source, notes, display names or raw environment values. The user-facing output records exit code, truncation and whether tests match the current source revision. No hosted telemetry, alerts or dashboard is appropriate for this offline academic app; repeatable CLI test reports and explicit in-app save/run states provide the needed evidence.

### Section 9 — Rollout and rollback

Ship an additional local bundle/ZIP only after migration fixtures and runner isolation tests pass. Retain old evidence snapshots and the previous executable; personal-study schema is independent, so rollback leaves the older executable's assessment data readable. Do not publish, push, merge, notarize or add CI as part of the review; those remain separate explicit delivery instructions.

```text
baseline fixtures -> isolation spike -> models/tests -> UI slices -> all regression journeys
                 -> standard/compact visual checks -> signed local bundle + ZIP
rollback: stop new app normally -> reopen prior bundle -> old evidence readable; new study file preserved
```

### Section 10 — Long-term trajectory

The renderer stays native and the compiler worker becomes a reusable learning-lab capability without becoming the grading authority. Persistence schemas and run-result contracts are documented so Oleksandr can add authored tests without changing UI code. The main long-term risk is relying on an isolation interface the OS may retire; discovery, fail-closed behaviour and a documented supported baseline make that risk visible rather than silently unsafe.

### Section 11 — Design intentionality

The supplied screenshots are visual authority, including their course cards, compact blue buttons and warm streak panel even where a generic design rule would prefer another layout. Native C/C++ content replaces Python examples; the page architecture is copied in spirit and measured proportions, not with fabricated curriculum totals. At compact widths the module rail becomes a drawer and the material/workbench becomes a persistent two-tab workspace rather than squeezing three unreadable columns.

```text
My learning <-> All courses/search -> lesson Learn / Notes / Discussion
       |                              | material + editable workbench
       +------ Settings/preferences   + output/tests -> Mark complete -> next lesson
```

The full interaction-state matrix is required in Phase 2. Discussion retains an explicit offline-unavailable state with a Notes shortcut; there are no invented peers or shared comments. This is a taste choice to surface at the final gate because the screenshots do not show the tab's contents.

### Failure Modes Registry

| Path | Failure mode | Rescued? | Required test | User sees | Logged |
| --- | --- | --- | --- | --- | --- |
| Runner | isolation cannot start | Fail closed | preflight failure | unavailable + cause | reason only |
| Runner | malicious or runaway code | enforced limits | deny/timeout/output/process tests | bounded error | job stage/reason |
| Editor | navigate during run or unsaved draft | cancel/preserve | navigation race/restart | saved/unsaved state | status only |
| Completion | result belongs to older source | invalidate | edit-after-pass | rerun required | revision IDs |
| Study storage | corrupt/newer file | preserve/read-only | corrupt/version fixtures | recovery guidance | code only |
| Goals | duplicate completion or clock/day transition | idempotent ledger | same-day/restart/revision cases | truthful goal/streak | day/count |

Critical gaps: isolation feasibility is still an open acceptance gate, not an assumed solved implementation. No row may ship with silent failure or no corresponding test. Phase 3 must resolve the concrete isolation contract before the final approval gate.

### NOT in scope

Cloud accounts/sync, live shared discussion hosting, provider AI, OS push notifications, extra-language curricula, downloaded editor frameworks, CI/CD and public release infrastructure. None is needed to reproduce the requested native study experience with truthful local functionality. The Discussion tab's local scope is surfaced as a taste choice, not misrepresented as a shared forum.

### CEO completion summary

- Outcome and three reference surfaces mapped; existing code leverage specified.
- Premises passed explicitly: native C/C++ and real compile/run.
- Architecture, happy/missing/empty/error paths, data integrity, error/rescue and failure registries recorded.
- Selective additions complete requested flows; unrelated services remain out of scope.
- Independent Codex strategy review is recorded below; Claude voice unavailable because CLI auth probe returned failure. Missing voice is N/A, not agreement.
- Strategy readiness: 8/10 pending executable-isolation feasibility and independent review; this is a plan score, not implementation acceptance.

### CEO state and interaction edge cases

```text
Editor: Clean --edit--> Dirty --save--> Saving --success--> Clean
                         ^                +--failure--> Unsaved/Error --retry--+
Runner: Idle -> Compiling -> Running -> Passed / Failed / TimedOut / Cancelled
            +--preflight failure--> Unavailable (no process spawned)
        edit/new lesson while active -> cancel owned job -> Dirty
        late result with old source/lesson revision -> Discarded (never enables Complete)
Completion: Ineligible -> RequirementsMet -> Saving -> Complete
                                     failure -> RequirementsMet + unsaved message
```

Double Run is disabled while a job exists; Cancel is idempotent and stops the owned job rather than another application process. Navigating away cancels execution and preserves the draft before changing scope; a failed draft save leaves an explicit keep-editing/discard-session decision. Preferences are edited in a draft form and require Save; Cancel restores the last committed settings and editor preview.

Empty or malformed personal data cannot become a successful migration. Unknown lesson IDs and old assessment revisions remain preserved but are excluded from current completion, search and goal counts. Out-of-order worker messages carry job/source/lesson revision IDs, so an older result cannot overwrite a newer output or completion state.

```text
Run error -> bounded reason + retained draft -> edit/rerun OR setup guidance
Save error -> retained session state + retry -> atomic successful write OR stay unsaved
Rollback -> prior app reads prior evidence -> newer personal file stays preserved
```
### CODEX SAYS — CEO strategy challenge

The independent CLI reviewer returned seven findings and an initial score of 6/10: isolation mechanism absent (critical); duplicate completion authority, unrunnable source fragments and cross-store receipt consistency (high); milestone sequencing, daily-goal ledger and overlapping Discussion/private questions (medium). Read-only review completed successfully. Claude CLI authentication was unavailable; no Claude findings or cross-model agreement is claimed.

| Dimension | Claude | Codex | Consensus |
| --- | --- | --- | --- |
| Premises | N/A | native and runner accepted; isolation unresolved | N/A, one voice |
| Right problem | N/A | actual editable lesson content must be specified | N/A, one voice |
| Scope calibration | N/A | sequence around executable feasibility | N/A, one voice |
| Alternatives | N/A | require working isolation spike | N/A, one voice |
| Risks | N/A | completion and stores need one authority | N/A, one voice |
| Trajectory | N/A | goals need ledger; avoid redundant discussion | N/A, one voice |

### CEO decisions from outside review

Adopt runner feasibility before UI implementation, then one complete C and C++ vertical slice, then the remaining layouts. Replace the vague completion proposal with one Controller-owned revision-aware predicate consumed by catalogue, next lesson, My learning and goals; retain `completed()` as explicitly practice-question count. Phase 3 must specify complete translation units, source/test identity and a durable completion receipt, not trust compilation alone.

Daily targets count distinct lesson-completion records, not active-day booleans; preserve legacy earned days without fabricating counts. Personal drafts are not a second grading authority: save source identity before recording completion and define crash/restart rules in Phase 3. Remove the reviewer's correctly identified redundant private-question expansion; retain the requested Discussion tab with an honest offline-unavailable state and a Notes shortcut. This changes our proposed expansion, not a user-specified shared-backend requirement, and remains a final taste choice.

| # | Phase | Decision | Classification | Principle | Rationale | Rejected |
| --- | --- | --- | --- | --- | --- | --- |
| 3 | CEO | Native full feature approach; real runner | User confirmed | P1/P4 | Explicit D1/D2 replies; reuse tested app | Web rewrite / fake Run |
| 4 | CEO | Isolation spike before UI build | Mechanical | P1/P5 | Critical feasibility finding must become evidence | Unrestricted subprocess fallback |
| 5 | CEO | One Controller completion predicate | Mechanical | P4/P5 | Avoid progress/goal/navigation disagreement | Renderer-owned completion |
| 6 | CEO | Durable source/test identity and completion ledger | Mechanical | P1 | Restart and crash consistency are required | Boolean day set as quantitative goals |
| 7 | CEO | Offline Discussion state links to Notes | Taste | P3/P4 | No backend exists; avoid duplicate private journals | Fake community / new unapproved service |
| 8 | CEO | Two runnable language fixtures before remaining screens | Mechanical | P5 | Prove real source/test semantics before scaling | Styling all pages before execution works |

Phase 1 complete. Seven independent concerns recorded, no Claude voice and no two-model consensus. Critical execution feasibility remains a required Phase 3 spike before final approval; the review proceeds to design, not implementation.

## Phase 2 — Design review

### Scope, reference authority and measured palette

This is an Operate surface replacement within the existing native product, not a marketing page. Initial design completeness was 5/10: the screenshots specified the visual world but not narrow layouts, errors, persistence or runner states. The three user images serve as the approved mockups; no alternate concept or generated artwork is needed.

Pixel-frequency inspection of the source PNGs confirmed the dominant colours: page #101114, card #17191d, code area #131519, border #2c2e34, raised control #1f2126, accent #638af2, warm streak panel #1b1816 and completion banner #1c2720. Use these tokens centrally rather than mixing the former periwinkle palette into the replacement. Body text remains at least 16 logical pixels; editor size is independently selectable at 12/14/16/18/20/22/24 px with default 14, while overall reading size retains its existing 16–24 px contract.

The reference is authoritative over generic style prohibitions: course cards, the warm fire panel, avatar circle, labelled kicker and restrained rounded controls are intentional. The logo is redrawn with native geometry as the reference's open angular C/waypoint mark; no imported font, web framework or icon dependency is required. Use the installed macOS sans and mono fonts already used by the executable, with native crisp sizing and no redistribution; do not claim the unknown reference typeface was identified.

### Pass 1 — Information architecture (initial 6/10; specified 9/10)

Global navigation is My learning, All courses and Settings, followed by streak and derived profile initials. My learning leads with the current resume destination and enrolled/started C/C++ courses; All courses leads with the resume + daily-fire row, then search and the real course-card grid. Settings is a two-column preferences form and streak/context rail; no technical profile paths or reset action competes with the main preference form, but both remain in an expandable Learning data section.

```text
Header: logo | My learning | All courses | Settings                 streak | initials
All courses: heading -> resume (2/3) + daily fire (1/3) -> search -> C / C++ cards
Settings:   heading -> Profile & learning / editor controls (2/3) + daily fire/info (1/3)
Lesson:     module rail | Learn / Notes / Discussion + material | code editor
                                                       footer: Output / Tests + completion
```

The course cards show the current four authored modules across two real courses, not six fabricated languages or invented hour estimates. A first-time resume card becomes Start your first lesson with the first C module; completed-current-course routes to its summary and offers the other track. All headings, progress denominators and action states derive from the same Controller projection.

### Pass 2 — Interaction states (initial 3/10; specified 9/10)

| Feature | Loading/busy | Empty | Error | Success | Partial/stale |
| --- | --- | --- | --- | --- | --- |
| Catalogue | Local rendering; no fake spinner | First lesson action | Invalid catalogue retains diagnostics, valid content still readable | Real course cards | Only available modules counted |
| Search | Immediate local filter | No matches + Clear search | Oversize text bounded at input | Matching C/C++ cards | Query retained across course navigation |
| Resume | Opening saved scope | Start first lesson | Old revision returns explanation with note | Exact stage/question restored | Draft restored separately, not graded |
| Preferences | Saving, button temporarily disabled | Name optional; initials CW | Fields retained + cause/retry | Preferences saved | Unsaved form remains draft; Cancel reverts |
| Code editor | Compile/run state + Cancel | Empty source helper | Source/size/encoding error | Current saved source status | Dirty or save failed labelled separately |
| Runner | Compiling then Running; responsive UI | No output is a valid empty output | Compiler diagnostics / exit / timeout / cap | Real stdout, exit code and test results | Prior result labelled stale after edit |
| Tests | Test progress for current job | No tests for this exercise | Failed expectation explains expected vs actual | N/N authored tests passed | Not run / cancelled / old source |
| Notes | Debounced local save status | Write your first note | Unsaved text + retry | Saved on this Mac | Draft retained on failure |
| Bookmark | Immediate pending save | Outline bookmark | Revert saved marker / explain error | Filled icon + text tooltip | Unsupported old record preserved |
| Discussion | No pretend network load | Discussion is not connected in this offline app | Not a silent failure | Notes shortcut available | No fake peers or posts |
| Completion | Saving completion | Requirements shown | Save error; no new goal credit | Completion banner + Next lesson | Old run cannot enable Complete |
| Daily fire | Immediate local projection | 0 days / First spark | Save failure does not create earned day | Milestone + remaining-to-next | Legacy days preserved, no invented lesson count |

Output and Tests tabs are real views of a job result, not fabricated badges. Reset editor affects only that lesson's draft after confirmation and cancels any active job; Reset learning data has its separate existing confirmation and never executes a shell command. Restoring editor source does not reset historical attempts or achievement receipts.

### Pass 3 — Learner journey (initial 5/10; specified 9/10)

| Step | Learner action | Likely feeling | Design response |
| --- | --- | --- | --- |
| 1 | Opens My learning | Where was I? | Named course/lesson and Continue |
| 2 | Chooses module | Wants orientation | Current/completed icons and truthful progress |
| 3 | Reads then edits | Wants a manageable experiment | Authored complete starter and short numbered instructions |
| 4 | Runs code | Anticipation / uncertainty | Compiling/Running states, Cancel and actual output |
| 5 | Hits compiler error | Frustration | Line location, readable message and edit/rerun action |
| 6 | Passes tests | Confidence | Current source test summary; no mastery claim |
| 7 | Marks complete | Closure | Saved achievement, daily goal update and next lesson |
| 8 | Returns later | Wants continuity | Exact resume + restored notes/bookmarks/draft |

At five seconds the page explains its course and action; at five minutes a learner has edited, run and understood an example. Long-term value comes from trustworthy continuity and useful notes rather than fabricated progress or repeated celebrations. Streak feedback celebrates activity without punishing an unmet personal daily target.

### Pass 4 — Specificity and visual quality (initial 8/10; specified 9/10)

The reference supplies a specific composition, so keep its flat dark page, modest bordered cards, vivid blue controls and warm streak module. Do not add gradients behind text, decorative charts, extra cards around every paragraph or a replacement design-world exercise. The workbench, left course rail and central lesson content have distinct backgrounds and clear separators, with restrained hover/selection/focus cues.

The existing offline assistant moves into an explicit Help/hints disclosure within material rather than occupying the code pane. This preserves help persistence and the three authored hint levels without competing with the requested editor. Reading or revealing exercise help in any tab still passes through Controller::help before content appears.

### Pass 5 — Design system alignment (initial 4/10; specified 9/10)

On implementation, replace DESIGN.md's superseded visual-world sections with these reference tokens and surface specifications; preserve truthful accessibility and input caveats. Use one button/dropdown/field vocabulary across the course cards, preference form and workbench. The font-size control keeps a visible label, keyboard selection, outside-click/Escape dismissal and focus restoration, while editor theme changes only editor/output areas.

Spacing follows 8/16/24/32 logical pixels, 44-pixel minimum clickable controls, 8-pixel button corners and 10–12-pixel panel corners. Normal/hover/pressed/disabled/selected/focused states are specified for every shared primitive. Contrast is measured during implementation: if the reference's blue/white combination fails 4.5:1 for normal text, retain the blue and use a sufficiently dark text foreground or minimally deepen the fill rather than copying an inaccessible combination.

### Pass 6 — Responsive and native input (initial 3/10; specified 8/10)

At width ≥1280, use a 216-pixel module rail, remaining workspace split approximately 40% reading / 60% workbench, with each main pane independently scrollable. At 960–1279, use a closable module drawer and persistent Material / Code tabs; switching tabs preserves caret, scroll, output, pending job and notes. At the 960×640 minimum, catalogue and settings stack resume/form above the fire panel and course cards become one column; content never relies on horizontal page scrolling.

Keyboard actions include Tab/Shift+Tab through visible controls, Enter/Space activation, Escape dismissing one overlay layer, editor arrows/Home/End/selection/undo/redo, and explicit shortcuts shown in Help. Focus is stable by semantic control identity in the new surfaces and is restored after dropdowns/drawers; background controls cannot activate beneath an open overlay. Native VoiceOver/IME remain existing limitations, not claims solved by a styled custom renderer; the plan includes a focused feasibility follow-up rather than false compliance.

### Pass 7 — Resolved choices and remaining taste (initial 4/10; specified 9/10)

Use two available course cards, optional personal display name with CW fallback initials, saved local preferences, and honest old-progress migration. Keep the Discussion tab as offline unavailable with a Notes link rather than inventing a forum. Compact Material / Code tabs are the only major layout adaptation from the wide references and will be shown at the final gate.

| Decision | Chosen outcome | If left unresolved |
| --- | --- | --- |
| Screenshot Python | Complete C/C++ examples | Wrong curriculum |
| Empty/current source | Actual editor snapshot, bounded | Fake or stale pass results |
| Name in screenshot | User-editable local name; no hardcoded surname | Wrong identity |
| Other-language cards | No fabricated courses | Misleading catalogue |
| Discussion | Explicit offline state | Fake social feature |
| Narrow panes | Material / Code tabs + drawer | Unreadable squeezed columns |

### Design litmus scorecard

| Litmus | Primary review | Claude | Codex |
| --- | --- | --- | --- |
| CWaypoint identity clear | Yes | N/A | Not individually rated; findings below |
| Clear anchor and primary action | Yes, resume/editor | N/A | Not individually rated; findings below |
| Headings scan independently | Yes | N/A | Not individually rated; findings below |
| One job per section | Yes | N/A | Not individually rated; findings below |
| Cards justified | Course/resume/streak are distinct interactions | N/A | Not individually rated; findings below |
| Motion serves hierarchy | No decorative motion required | N/A | Not individually rated; findings below |
| Works without shadows | Yes, flat reference surfaces | N/A | Not individually rated; findings below |

Overall design-plan readiness 8.9/10 by primary review, limited by real native accessibility and the pending execution result contract. This is not a rating of a built UI. Independent design voice and amendments are recorded below.

### CODEX SAYS — design UX challenge

The independent design reviewer scored the initial plan 7/10 and found six issues: unproven pane size budgets (high), incomplete completion presentation (high), generic runner recovery (high), keyboard/indentation conflict (high), missing transition/save recovery (medium), and ambiguous goal/streak projections (medium). All are accepted as contract fixes below. Claude is unavailable; the litmus consensus remains single-voice N/A, not two-model agreement.

### Design contracts added after review

At 1280 width the module rail is 216 px and the remaining width is split 40/60: material ≈426 px and workbench ≈638 px, before internal 16-pixel padding. At height 640, a 72-pixel header and 20-pixel status strip leave 548 px; the workbench reserves 40 header, 44 run controls, 120 scrollable output/tests and 64 completion, leaving 280 for its scrollable editor. No result panel or completion control expands the overall workspace beyond the window. At 960–1279, Material/Code tabs use the full content width and the 216-pixel module drawer overlays it with focus containment; tests must cover 1280×640 and 960×640 at 24-pixel editor size, long diagnostics and an open drawer.

| Completion state | Rail/cards | Footer text | Action |
| --- | --- | --- | --- |
| Practice incomplete | Practice N/2 | Complete both practice questions | Go to missing question |
| Practice done, tests not run | Ready for lab | Run your exercise to check it | Run code |
| Compile or runtime failed | In progress | Fix the current error | Focus diagnostics/editor |
| Tests failed | In progress | N/M authored checks passed | Edit/rerun |
| Result stale | In progress or historical completed | Code changed; run again | Run code; Complete disabled |
| Eligible current result | Ready to complete | All required checks passed | Mark lesson complete |
| Completion saving | Pending | Saving completion | Disabled until durable result |
| Completion saved | Completed receipt | Lesson completed on this Mac | Next lesson |
| Later edits after completion | Historical Completed retained | Current draft untested; prior achievement retained | Rerun draft; no duplicate day credit |

Run compiles/executes and reports actual program behaviour. Check answer retains existing authored assessment semantics. Mark lesson complete records the Controller-owned achievement only after current eligibility and durable save; it never grades an independent first attempt.

| Runner state | Exact user-facing outcome | Primary recovery | Retained data |
| --- | --- | --- | --- |
| Compiler missing | C/C++ compiler not found. Install Apple's Command Line Tools to run this exercise. | Setup instructions | Source, notes and practice |
| Isolation unavailable | Code execution cannot be isolated on this Mac. Your code has not been run. | Run diagnostics / continue reading | Everything |
| Compile error | Compilation failed, line N: bounded compiler diagnostic | Go to line N | Source + diagnostics |
| Runtime nonzero/signal | Program stopped: exit/signal reason | Edit and run again | Source + bounded output |
| Timeout | Program exceeded its time limit and was stopped | Inspect loop / edit | Source + partial labelled output |
| Output cap | Output limit reached; program stopped | Reduce output / rerun | First bounded output, clearly truncated |
| Stale result | Code changed since this result. Run again to check this draft. | Run code | Old result marked stale |

The first language fixtures are complete C17 and C++17 programs, not fragments. Phase 3 defines their input/output tests and source identity so Output, Tests and completion cannot be conflated. A zero exit code does not itself mean authored tests passed.

Inside the multiline editor, Tab indents by four spaces and Shift+Tab dedents selected lines. Escape first exits editor text focus and returns to the Code toolbar; a second Escape closes a drawer/popover, then normal navigation applies. Command+A/C/X/V/Z/Shift+Z, Command+Left/Right and Option+Left/Right are supported; editor caret and selection are separate from UI focus. Display name uses bounded single-line ASCII in this version, matching the current input bridge; Notes/editor also use the explicit ASCII input contract, with pasted unsupported characters rejected visibly rather than silently corrupted. Full IME is not claimed.

| Transition | Active run | Dirty/saving/failed save | Focus/persistence |
| --- | --- | --- | --- |
| Lesson change | Cancel owned job and wait for acknowledgement | Save source/notes; on failure Stay / Discard session edits | Next lesson heading; saved edits untouched |
| Leave Settings | No new job started | Save / Discard form edits / Stay | Original initiating navigation restored on Stay |
| Window close | Cancel and join worker | Save dirty text; on failure Stay / Discard session edits | Close only after explicit outcome |
| Resize/breakpoint | Job continues | No implicit discard/write | Active pane preserved; editor focus moves to Code tab if hidden |
| Theme/font preview then Cancel | No execution change | Revert committed editor preferences | Return to field that opened preview |
| Dropdown/drawer dismiss | No execution change | Draft intact | Return to trigger; background cannot activate |

Today's goal shows X/Y distinct lesson completions today; the streak shows consecutive earned days and is labelled separately. A target change updates the denominator immediately only after Save and never removes earned days. Day rollover uses the local calendar on resume and foreground refresh; milestones announce once per newly reached threshold, and legacy activity days contribute to historical streak without inventing today's completion count.

Measured normal white text on the sampled accent #638af2 is 3.26:1; dark #101114 text on that accent is 5.80:1. Prefer white text on a minimally deeper #446fd6 primary fill (4.68:1), preserving the sampled accent for links and selections. This is a mechanical contrast correction to the same visual direction, not a new palette choice.

### Design decisions and phase completion

| # | Phase | Decision | Classification | Principle | Rationale | Rejected |
| --- | --- | --- | --- | --- | --- | --- |
| 9 | Design | Reference compositions/tokens, measured contrast correction | Mechanical | P1/P5 | Concrete approved visual source and readable controls | Speculative replacement style |
| 10 | Design | Compact drawer + Material/Code tabs | Taste | P1/P5 | Preserve usable pane measures at minimum width | Squeeze three narrow panes |
| 11 | Design | Historical achievement vs current-source tests shown separately | Mechanical | P1 | Editing must invalidate tests without rewriting history | Silent stale completion |
| 12 | Design | Explicit editor Tab/Escape and unsaved-navigation contracts | Mechanical | P1/P5 | Prevent indentation/focus and data-loss surprises | Generic keyboard-support promise |
| 13 | Design | Separate goal denominator and streak-day numerator | Mechanical | P4 | Different facts need distinct labels | Treat active days as lesson count |

All seven dimensions were examined and the six findings have concrete specification fixes. Primary design readiness remains 8.9/10 because native VoiceOver/IME and final runtime rendering are not yet solved. Phase 2 complete; pass the fixed design contract to Engineering. Independent initial design score 7/10 is retained rather than relabelled as a verified post-fix score.

## Phase 3 — Engineering review

### Scope challenge and actual-code evidence

Read Profile/Store/Controller in src/core.hpp/.cpp, the standalone lesson adapter and full starter catalogue, the extracted rendering/editing functions in src/main.cpp, CMakeLists.txt and the seven existing test targets. The current executable has no multiline editor, compiled-program test contract, completion ledger, persisted display name/notes or worker lifecycle. Those are new capabilities; the reference cannot be implemented solely by restyling existing buttons.

The measured application baseline is 230 functions with maximum CCN 15 and no violations. Preserve the gate for every added C/C++/Objective-C++ function; resource/process operations belong in a small macOS-specific runner boundary rather than UI event handlers. No LLM/provider prompt changes exist, so model eval suites are not applicable after inspecting the authored-hint codepaths.

### Isolation feasibility experiment — actual result, unresolved gate

A safe planning probe successfully launched `/usr/bin/true` under a deny-default sandbox-exec profile. Two bounded attempts to start the actual Apple clang++ compilation under restrictive profiles aborted with signal 6 before producing diagnostics; the exact cause is not established. A narrowed `clang++ --version` check also aborted, locating the failure before user-source compilation. No learner program was run and no unrestricted fallback was attempted.

Stop adjusting that profile after the two failed compilation experiments. Apple's installed sandbox.h says that interface is deprecated and recommends App Sandbox; Apple's current documentation describes resource restrictions through entitlements. The revised primary implementation approach is a separately signed App Sandbox helper/XPC boundary with no network/user-file entitlements, using Apple frameworks already present on macOS. Its first task is a working compiler+execution spike with verified inheritance, narrowly scoped toolchain/SDK reads and job-container writes; packaging, signing and child executable behaviour remain unverified until that spike runs.

This is an unresolved feasibility gate, not a solved sandbox or a reason to run arbitrary source unsafely. Final approval can authorize implementation beginning with that helper spike, but the runnable-version acceptance cannot pass until the required deny/resource/cleanup tests pass. If the helper also fails, stop and present the concrete runner architecture alternatives; do not silently downgrade Run to fake evaluation.

Sources: installed macOS SDK sandbox.h and https://developer.apple.com/documentation/security/app-sandbox . Planning artifacts: `.scratch/autoplan/isolation_spike.py` and `.scratch/autoplan/isolation/results.json`.

### Section 1 — Architecture and ownership

```text
CWaypoint.app (native UI, main thread)
  + AppOptions / existing Controller and evidence Store
  + StudyStore (preferences / notes / bookmarks / editor drafts)
  + CodeEditor (bounded multiline text + semantic focus)
  + RunCoordinator (one job, immutable request, cancellation, bounded mailbox)
       -> signed isolated macOS helper [feasibility gate]
             -> clang C17 / clang++ C++17 [compile-stage restriction]
             -> compiled program in per-job container [runtime-stage restriction]
       <- RunResult(profileId, generation, jobId, lessonId, assessmentRevision, testRevision, sourceHash,
                    stage, bounded diagnostics/output, exit/signal, testResults)
  + Controller::lessonState() [canonical projection for every progress consumer]
```

UI input remains on the main thread; compiler/process reads occur on one owned worker and publish immutable messages. The helper protocol is bounded and validated on both sides; it receives source and authored test inputs, never an arbitrary command/path/env from the UI. RunCoordinator owns cancellation, child lifetime and stale-message rejection; changing lesson or closing waits for owned job shutdown without terminating unrelated processes.

Discover compilers using xcrun once, retain their absolute trusted toolchain/SDK paths and do not search a learner-controlled PATH. Use argv without a shell, an empty/allowlisted environment, per-job generated names, no credential inheritance and closed inherited descriptors. Compile wall timeout 15 seconds, run wall timeout 2 seconds per test, bounded CPU/file/open-descriptor/process resources, 64 KiB combined visible output and capped source/test counts are part of the helper acceptance contract; choose platform-supported enforced limits during the spike and document any unavailable guarantee as a blocker.

### Runnable authored content contract

Add a `LabExercise` associated with a stable lesson ID, language standard, starter translation unit, test revision, bounded stdin cases and exact expected stdout/exit policy. C uses complete `#include <stdio.h>` and `int main(void)` source; C++ uses complete `#include <iostream>` and `int main()` source. No user string is inserted into compiler flags and no test can specify an external file path or shell command.

The first C variables lab reads two bounded integers using scanf, prints their sum and returns zero; tests use `2 3\n -> 5\n` and `-1 4\n -> 3\n`. The first C++ strings lab reads a token using `std::cin` and prints `Hello, <token>!\n`; tests use `Ada\n -> Hello, Ada!\n` and `Lin\n -> Hello, Lin!\n`. Starters are editable full files with a small TODO that initially fails at least one expected test; the learning text explains input and output. Remaining C pointers and conditions get separately authored full programs/test cases before their workbench can be advertised as available.

The user's Run action executes the visible source with a visible example input and reports actual stdout/exit. Authored tests run that same immutable compiled snapshot against the declared cases; successful compilation, zero exit and all tests passed are three separate facts. Source hash, lesson assessment revision and test revision identify the result; edit/reset/lesson change immediately marks its UI result stale.

### Canonical completion, source identity and storage

Introduce one Controller-owned `lessonState(lessonId)` returning practice count, historical achievement, current run eligibility and an explicit reason/action. All course cards, rail icons, next lesson, My learning, daily goal and Complete controls use it. Existing `completed()` stays the practice-count primitive, never a renamed boolean lesson completion.

Evidence format must advance explicitly to carry completion receipts and a bounded completion ledger; this is a material migration, superseding the earlier preference to leave all evidence serialization unchanged. A receipt records lesson ID, assessment revision, test revision, source SHA-256, job success token and completion day. First-attempt evidence fields are never copied from runner success and fresh checks remain optional. Recovered histories cannot become independent successes.

Use a separate versioned personal file for notes/preferences/drafts, under the same profile writer lock. Persist a source snapshot and its SHA-256 before committing the corresponding completion receipt; a crash after source save but before receipt leaves an uncompleted draft. A crash after receipt commit preserves historical achievement; subsequent edits invalidate current test results but not the earned receipt. A missing/damaged source snapshot means the current editor restores an authored starter with an explicit recovery notice, while the historical receipt remains an achievement record, not a claim that the current code passes.

Migration ordering: create and fsync a dedicated immutable pre-upgrade backup; load validated v1/v2/v3; derive only practice history and legacy days; write the new generation/schema atomically; then switch to the new app. Do not invent workbench receipts for old solved questions. On rollback, the prior bundle uses an explicitly copied pre-upgrade profile; newer files stay preserved. An old executable is not promised to display new achievements. Tests must prove interrupted migration, unsupported newer schemas, simultaneous old/new writers and restart consistency before the new default profile is opened for writing.

Goals count unique `(profileId, generation, localDay, lessonId)` completions; revisions remain receipt provenance, not extra daily credit; clicking Complete twice on the same day adds no credit. Repeating a completed lesson on a later day may earn that day's practice goal after current tests/requirements pass again, while historical course completion remains one achievement. Preserve existing studyDays as legacy earned activity; they contribute to streaks but never fabricated daily completion totals. Cap quantitative ledger records and aggregate snapshot size; show an explicit capacity error rather than silently dropping history.

### Section 2 — Code quality and data bounds

Keep the native renderer's named section functions and move persistent models/runner out of src/main.cpp. Use one dropdown, one form-draft model and one code-editor model; no speculative UI framework or event bus. Suggested owning modules are `study_store.*`, `code_editor.*`, `lab_content.*`, `runner.*` plus a platform helper; CMake owns helper build/signing and no external production package is added.

Source ≤64 KiB; Notes ≤64 KiB per lesson; personal data ≤2 MiB; display name ≤80 ASCII characters; search ≤160 ASCII characters; goals 1/2/3; editor font sizes from the specified finite set; at most one job, sixteen authored test cases and bounded undo history. Both serialization writers and readers validate the same bounds. Failures return typed result records and actionable UI messages; do not catch-all and pretend success.

### Section 3 — Test review, branch and UX coverage

The existing CMake/CTest suites cover learning/storage, answer editing, authored integration, responsive geometry, progress reporting, content validation and launch options. The optional Python gate proves CCN thresholds and an empty-scan failure; `make test` covers the standalone contribution. Read their fixtures rather than inferring coverage from a green build: none currently tests a multiline source editor, compiler isolation, personal store or achievement receipt, so those are real gaps to fill test-first.

```text
Catalogue/search -> projection -> empty/nonempty/revision filters -> UI journey + model tests
Preferences form -> validate -> draft/save/cancel -> store roundtrip/error fixtures + UI journey
Notes/bookmarks/source -> bound -> debounce/atomic save -> reload/recover/unsupported -> storage tests
Editor input -> caret/selection/multiline/indent/undo -> source hash -> editor contract tests
Run -> validate request -> preflight -> isolated compile -> isolated execute/tests -> coordinator integration
   | missing/invalid | compiler error | timeout | cap | forbidden read/write/network | child escape
   + every branch -> failure record -> visible recovery -> assertion + native journey
Edit during run / navigate / close -> cancel & join -> stale-message discard -> race integration tests
Passed current result + required practice -> Complete -> receipt save -> shared projection -> goal/streak
   | stale | save fail | duplicate | old revision | day rollover | restart/crash -> controller/storage tests
Layout -> wide/compact/drawer/24px/long diagnostic -> geometry assertions + bounded native captures
```

| New path/branch | Required test type | Existing? | Decision |
| --- | --- | --- | --- |
| Search query normal/empty/no match/clear | projection unit + executable | No | Add |
| Name/goal/theme/font/nudge save/cancel/error | model/store + executable | Partial font only | Add full preference cases |
| Notes/bookmark/draft bounds and migration | roundtrip/corrupt/read-only fixtures | No | Add |
| Multiline insert/newline/delete/selection/indent/history | model unit tests | Single-line only | Add independent model suite |
| Compile success/error/missing tool | helper integration | No | Add before UI |
| Runtime success/nonzero/signal/timeout/output cap | helper integration | No | Add |
| Denied home/neighbour/symlink/network and process/resource escape | synthetic negative helper tests | No | Mandatory release gate |
| Cancel/navigation/close/stale result/double Run | coordinator race tests + executable | No | Add |
| Complete eligibility/save fail/duplicate/current revision | controller contracts | Practice only | Add |
| Crash between source and receipt, old/new version rollback | storage fault fixtures | No | Add |
| Goal duplicate/target change/day rollover/legacy days | deterministic day-injected unit tests | Streak only | Add |
| Compact/wide panes, hidden-pane focus, open dropdown/drawer | geometry + native captures | Existing fixed panels only | Add |

Synthetic denied-resource fixtures must never contain real secrets. Test process cleanup using only children created by the test, never kill another app or free a port. Every helper failure must become a reasoned UI state; no mocked process result can substitute for the real isolated integration suite. Retain all fourteen existing executable journeys, plus new reference-design scenarios and standard/compact screenshots.

### Section 4 — Performance and responsiveness

The worker drains stdout/stderr continuously into a bounded ring and stops an output-flooding job rather than allocating unbounded buffers. Resource limits apply before compiler/program execution and cancellation reaches all owned descendants; subprocess lifetime is not tied to a blocking UI wait. No per-frame disk writes, process polling loops without deadlines, or full-text layout rebuild is allowed on unchanged editor content.

Measure first C and C++ compile time, worst bounded output, save duration and UI responsiveness on the actual Mac. These are measurements to collect, not invented p99 values. A toolchain missing or isolation failure does not block reading, notes or deterministic practice, but the version cannot be accepted as the promised runnable version until the isolation gate passes.

### Failure modes and critical gaps

The CEO registry remains applicable; add helper launch/signature/entitlement failure, compiler startup abort, unsupported OS/toolchain and cross-store crash windows. Every row has a required test and user-visible result; critical gap remains the unproven supported helper path. Design and code are not accepted merely because a mock shows output or a unit mock returns Passed.

### Engineering completion summary

- Actual code and test infrastructure inspected; no production code changed during review.
- Complete source/test examples, one progress authority, explicit receipt/ledger and rollback contracts specified.
- Required branch/UX coverage mapped; test-plan artifact to be written alongside this plan.
- Source CCN ≤15 retained; no unapproved production dependency or hosted service.
- Isolation spike: deny-default process launch worked; compiler startup did not. Supported helper feasibility remains a hard first implementation/release gate.
- Primary engineering readiness: 7/10 with this disclosed critical gate; independent engineering review and amendments are recorded below.

### CODEX SAYS — engineering architecture challenge

The independent engineering review returned six new findings and an initial score of 7/10: profile generation missing from cross-store/run identity (high), mutable drafts cannot retain receipt source bytes (high), supervisor/control authority must be isolated from learner processes (high), cancellation acknowledgement lacks a deadline (high), revision-based goal keys can double-credit a lesson (medium), and the existing correct-answer activity producer must change explicitly (medium). It confirmed the helper spike is already a disclosed feasibility gate, not evidence of either success or impossibility. No runtime tests or file edits were performed by the reviewer; Claude is unavailable.

### Engineering contract amendments

All personal-state roots, requests, worker messages, source snapshots and receipts carry stable profileId and active generation. Reset first cancels/invalidates jobs, then advances the generation durably; late results from the previous generation are rejected. Display name, editor theme/font, goal and nudge preference survive Reset learning progress; generation-scoped notes, bookmarks, drafts, achievement receipts and goal credit do not reappear in the new generation. Old on-disk artifacts remain preserved according to existing reset semantics.

Separate mutable draft files from immutable hash-addressed successful-source snapshots. The completion transaction verifies snapshot bytes against SHA-256, durably commits the immutable source, then writes the receipt; failed receipt writes leave an unreferenced but preserved snapshot, not a completed lesson. Saving draft B after completing source A never overwrites snapshot A. Capacity failures are explicit and cannot prune a referenced achievement snapshot silently; corruption tests cover snapshot A, draft B and receipt mismatch independently.

The parent coordinator alone owns trusted expected results and completion messages. The learner receives only its own source/program/stdin and stdout/stderr descriptors; no supervisor IPC handle, receipt writer, expected-output store, signing credential, parent profile path or other-job workspace is inherited. Treat stdout as untrusted text and compare it in the coordinator. The helper spike must demonstrate cross-job/supervisor-data denial and no inherited control-channel access; App Sandbox branding alone is not proof of these properties.

Cancellation has deadline-driven states: Running -> Cancelling -> Cancelled, or HelperLost/TerminationFailed. Render throughout; request stop, wait at most 250 ms for cooperative acknowledgement, then escalate termination only within the owned job, with a 2-second shutdown deadline. Late success after cancellation is discarded. Navigation can proceed after cancellation confirmation or a terminal helper-loss error that proves owned execution has stopped; if it cannot be proven, show a blocking runner error, disable future runs and preserve the editor rather than pretending shutdown succeeded. Descendant-held pipes and escaped-child attempts are mandatory acceptance tests.

Daily goal credit deduplicates `(profileId, generation, localDay, lessonId)` regardless of assessment/test revision changes on that day. Historical achievements retain revision provenance; new content revisions may require current study again but cannot produce two credits for the same lesson/day. In the new schema, correct practice answers no longer insert studyDays directly: only a successfully saved completion earns a new day. Preserve imported legacy days unchanged, update both Controller::submit and completion producers plus their existing activity tests, and prove read-only/failed completion earns no day.

| Dimension | Claude | Codex | Consensus |
| --- | --- | --- | --- |
| Architecture | N/A | generation and trust boundaries needed | N/A, one voice |
| Test coverage | N/A | add reset/snapshot/cancel/ledger cases | N/A, one voice |
| Performance | N/A | cancellation deadline needed | N/A, one voice |
| Security | N/A | supervisor and child separation required | N/A, one voice |
| Error paths | N/A | helper-loss terminal state needed | N/A, one voice |
| Distribution | N/A | helper still unproven | N/A, one voice |

| # | Phase | Decision | Classification | Principle | Rationale | Rejected |
| --- | --- | --- | --- | --- | --- | --- |
| 14 | Eng | Signed helper spike first, no unrestricted fallback | Mechanical | P1/P5 | Actual restricted compiler experiment failed | Pretend isolation is solved |
| 15 | Eng | Profile/generation in every state/result identity | Mechanical | P1 | Reset must invalidate old jobs and personal content | Lock as sole identity safeguard |
| 16 | Eng | Immutable receipt-addressed source snapshots | Mechanical | P1/P4 | Hash alone cannot preserve old bytes | Mutable draft as achievement snapshot |
| 17 | Eng | Coordinator owns trusted tests and receipt authority | Mechanical | P1/P5 | Learner stdout cannot forge trusted results | Inherited control descriptors |
| 18 | Eng | Bounded cancellation + explicit terminal failure | Mechanical | P1 | No frozen close/navigation or silent live descendants | Infinite acknowledgement wait |
| 19 | Eng | Daily credit keyed by lesson/day, completion-only producer | Mechanical | P4/P5 | Preserve historical days without duplicate new credit | Revision-key double count / old submit producer |

The test-plan artifact is `/Users/thomas/.gstack/projects/TymofiiZuren-CWaypoint/tymofii-feat-macos-learning-app-test-plan-20261113-reference-design.md`; append the six new cases before implementation. Critical gap count remains one: supported real compiler/execution isolation has not been demonstrated. Phase 3 is complete as a reviewed implementation plan with concerns, not as an accepted runnable build; proceed to DX review.

## Phase 3.5 — Developer experience review (DX POLISH)

### Persona and actual current path

Primary persona: a C/C++ student on an Apple Silicon Mac opening the packaged learning app, with little tolerance for compiler setup or unexplained errors. Secondary persona: either project contributor cloning and building the native source with CMake and raylib. The actual README currently offers Finder/open, then a separate build path requiring Apple command line tools, CMake, pkg-config and raylib; the bundled app embeds raylib and makes no network calls.

Current machine evidence: macOS 27.0, arm64; xcrun finds Apple's installed clang++ in the Xcode toolchain. That is a tested development machine, not a validated minimum OS/second-device baseline. Product type is a native learning application with a developer workbench, not an SDK, hosted API or agent skill; API-key, pricing and hosted-auth onboarding do not apply after inspecting the actual local architecture.

### Developer perspective (evidence-grounded empathy narrative)

I open the README because I want to learn C++, not configure another development environment. The first section tells me to open CWaypoint.app, which is reassuring: I can begin reading without Homebrew or a login. If I follow the build section instead, I suddenly need CMake, pkg-config and raylib, and it is not immediately clear whether those are also required merely to run a lesson. The current app asks me to predict an output and check an answer, so I can make progress, but it does not yet let me edit and execute a real program.

In the new version I expect the code already on screen to be complete enough to run. I want the output to be real and the tests to explain which expectation failed. If my Mac has no compiler, I need one clear setup instruction and a way to keep reading; a generic Failed message would make me think my code is wrong. When I close and reopen the app I expect my source and notes to remain, and I do not expect changing a preference or retrying a lesson to rewrite my earlier attempts. As a contributor, I also need an isolated test profile and explicit commands proving that the helper is safe before I work on its visual controls.

### Competitive benchmark and chosen target

| Tool | Verified onboarding choice | Published comparable TTHW | Source |
| --- | --- | --- | --- |
| C++ Shell | Online C++ compiler/editor | Not reported on inspected source | https://www.cpp.sh/ |
| Compiler Explorer | Execution Only view accepts compiler choice and stdin | Not reported in inspected docs | https://github.com/compiler-explorer/compiler-explorer/blob/main/docs/WhatIsCompilerExplorer.md |
| Exercism C++ | Local toolchain/CMake and track-specific tests; CLI workflow documented | Not reported in inspected docs | https://exercism.org/docs/tracks/cpp/installation and https://exercism.org/docs/tracks/cpp/tests.md |
| CWaypoint target | Packaged app -> available lesson -> real authored starter Run | Target <2 min with compiler/helper ready, not measured | This plan |

Do not invent a competitor timing or equate browser startup with local SDK installation. Choose a competitive 2–5 minute first-run target when prerequisites are ready, with a stretch under two minutes; compiler installation is disclosed separately and can exceed that target. The meaningful moment is editing the visible starter, seeing real output and understanding a passing authored test, delivered in the native workbench without a hosted playground or account.

### Nine-stage developer journey

| Stage | Learner/contributor does | Current friction/evidence | Planned resolution |
| --- | --- | --- | --- |
| Discover | Reads README | Run and build paths can be confused | Separate packaged learner quick start from contributor setup |
| Install | Opens bundle or builds | Developer dependencies listed together | Packaged app needs no raylib install; compiler prerequisite only for Run |
| Configure | Chooses name/goal/editor | New form not implemented | Optional fields and sensible saved defaults, no account |
| First result | Opens C/C++ starter and Run | No current executable source lab | Complete starter + visible sample stdin + real output |
| Real usage | Edits, tests, notes, bookmarks | New saved models needed | Local drafts and explicit stale-result identity |
| Debug | Reads error and fixes line | Existing generic boundary failures | Problem/cause/fix with line navigation and retained draft |
| Test/extend | Runs local suites or adds lesson | Existing CMake/Make/quality are usable | New helper/editor/store suites and lab schema example |
| Package/share | Builds native bundle | Existing ad hoc Apple Silicon package | Embed/sign helper; no CI/public release claim |
| Upgrade/recover | Opens old profile or rollback bundle | New receipts/schema can diverge | Tested migration, immutable backup and explicit rollback profile |

### Pass 1 — Getting started (current 6/10; plan target 9/10)

Provide three learner steps: open the packaged app, select a C/C++ course, Run the visible example. On first Run, preflight the compiler/helper before consuming an attempt and show one setup page if unavailable; deterministic practice and reading remain available. Time to first real output must be measured with a warm toolchain and on a clean-user fixture rather than asserted from these planned clicks.

For contributors retain the actual `cmake -S . -B cmake-build-reference -DCMAKE_BUILD_TYPE=Release`, `cmake --build cmake-build-reference -j4`, `ctest --test-dir cmake-build-reference --output-on-failure` path, with `make test` and the existing pinned `make quality` instructions. Describe helper build/signing output and the tested macOS/toolchain versions. Do not install global packages or Command Line Tools automatically on app launch.

### Pass 2 — API/CLI ergonomics (current 7/10; plan target 9/10)

Keep existing launch flags and isolated data-dir semantics. Add a noninteractive `--check-runner` preflight with human-readable default output, explicit reason/exit code and optional machine-readable report, plus isolated `--reference-test`/runner test commands only if they use the same parser conventions and safe profile requirements. UI source is not a command-line compiler option and must never become shell text.

Lab authors provide language standard, complete starter, bounded stdin/expected output, stable lesson ID and explicit test revision. They do not choose arbitrary compiler flags, include paths, executables or network settings. One C and one C++ fixture demonstrate the complete contract before other lessons are adapted.

### Pass 3 — Errors and debugging (current 5/10; plan target 9/10)

Trace compiler missing, compilation failed and personal save failed. Today no real compiler-run path exists and save feedback is separate from editor state; in the new workbench each failure must name what failed, why, and the next safe action. Examples: Compiler unavailable -> install Command Line Tools / continue reading; Compilation failed line 7 -> Go to line 7 and edit; Note could not be saved -> text retained in this session, Retry / Stay.

Keep full bounded compiler diagnostics in the Output panel and present an actionable summary first. Job reason codes and source revision identify debug reports without logging user source, profile name, notes or raw environment. Helper-loss/timeout is a platform/runtime failure, not a learner answer marked incorrect.

### Pass 4 — Documentation and learning (current 6/10; plan target 9/10)

Update README's delivered-feature truth only after implementation, not when this plan is approved. Document the first C/C++ lab, exact stdin/output behaviour, code-run vs authored assessment distinction, storage migration and native keyboard help beside runnable examples. Add a short troubleshooting section for compiler setup, helper launch/isolation, stale results and failed saves.

A contributor should find the lab schema/example and relevant test command in under two minutes; verify that with an actual clean walkthrough. Keep authoring reference separate from learner Help so product screens do not become implementation documentation. Existing content/evidence contracts remain linked and revised together.

### Pass 5 — Upgrade/migration (current 6/10; plan target 9/10)

The plan must not say old binaries show new receipts. State the migration generation/schema, pre-upgrade backup location and explicit rollback procedure, and prove that unknown formats are preserved. Test new helper launch errors independently from profile upgrades so a runner failure cannot silently migrate or reset a learner's data.

No automatic destructive downgrade or catch-all empty-profile fallback is allowed. Missing compiler/helper does not erase preferences or current evidence. Reset and migration tests include both old-generation run messages and newer study files.

### Pass 6 — Environment/tooling (current 7/10; plan target 8/10)

Use the existing C++17/CMake/raylib and native Apple frameworks; there is no new production package or web runtime. Build the helper from declarative CMake targets and signing inputs, not hand-edited generated bundle files. Existing tests run offline with fresh `.scratch` profiles and only owned test children.

A real minimum macOS/CPU support matrix remains to be measured; current machine 27.0/arm64 is the initial verified environment. No Windows/Linux/Intel or notarization claim is added. Native VoiceOver/IME remains a separate known feasibility gap, not something syntax colouring can solve.

### Pass 7 — Community/ecosystem (current 4/10; plan target 6/10)

The public repository and partner-owned curriculum are the actual collaboration mechanism. Include a short lab-authoring/contribution handoff and a reproducible issue-report template in docs if needed; do not send external messages or create issues during this task. The Discussion screen must state that shared discussions are unavailable in this offline version and route to Notes rather than fabricate a community.

No new public forum, payment plan, telemetry service or plugin ecosystem is warranted by the supplied screens. The existing teacher/partner content review remains necessary for teaching quality even after execution tests pass. A lack of community features is disclosed, not disguised by placeholder comments.

### Pass 8 — Measurement and feedback (current 6/10; plan target 8/10)

Measure packaged time-to-first-output, contributor clean build/test time, helper compile/run latency and persistence restart cases locally. Store only synthetic-fixture evidence and bounded reason codes; no learner analytics upload or hosted monitor is introduced. A later runtime review can compare these measures with the targets because the commands, fixture and acceptance states are explicit.

The skill/agent appendix does not apply: the product is a native learning app, not an AI-agent skill or SDK, after checking its executable and README. No LLM evaluation is required because hints stay authored and network/provider code is not added. The current review scores assess plan completeness, not delivered feature performance.

### DX scorecard and implementation checklist

| Dimension | Current | Planned readiness target |
| --- | ---: | ---: |
| Getting started | 6 | 9 |
| CLI/content authoring | 7 | 9 |
| Errors/debugging | 5 | 9 |
| Documentation | 6 | 9 |
| Upgrade/migration | 6 | 9 |
| Environment/tooling | 7 | 8 |
| Community/ecosystem | 4 | 6 |
| Local measurement | 6 | 8 |

Current average 5.9/10, planned target 8.4/10; these are rubric judgements, not measured user outcomes. Checklist: separate learner/contributor quick starts; document compiler/helper preflight; one runnable fixture per language; error reason/action map; migration/rollback guide; new offline suites; CCN ≤15; packaged launch and local first-output timing. Independent DX review and amendments are recorded below; runtime TTHW and helper feasibility remain unmeasured.

### CODEX SAYS — DX developer-experience challenge

The independent brief-only reviewer scored the DX specification 8/10 and identified five additional gaps: packaged first-launch/Gatekeeper handling, a user-facing Run/Check/Complete vocabulary, exact contributor dependency evidence, interrupted/out-of-space migration recovery, and compile/link capability preflight rather than path discovery alone. These are accepted below; the reviewer explicitly did not validate runtime DX. Claude remains unavailable, so no dual-model consensus is claimed.

| Dimension | Claude | Codex | Consensus |
| --- | --- | --- | --- |
| Getting started | N/A | add first-launch security warning path | N/A, one voice |
| Names/actions | N/A | separate Run / Check answer / Complete semantics | N/A, one voice |
| Actionable errors | N/A | distinguish compiler path from usable toolchain | N/A, one voice |
| Findable docs | N/A | publish exact dependency checks | N/A, one voice |
| Safe upgrade | N/A | interruption and disk-full publication rules | N/A, one voice |
| Environment | N/A | synthetic isolated compile/link preflight | N/A, one voice |

### DX amendments and completion

Document the actual local ad hoc signed bundle opening path and any observed macOS warning; test a clean-user/downloaded-copy launch when a suitable device is available. Never recommend disabling Gatekeeper or other system protections. Until notarization and another device are verified, label this a local Apple Silicon build rather than a public friction-free release.

Use these labels consistently: Run code compiles/executes the current source and runs its authored test cases, recording a run history but not an independent prediction attempt. Check answer records the existing deterministic assessment attempt; invalid/empty inputs follow the evaluator's existing rules. Mark lesson complete writes the eligible achievement receipt and goal credit only after durable save. A fixture must show Compile successful / Program exited 0 / 1 of 2 tests passed as three separate outcomes, with Complete disabled.

Contributor environment observed for this review: macOS 27.0 arm64, Apple clang 21.0.0, CMake 4.4.5 and raylib pkg-config 6.2.0. The implementation quick start must include `xcrun --find clang++`, `xcrun --show-sdk-path`, `pkg-config --modversion raylib` and a clean build directory, then record actual build/test output. These are verified installed versions, not a promise that every other version is supported.

Migration publishes a new supported snapshot only after backup creation, validation, complete write, fsync and atomic rename succeed. A journal/marker distinguishes NotStarted, BackupReady and Published; on reopening, only a complete validated supported snapshot is selected. Disk-full/write/interruption at each boundary leaves the original intact and shows a recovery instruction. The marker itself cannot authorize promotion of a partial file; tests corrupt/truncate it and both candidate snapshots.

Runner preflight must compile and link bounded synthetic C17 and C++17 fixtures through the chosen helper, then execute the resulting binaries under runtime restrictions. It distinguishes CompilerMissing, ToolchainOrSDKUnusable, LanguageUnsupported and IsolationUnavailable; each has a specific safe next action. This resolves the diagnostic design, not the currently unproven helper feasibility.

| # | Phase | Decision | Classification | Principle | Rationale | Rejected |
| --- | --- | --- | --- | --- | --- | --- |
| 20 | DX | Separate packaged learner and contributor setup | Mechanical | P5 | Different prerequisites; fewer confusing choices | One mixed installation path |
| 21 | DX | Real first-launch warnings, no protection bypass | Mechanical | P1 | Ad hoc signing is not notarized distribution | Promise warning-free public install |
| 22 | DX | Run / Check answer / Complete have distinct durable effects | Mechanical | P1/P4 | Keep learning evidence truthful | Zero exit means independent success |
| 23 | DX | Preflight verifies isolated C and C++ compile/link/run | Mechanical | P1 | xcrun path discovery alone is insufficient | Compiler path as readiness check |
| 24 | DX | Atomic migration publication and restart marker tests | Mechanical | P1/P5 | Backup alone cannot resolve partial publication | Silent empty-profile recovery |

Phase 3.5 complete. Current DX rubric average 5.9/10, planned target 8.4/10, initial independent brief score 8/10. TTHW remains unmeasured, with target 2–5 minutes when prerequisites are ready. No additional review cycle is claimed for these amendments; unresolved feasibility and device limitations remain visible at the final gate.

## Cross-phase themes

Execution readiness repeatedly surfaced across CEO, engineering and DX: finding a compiler, compiling safely, executing safely and presenting a trustworthy result are separate obligations. Completion identity repeatedly surfaced across CEO, design and engineering: current source, historical achievement, generation and daily credit cannot share one ambiguous boolean. These are recurring review themes, not independent Claude/Codex agreement; Claude was unavailable throughout, and later Codex prompts included the prescribed prior-phase summaries.

## Final review status and implementation handoff

**DONE_WITH_CONCERNS — planning complete, implementation not started.** All required plan artifacts exist: inventories, architecture/state/error/rollout diagrams, rescue/failure registries, seven design passes, engineering branch/UX test map, external test-plan file, eight DX dimensions, nine-stage journey, empathy narrative, benchmark and decision audit trail. Four independent Codex CLI reviews completed in sequential CEO/design/engineering/DX order; initial scores were 6/7/7/8. Claude authentication was unavailable, and every consensus row therefore records N/A rather than invented agreement.

**One critical implementation acceptance gate remains:** a supported signed helper must demonstrate real C/C++ compilation/execution, denied unrelated resources/control channels and bounded descendant cleanup. The legacy sandbox-exec compiler experiments failed before diagnostics; no cause or safe fallback is assumed. Approval authorizes this feasibility-first implementation sequence, not a claim that the runtime works already.

Taste choices for final confirmation: use Material/Code tabs and a module drawer below 1280 px; keep Discussion visibly offline with a Notes shortcut instead of adding a service; preserve the reference's design while making normal button text meet contrast requirements. Native C/C++, real code execution and CCN ≤15 are already confirmed and are not being re-asked.

## Implementation Tasks (aggregated across phases)

- [ ] **T1 (P1) — Prove compiler and execution isolation on the target Mac**. ceo-review; human ~2–3 days / assisted estimate ~2–4 hours (unvalidated). Files: src/runner.hpp, src/runner.cpp, tests/runner_tests.cpp.
- [ ] **T2 (P1) — Define one revision-aware completion and goals contract**. ceo-review; human ~1–2 days / assisted estimate ~1–2 hours (unvalidated). Files: src/core.hpp, src/core.cpp, tests/core_tests.cpp.
- [ ] **T3 (P1) — Implement approved shell, catalogue and settings compositions**. design-review; human ~2–4 days / assisted estimate ~3–5 hours (unvalidated). Files: src/main.cpp, src/ui_layout.hpp, DESIGN.md.
- [ ] **T4 (P1) — Implement multiline editing, native focus and compact workspace**. design-review; human ~2–4 days / assisted estimate ~3–5 hours (unvalidated). Files: src/code_editor.hpp, src/code_editor.cpp, tests/code_editor_tests.cpp.
- [ ] **T5 (P1) — Add generation-bound study storage, source snapshots and migration**. eng-review; human ~2–4 days / assisted estimate ~3–5 hours (unvalidated). Files: src/core.hpp, src/core.cpp, src/study_store.hpp, src/study_store.cpp.
- [ ] **T6 (P1) — Implement coordinator cancellation and trusted result protocol**. eng-review; human ~2–4 days / assisted estimate ~3–5 hours (unvalidated). Files: src/runner.hpp, src/runner.cpp, tests/runner_tests.cpp.
- [ ] **T7 (P1) — Cover every new state and retain existing executable journeys**. eng-review; human ~2–4 days / assisted estimate ~3–5 hours (unvalidated). Files: tests/core_tests.cpp, tests/code_editor_tests.cpp, tests/study_store_tests.cpp, src/main.cpp.
- [ ] **T8 (P2) — Document learner/contributor setup and actionable runner preflight**. devex-review; human ~1–2 days / assisted estimate ~1–3 hours (unvalidated). Files: README.md, docs/runner.md, src/app_options.cpp.
- [ ] **T9 (P2) — Verify migration, native layouts and packaged executable**. devex-review; human ~1–2 days / assisted estimate ~1–3 hours (unvalidated). Files: tests/core_tests.cpp, docs/implementation-status.md, CMakeLists.txt.

## Pre-gate verification

CEO: premises, alternatives, existing-code map, scope/deferred work, temporal/dream mapping, eleven reviewed sections, rescue/failure tables, diagrams, external voice and completion summary are written. Design: seven scored passes, literal measured palette, state matrix, journey, responsive dimensions, litmus table, outside findings and fixes are written. Engineering: actual source/test inspection, architecture, test map/artifact, error/performance analysis, findings/fixes and critical-gap status are written. DX: persona, first-person narrative, sourced benchmark, nine-stage map, eight scored dimensions/checklist and outside findings/fixes are written.

Audit: 24 decisions recorded, including two final taste choices and explicit user-confirmed premises. Deferred items are linked in TODOS.md. No application source changed, no push/PR/external publication occurred, and pre-existing journals remain untouched. No additional code review, fresh native-design build, sandbox certification or new feature acceptance is claimed by this planning workflow.

## GSTACK REVIEW REPORT

| Review | Primary status | Independent initial score | Remaining concern |
| --- | --- | --- | --- |
| CEO | complete with concerns | 6/10 | execution feasibility first |
| Design | complete specification | 7/10 | built native verification still required |
| Engineering | complete with one critical acceptance gate | 7/10 | supported helper not yet proven |
| DX | complete specification | 8/10 | first-output/device timing unmeasured |

Final approval received: A — Approve and start building. Implementation sequence: implement the helper spike first, prove one C and C++ vertical slice, then finish the supplied surfaces and personal features, run all required tests/captures and package the native executable. Do not mark the runnable version complete unless the helper and end-to-end acceptance gates pass.

## Implementation progress — 14 November 2026

- User approved the reviewed plan and its reference-design/native-runner scope.
- T1 started: building the signed App Sandbox helper spike; runtime capability is not yet claimed.

### T1 signed-helper experiment outcome

The signed App Sandbox helper successfully compiled the complete C++ fixture,
but executing the generated file was denied. Removing the process-limit hypothesis,
signing the child correctly and adding the documented executable entitlement did
not change that outcome; quarantine metadata was observed and left intact. No
unsafe execution or quarantine bypass was performed. See `docs/runner-feasibility.md`.

T1 remains incomplete. A concrete architecture/dependency choice has been presented:
bundled WASI SDK 34 + Wasmtime 50.0.1 (recommended) or Docker-backed execution.
Both require dependency/runtime approval; the UI remains native C/C++ in either
case. Do not download or bundle either option before the user's response.
