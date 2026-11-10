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

The implemented direction is a charcoal course workspace with top navigation, a CWaypoint monogram Home link, a drawn gear menu, course cards and fixed rounded internal panels. This replaces the former light sidebar design. UI/UX Pro Max's verified dark-mode style guidance informed the neutral layering and contrast; its broad light education/marketing recommendation was rejected as unsuitable for this native learning app. No web framework, font download or icon package was added.

## Colors and hierarchy

The frontmatter contains the rendered opaque palette. Normal text uses ink; secondary text uses muted. Raised surfaces distinguish code and inputs from the enclosing surface. Periwinkle is the primary action/focus color with dark paper text. Selection has a tonal background and textual/contextual cues. Correct and error feedback include words alongside color.

Calculated sRGB contrast: primary text on panels 14.23:1, secondary text on raised surfaces 6.64:1, primary action text 9.43:1, essential borders on raised surfaces 3.50:1, success text on panels 9.47:1 and error text on panels 8.24:1. These measurements are for opaque token pairs, not a claim of native accessibility compliance.

## Typography

Use the installed macOS Arial and Andale Mono fonts without redistributing them. The existing fallback remains raylib's default font. Page titles use 26 logical pixels, headings 22, body/code the learner's 16–24 setting, labels normally 16–17 and secondary status 14. Long heading/prose text wraps; code preserves source lines and has independent horizontal controls. Existing narrow button labels can still shrink to 13; improving that remaining behavior is a follow-up.

## Window and layout

The window opens at 1280×840 with a 960×640 minimum. Top navigation occupies 80 logical pixels. Content is clipped below y=88 and above the 48-high status area. Workspace width is min(1200, window width minus 64), centered. `src/ui_layout.hpp` owns these measured geometry rules and pointer boundaries.

Home and Courses show both C and C++ course cards in one catalogue. Current lessons are the modules: C has variables, pointers and conditions; C++ has strings. Counts derive from the actual catalogue and practice completion; there are no placeholder curriculum totals. Course cards open the first unfinished module, or the first module when the track is complete. Continue restores the saved position.

Course screens use three columns at both supported window sizes: a 160/192-wide module list on the left, material in the centre and a 224/280-wide study assistant on the right. `courseLayout()` in `src/ui_layout.hpp` owns this geometry. The centre remains at least 400 logical pixels wide at tested sizes. Material has its own main scroll; assistant transcript/hint options have a bounded scroll region and an anchored message field/Send action. The module list now has its own clipped scroll region, selected-module reveal and a fixed All courses action. Code preserves independent horizontal reading controls.

Panels share their drawing primitive, surface, boundary and 16-logical-pixel corner radius. Existing buttons/input/code use raylib proportional rounded rectangles with eight segments. A compact streak badge sits before the gear without overlapping navigation at minimum size. It retains Oleksandr’s embedded attributed flame, daily count and milestone glow, with dark surfaces. Inactive and active states were rendered locally; teammate appearance acceptance remains pending. The top logo uses a simple C monogram with a waypoint dot, drawn with primitives rather than emoji or external brand assets.

## Navigation and settings menu

The logo returns Home. Lessons, Review, Progress and Compare remain labelled across the top; the active destination uses a tonal fill and marker. The gear opens Preferences and Keyboard help. Menu selection uses the same button handlers as the application, traps Tab between its entries, dismisses on outside click or Escape, and prevents pointer activation of covered content. Escape returns focus to the gear. Icon tooltips and visible focus are provided, but these custom controls have no native VoiceOver names or roles.

## Learning panels

Home and Courses share the catalogue. Top navigation, lesson sections and secondary actions use quiet text treatments with hover/focus; primary actions retain periwinkle fill. Selecting a module changes the centre material without replacing the course shell. Lesson stages use an underline instead of large filled tabs. Check answer leads before submission; Try again is shown after a result. Editing history never changes saved evidence.

The right-hand Study assistant is explicitly offline. It displays authored notes for reading/summary, or authored hints for the active exercise. Quick prompts and typed messages use the same response path. Free-text prompts advance through authored hints rather than generating a response to arbitrary questions; this is disclosed in the panel. A hint is displayed only after controller help persistence succeeds. It does not grade, execute code or send network requests. Fresh checks with blocked issuance have no assistant reveal. Conversations are session-only, scoped to lesson/stage/question and cleared after a successful progress reset, along with assistant drafts and scroll positions.

`Question::code` optionally supplies the code for that question; otherwise the lesson example is used. Fresh checks have explicit matching examples, including the variables a/b copy example. Code remains authored teaching data, never learner execution. Code-panel copy, focus and independent horizontal positions are retained.

## Save and error feedback

A healthy profile has quiet status without an always-present Retry save action. The controller exposes unsavedChanges after a failed persistence attempt; the UI combines this with store writability, rather than parsing status wording. Save details leads to Settings, where the full notice is wrapped and Retry save is available for writable profiles. Unsupported/read-only profiles retain their existing preservation rules. Reset confirmation and generation semantics remain unchanged.

## Input and validation limits

Tab, Shift+Tab, Enter/Space, Escape and macOS answer-editing shortcuts remain. Covered header/footer regions reject pointer activation. Draft editing is single-line ASCII with a 1024-byte limit; Unicode/IME and VoiceOver remain unimplemented. Focus in the broader immediate-mode interface still uses draw indices, so semantic focus restoration across every possible resize is not claimed.

Source of truth is the native renderer and contract tests. Current captures are under `.scratch/course-ui/release-home-captures`, `release-course-captures`, `journey-captures` and `compact-captures`. Automated actions exercise real handlers/input models rather than physical OS key/clipboard events. There is no animation or motion-dependent content to disable.
