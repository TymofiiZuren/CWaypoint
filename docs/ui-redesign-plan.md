# CWaypoint UI redesign plan

Date: 10 November 2026

Status: implementation roadmap with the requested dark-theme/top-navigation/panel increment delivered on 10 November 2026; see DESIGN.md and implementation-status.md for verified behavior. The user selected side-by-side code and answers with stacking on small windows, and guided next actions with free access to every lesson section. The user subsequently requested top navigation, a dark theme, a logo Home link, a settings icon menu, rounded corners and internal windows. The dark theme, top navigation, logo Home link, gear menu and fixed responsive panels are now implemented. Some wider usability refinements below remain roadmap items.

## Purpose and audience

Help a beginner understand the example, enter an answer, interpret feedback and choose the next step without searching through a long page. The primary surface is a macOS learning workspace used on a laptop. Reading supports the task; controls should remain predictable and quiet.

Success means the learner knows where they are, can find the answer field immediately, can request help deliberately and can distinguish completing practice from answering a fresh question independently. The application must not imply mastery from a single correct result.

## Evidence from the current application

- `lessonScreen()` repeats the title, objective and four large stage tabs before the question and full example. Current 960×640 captures show the answer field partly below the fixed footer. Undo/redo, feedback, hints and progression add more vertical distance.
- Practice always displays the lesson example, even when a fresh question describes a different example. In the variables lesson, the fresh question describes a and b while the displayed example uses score. This can confuse a beginner about which code to trace.
- Home combines Start or Continue, a next-lesson suggestion, introductory paragraphs and Explore lessons. These compete for attention instead of giving one clear starting action.
- Navigation uses large button surfaces for six destinations. Selected tabs, selected navigation and action buttons share strong blue treatment, reducing the distinction between location and action.
- The save strip always shows Retry save and renders only the first wrapped status line. A healthy profile gets an unnecessary action; a long failure can lose its useful explanation.
- Hint levels, Reveal answer and an AI configuration explanation are always part of the practice page. Development ownership and implementation details appear in the learning flow.
- Settings changes lesson/code text size, while many labels remain fixed. The custom controls have no VoiceOver bridge. A visual redesign alone cannot establish accessibility support.

These are observations from source and compact renders, not findings from a learner usability study.

## Direction

Use a dark study workspace with navigation across the top and rounded internal panels below it. The CWaypoint logo at the top left returns Home. The top bar contains Lessons, Review, Progress and Compare, with a gear icon menu at the right for Settings and keyboard help. Remove the sidebar. Keep question, input and feedback in one visual group.

Use a charcoal canvas, slightly lighter panels, off-white text and a restrained periwinkle accent. Primary actions use accent fill with dark text; selected navigation uses a tonal background and selection marker. Secondary controls remain quieter. Success/error states always include words.

| Token | Proposed value | Purpose |
| --- | --- | --- |
| Canvas | `#101318` | Main app background |
| Panel | `#191E26` | Internal window surfaces |
| Raised surface | `#242C38` | Menus, hover and input treatment |
| Text | `#E8EDF5` | Primary labels and prose |
| Secondary text | `#A8B3C4` | Supporting text |
| Accent | `#A8B4FF` | Primary actions and focus |
| Boundary | `#718096` | Essential input/control edges |
| Success | `#75D6A0` | Correct/saved states with words |
| Error | `#FF9A9A` | Error feedback with words |

Propose 16px panel corners, 10px controls and 8px input corners, expressed as logical measurements in the raylib implementation. Panel title, surface, border and padding belong to one shared panel component. Avoid decorative nested frames. Calculated opaque sRGB contrast is 14.23:1 for primary text on panels, 7.90:1 for secondary text on panels and 9.43:1 for dark text on accent buttons. Final rendered states and the revised boundary color still require measurement before implementation acceptance.

Proposed type sizes are 26 for page titles, 22 for section headings, 18 for body/code and 16 for control/status labels. Keep lesson/code scaling from 16–24; wrap labels instead of shrinking them to fit. Use the existing system fonts. Draw a simple CWaypoint monogram and gear with existing raylib primitives; no external assets or dependencies are required. Keep the 8/12/16/24/32 spacing rhythm, at least 44-high main controls and visible keyboard focus.

## Navigation and screens

| Surface | Proposed structure and primary action |
| --- | --- |
| Home | Show a prominent CWaypoint logo/wordmark as the page identity, with a brief “Learn C and C++” line. Keep the logo compact enough to retain the main action above the fold. Continue lesson with saved stage/question, or Start first lesson for a new profile. Below it: next unfinished lesson and mistake-review count. Move general introduction to a short first-run explanation. |
| Lessons | C / C++ track switch followed by compact lesson rows: title, objective, two-question completion count and separate fresh-check status. Row action says Start, Continue or Revisit according to real state. |
| Lesson | Compact breadcrumb and title; Read, Practice, Fresh check and Summary stage navigation; responsive example/activity workspace; one primary next action appropriate to current state. |
| Review mistakes | Filter, current count and Start review above a compact queue. Within a retry, show remaining count and return destination; show Next mistake after a result. |
| Progress | Scan-friendly lesson rows with separate Practice and Fresh check columns. Expand a lesson for question details; never combine these into a mastery score. |
| Compare | Topic selector followed by labelled C and C++ examples beside each other when space permits, stacked otherwise. Preserve independent copy/scroll controls. |
| Settings | Reading size with a live sample, keyboard help, saved-progress state and a separated reset section. Technical save details are available here. No empty AI configuration controls. |

Home is reached through the logo, the four study destinations remain labelled across the top, and Settings sits behind the gear icon. The logo and gear have visible focus and tooltip labels Home and Settings. The gear opens a dropdown containing Settings and Keyboard shortcuts; Escape closes it and returns focus to its trigger. Clicking outside dismisses it without activating the content beneath. Keep the native macOS title bar and window controls.

At 960px, shorten navigation spacing before wrapping labels or collapsing anything. Reserve 44×44 logical targets for the logo and gear. If the largest text setting cannot fit the four navigation labels, put Compare in a labelled More menu; never silently remove a destination. Opening Settings swaps the central workspace rather than opening another OS window.

## Lesson workspace

Wide layout, expressed as logical regions rather than a pixel-perfect mockup:

```text
[CWaypoint logo]    Lessons   Review   Progress   Compare       [gear]

C / Variables and assignment                         Saved on this Mac
Read   Practice   Fresh check   Summary
Practice question 1 of 2

+----------------------------+  +------------------------------------+
| Example                    |  | Question and answer                |
| Code             Copy code |  | What does the example print?        |
|                            |  | [ Answer field                   ] |
|                            |  | [ Check answer ]    Get a hint      |
|                            |  | Feedback after submission          |
+----------------------------+  +------------------------------------+

Back to explanation                         Next question after result
```

### Responsive layout

- Target 1280×840 and minimum 960×640 logical window sizes; retain resizing and retina rendering.
- With the sidebar removed, use the full width below the top bar, with 24px outer padding at minimum width and 32px on larger windows.
- Use two columns only when the available workspace supports approximately 360 for code, 400 for activity and a 24 gap. Compute this from the actual available content width rather than the outer window alone.
- At smaller widths stack context above the question/input. Keep the prompt and input ahead of optional explanation and walkthrough material. Collapse reference code only when the question remains understandable without it; code essential to the question stays expanded.
- At 24px text, long prompts or multi-line examples may require scrolling. Preserve text size and a logical reading order; do not promise every future question fits without scrolling.
- Keep one continuous vertical scroll model for the activity. Only code retains independent horizontal scrolling. Avoid adding nested vertical scrolling panes.
- Reserve measured space for fixed chrome. A focused answer, feedback action or last control must scroll fully into the usable viewport, including its focus outline.


### Internal windows

Confirmed: fixed rounded in-app panels for lessons, code and practice, including Example, Question and answer, Walkthrough and Progress. They are contained in the main window, arranged by the responsive layout, with clear titles. They do not have fake macOS traffic-light controls or misleading close buttons. Supporting panels can expand/collapse with keyboard-operable controls; the active question/input stays available.

Panel positions are controlled by the responsive layout. Learners do not drag, resize or rearrange individual panels. Window resizing reflows them automatically; no floating-window layout or position persistence is needed.

### Correct context for each question

The owning question contract should distinguish lesson reference code from question-specific example code. Add an optional question code field to the declarative record, with explicit authoring intent; do not parse executable examples out of prose at render time.

For existing content, retain the lesson example where it is relevant. Give fresh questions their own correctly matched examples where appropriate. Adapting the variables fresh check requires Oleksandr to review the extracted teaching statements and wording. Present the actual a/b example during that question, rather than labelling the score example as its context. Review assessment revision implications before changing any assessed meaning; do not create new evidence for an unchanged assessment solely because layout changed.

### Interaction states

| State | Visible behavior |
| --- | --- |
| Before submission | Answer field and Check answer are the main task. Undo/redo remain keyboard-accessible and discoverable through compact editing controls. Empty input produces local guidance and consumes no attempt. |
| Incorrect | Keep the entered answer visible; show feedback beside it. Try again becomes the primary action. Get a hint is secondary. Do not auto-reveal the answer. |
| Correct | Show the explanation and existing evidence status. Next question becomes the primary action. Practise again remains available as a secondary action. |
| Draft edited after a result | Explain that editing does not change the previous result. Require the existing explicit retry transition before another graded submission. |
| Help opened | Before exposing content, state that hints count as help for this question and honor the controller's durable help barrier. Disclose one level at a time, then offer the next. Reveal answer remains separate and clearly labelled. |
| Fresh check | Explain the first-attempt rule briefly before starting. Save issuance before revealing the question. Preserve the existing ability to use help, with honest evidence status. This is not a progression gate. |
| Completed practice | Offer the fresh check, a next unfinished lesson or a summary, with one emphasized recommendation. Never require a successful fresh check to continue. |
| Optional content unavailable | Explain locally and retain practice access. No full-screen failure for an invalid optional walkthrough. |

Free navigation remains available throughout. Skipping an unanswered question does not mark it solved. A next action recommends a route; it does not unlock or rewrite learning state.

## Save feedback and recovery

Normal operation uses a small Saved on this Mac status. Show Retry save only when a retry is meaningful. Do not infer save success from the writable flag alone: expose structured saved, pending-failure, temporary and unsupported-format states from the owning controller/store boundary.

Failures expand into an inline notice with the full reason, the practical effect and a recovery action. Never truncate the only explanation to one line. Temporary practice must say that results will not be retained. Unsupported newer profiles remain preserved and read-only; a retry must not promise it can downgrade them. New fresh checks stay blocked when issuance cannot be saved.

Reset retains explicit confirmation and preservation semantics. Visual simplification must not weaken any save barrier, duplicate-submission guard or generation protection.

## Accessibility and input

Use words with color, predictable Tab order, visible focus, labelled inputs and familiar macOS editing shortcuts. Do not capture arrows or Command+C globally when the answer editor owns focus. Code-panel keyboard controls must operate only in that panel. On reflow, restore focus by semantic control identity instead of an unstable draw index.

Validate all changed controls at the largest text setting, minimum window and retina scale. Set a proposed contrast target of 4.5:1 for normal text and 3:1 for large text and control boundaries, then measure the chosen tokens during implementation. These targets are not claims about the current renderer.

Native VoiceOver exposure and Unicode/IME require a separate technical feasibility decision. Keep those limits explicit in Settings and project documentation; no accessibility-compliance claim follows from this plan.

## Implementation order

1. **Layout foundation:** implement the dark tokens, top navigation, logo Home link, settings icon menu and rounded panel primitive. Add shared measured regions and component variants for navigation, stage selection, actions and notices. Test viewport geometry, semantic focus restoration and covered-control hit rejection before moving screens.
2. **Lesson first:** implement the responsive workspace, question context contract, adjacent feedback and progressive hints. Extend producer, adapter, validation and consumer together; keep grading and persistence in their existing owners.
3. **Start and finish routes:** simplify Home, catalogue, summary and Review. Replace duplicate recommendations with one state-driven next action, preserving optional navigation.
4. **Supporting screens:** compact Progress, side-by-side Compare and clearer Settings. Expose save states through a shared contract rather than matching notice strings in UI code.
5. **Verification and documentation:** run existing learning/storage/editor/integration checks, targeted new journey tests and packaged launch checks. After acceptance, update DESIGN.md and its sidecar to describe the implemented system. Keep this plan as the proposed design until then.

Use focused validated commits for these increments. No push, new PR, dependencies or release infrastructure is part of the redesign plan.

## Acceptance and usability checks

- At 1280×840 with default text, the current starter practice question, answer field and Check answer are visible together without scrolling. At 960×640, the current short starter question/input/action are visible together using the stacked layout; long and 24px cases remain reachable without clipping.
- Variables fresh-check context displays the matching a/b statements. No unrelated lesson example is presented as the code being assessed.
- Keyboard-only traversal reaches every action in reading order, reveals clipped controls, preserves editing shortcuts and survives window resize without accidental activation.
- Wrong answer → hint → retry → correct → next question retains the original attempts and assisted evidence. Summary and Progress agree with the controller after restart.
- Choosing a section freely does not manufacture completion or bypass fresh-check issuance/help persistence. Save-failure and unsupported-profile journeys remain protected.
- Logo Home link, top navigation and gear menu remain reachable at both sizes; the menu closes safely and restores focus. Dark-theme text, controls and focus indicators meet the stated contrast targets.
- Internal panels reflow without overlap, hidden controls or competing vertical scroll areas.
- New-user Home has one primary start action. Returning Home states exactly what Continue restores. Empty review and completed curriculum have useful next actions.
- Perform a bounded screenshot pass at both target sizes and largest text, fix material findings together, then confirm. Native input and a second Mac require actual device evidence.
- Ask a beginner to start a lesson, answer once, find a hint, continue after restart and find a mistake. Proposed acceptance: each task completed without instruction or a blocking navigation error. Record observed confusion and assistance; do not invent time improvements or usability-study results.

## Remaining decisions

The top navigation, dark theme, logo Home link, gear settings menu, rounded corners, responsive arrangement and free-navigation policy are confirmed. Fixed rounded internal panels are confirmed; draggable windows are outside scope. The user authorized implementation of the dark theme and requested layout through UI/UX Pro Max. Remaining roadmap refinements should be delivered as subsequent focused increments. Question-specific teaching examples need author review, and a screen-reader bridge remains a separate scope decision. No new brand assets or library choices are needed for the proposed build.
