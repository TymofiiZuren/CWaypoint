---
name: CWaypoint
description: A dark desktop study workspace for C and C++.
colors:
  paper: "#101318"
  surface: "#191e26"
  raised: "#242c38"
  ink: "#e8edf5"
  muted: "#a8b3c4"
  blue: "#a8b4ff"
  pale: "#2b3348"
  line: "#718096"
  success: "#75d6a0"
  error: "#ff9a9a"
---

# CWaypoint dark study workspace

The implemented direction is a charcoal desktop workspace with top navigation, a CWaypoint monogram Home link, a drawn gear menu and fixed rounded internal panels. This replaces the former light sidebar design. UI/UX Pro Max's verified dark-mode style guidance informed the neutral layering and contrast; its broad light education/marketing recommendation was rejected as unsuitable for this native learning app. No web framework, font download or icon package was added.

## Colors and hierarchy

The frontmatter contains the rendered opaque palette. Normal text uses ink; secondary text uses muted. Raised surfaces distinguish code and inputs from the enclosing surface. Periwinkle is the primary action/focus color with dark paper text. Selection has a tonal background and textual/contextual cues. Correct and error feedback include words alongside color.

Calculated sRGB contrast: primary text on panels 14.23:1, secondary text on raised surfaces 6.64:1, primary action text 9.43:1, essential borders on raised surfaces 3.50:1, success text on panels 9.47:1 and error text on panels 8.24:1. These measurements are for opaque token pairs, not a claim of native accessibility compliance.

## Typography

Use the installed macOS Arial and Andale Mono fonts without redistributing them. The existing fallback remains raylib's default font. Page titles use 26 logical pixels, headings 22, body/code the learner's 16–24 setting, labels normally 16–17 and secondary status 14. Long heading/prose text wraps; code preserves source lines and has independent horizontal controls. Existing narrow button labels can still shrink to 13; improving that remaining behavior is a follow-up.

## Window and layout

The window opens at 1280×840 with a 960×640 minimum. Top navigation occupies 80 logical pixels. Content is clipped below y=88 and above the 48-high status area. Workspace width is min(1200, window width minus 64), centered. `src/ui_layout.hpp` owns these measured geometry rules and pointer boundaries.

At workspace widths of at least 1000, practice places example code beside the question/answer panel, separated by 24. Below that threshold, the question, matching code, input and Check answer stack in reading order. The current short starter practice fits at default text size; long questions and larger text require vertical scrolling. One main vertical scroll is retained. There are no draggable panels or saved panel coordinates.

Panels share their drawing primitive, surface, boundary and 16-logical-pixel corner radius. Existing buttons/input/code use raylib proportional rounded rectangles with eight segments. The top logo uses a simple C monogram with a waypoint dot, drawn with primitives rather than emoji or external brand assets.

## Navigation and settings menu

The logo returns Home. Lessons, Review, Progress and Compare remain labelled across the top; the active destination uses a tonal fill and marker. The gear opens Preferences and Keyboard help. Menu selection uses the same button handlers as the application, traps Tab between its entries, dismisses on outside click or Escape, and prevents pointer activation of covered content. Escape returns focus to the gear. Icon tooltips and visible focus are provided, but these custom controls have no native VoiceOver names or roles.

## Learning panels

Home leads with the logo/wordmark and Start or Continue, followed by the existing next-lesson suggestion and review route. Practice groups the input, Check answer, editing actions and result. Undo/redo sit after Check answer so the main action stays close to the field. Editing history never changes saved evidence.

Hints are collapsed behind Get a hint. Expanding the controls does not reveal teaching content or count as assistance; selecting a hint/reveal still goes through the controller's durable help barrier. The three authored levels remain available when expanded. This is not automatic sequential hint unlocking.

`Question::code` optionally supplies the code for that question; otherwise the lesson example is used. Fresh checks have explicit matching examples, including the variables a/b copy example. Code remains authored teaching data, never learner execution. Code-panel copy, focus and independent horizontal positions are retained.

## Save and error feedback

A healthy profile has quiet status without an always-present Retry save action. The controller exposes unsavedChanges after a failed persistence attempt; the UI combines this with store writability, rather than parsing status wording. Save details leads to Settings, where the full notice is wrapped and Retry save is available for writable profiles. Unsupported/read-only profiles retain their existing preservation rules. Reset confirmation and generation semantics remain unchanged.

## Input and validation limits

Tab, Shift+Tab, Enter/Space, Escape and macOS answer-editing shortcuts remain. Covered header/footer regions reject pointer activation. Draft editing is single-line ASCII with a 1024-byte limit; Unicode/IME and VoiceOver remain unimplemented. Focus in the broader immediate-mode interface still uses draw indices, so semantic focus restoration across every possible resize is not claimed.

Source of truth is the native renderer and contract tests. Captures for this increment are under `.scratch/dark-ui/verify-standard-captures`, `verify-compact-captures`, `verify-large-captures` and `verify-smoke-captures`. Automated actions exercise real handlers/input models rather than physical OS key/clipboard events. There is no animation or motion-dependent content to disable.
