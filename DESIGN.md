---
name: CWaypoint
description: A calm native desktop workspace for learning C and C++.
colors:
  paper: "#f7f9fc"
  ink: "#192a40"
  muted: "#4f6074"
  blue: "#1b56b1"
  blue-hover: "#144593"
  pale: "#e6eefa"
  line: "#d7e0eb"
  success: "#196b4d"
  error: "#a6342b"
  surface: "#ffffff"
  rail: "#eef3f9"
  code: "#ebf0f7"
  disabled: "#eaeef3"
typography:
  headline:
    fontFamily: "Arial"
    fontSize: "32px"
    letterSpacing: "0.4px"
  title:
    fontFamily: "Arial"
    fontSize: "22px"
    letterSpacing: "0.4px"
  body:
    fontFamily: "Arial"
    fontSize: "18px"
    lineHeight: 1.48
    letterSpacing: "0.4px"
  code:
    fontFamily: "Andale Mono"
    fontSize: "18px"
    lineHeight: 1.5
    letterSpacing: "0.4px"
  label:
    fontFamily: "Arial"
    fontSize: "17px"
    letterSpacing: "0.4px"
spacing:
  compact: "8px"
  paragraph-gap: "12px"
  control-inset: "16px"
  code-gap: "20px"
  section-gap: "24px"
  workspace-inset: "40px"
components:
  button-primary:
    backgroundColor: "{colors.blue}"
    textColor: "{colors.surface}"
    typography: "{typography.label}"
    height: "46px"
  button-primary-hover:
    backgroundColor: "{colors.blue-hover}"
    textColor: "{colors.surface}"
  button-secondary:
    backgroundColor: "{colors.surface}"
    textColor: "{colors.ink}"
    typography: "{typography.label}"
    height: "46px"
  answer-input:
    backgroundColor: "{colors.surface}"
    textColor: "{colors.ink}"
    typography: "{typography.code}"
    height: "50px"
  code-block:
    backgroundColor: "{colors.code}"
    textColor: "{colors.ink}"
    typography: "{typography.code}"
---

# Design System: CWaypoint

## Overview

**Creative North Star: "The Learning Workspace"**

Preserve the approved light desktop interface: neutral surfaces, restrained blue actions, fixed navigation and persistent save feedback. The interface supports reading and deliberate practice through clear text and a predictable workspace. This is the existing visual direction, not a new identity.

**Key Characteristics:**
- Light, flat surfaces with blue actions and visible keyboard focus.
- A stable navigation rail around a vertically scrolling learning workspace.
- Readable prose and monospace examples with explicit learning and save states.

## Colors

The frontmatter records the opaque raylib RGBA palette as equivalent hex values. Blue identifies primary actions and selected controls; pale blue is the secondary hover treatment. Ink and muted text sit on paper or white surfaces. Line separates regions, rail anchors navigation, and code gives examples a quiet tonal background. Success and error colors accompany textual feedback; disabled controls use their own neutral surface.

**The State Has Words Rule.** Keep textual feedback alongside state colors, including saved-progress failures and learning results.

## Typography

Arial is loaded from the macOS system font directory for interface text; Andale Mono is loaded for code and answer input. These fonts are not redistributed. raylib's default font is the fallback if loading fails.

Page titles use headline; section headings use title and wrap at workspace width. Body and code start at the frontmatter size and follow the user setting from 16–24 logical pixels in steps of two. The setting does not resize all UI text: buttons start at the label size and shrink to 13 logical pixels to fit; contextual labels, helper text and save status use fixed sizes from 14–19. DrawTextEx adds the recorded character spacing; this is native drawing, not a CSS font-weight system.

## Layout

The resizable window starts at 1280×840 logical pixels and has a 960×640 minimum; compact capture starts at that minimum. The rail is fixed at 224 wide. Workspace content starts at x=264, y=122 minus its scroll offset; width is min(900, window width − 304). The header divider sits at y=90, and the content clip begins at y=100. A fixed 58-high save strip stays below the scrolling content.

There are no mobile breakpoints or browser reflow rules. Prose wraps by measured words; headings wrap, while code preserves lines and scrolls horizontally with left Shift + wheel when necessary. Wheel and Page Up/Down scroll content; keyboard focus moves clipped controls into view. Navigation and save feedback remain visible.

Row controls are 46 high with a 58-pixel vertical advance. Lesson tabs are 42 high; track, hint and walkthrough controls are 44 high; answer fields are 50 high. Retry save is a compact 36-high control. Do not claim a universal 42-pixel minimum.

## Elevation & Depth

No shadows or animated elevation are implemented. Flat tonal surfaces and thin borders establish hierarchy. Hover and selection change color immediately; the answer caret blinks. Avoid documenting browser transitions as native behavior.

## Shapes

raylib DrawRectangleRounded uses proportional roundness, not fixed CSS radii: buttons use 0.16, answer fields 0.12 and code panels 0.08, with eight corner segments. Secondary button borders are one logical pixel. Keyboard button focus uses a two-pixel blue outline expanded three pixels beyond the control; active input uses a two-pixel blue border.

## Components

- **Actions and selection:** primary buttons have blue fill and white text; secondary buttons have white fill and a line border. Secondary hover uses pale blue; primary hover uses deeper blue. Selected navigation, track, lesson-stage and hint controls reuse the primary variant. Disabled controls are excluded from keyboard activation.
- **Answer field:** monospace text, placeholder, blue caret and active border. Input supports printable ASCII editing up to 1024 characters, with Enter submission. Clicking positions the caret; dragging or Shift+arrows selects text. Character/word movement, Home/End, Command+A/C/X/V, backspace and forward-delete are supported. Selection uses the pale-blue token and the caret scrolls horizontally into view. This is custom editing; full Unicode/IME editing and native screen-reader text-field semantics are not implemented. Drafts are retained per question for the running session.
- **Code panel:** tonal background and inset monospace content. Left Shift + wheel reveals long lines; an explanatory instruction appears when needed.
- **Navigation:** five stable destinations; Lessons remains selected during a lesson. Tab/Shift+Tab moves focus; Enter/Space activates controls; Escape returns or cancels reset confirmation.
- **Save strip:** a persistent notice and Retry save action, with error-colored notice text when the store is unwritable. The current renderer displays only the first wrapped notice line; do not assume multi-line status fits.

Native limitations: raylib custom controls are not exposed to VoiceOver. Minimum supported macOS, other CPU architectures and behavior on a teammate's device still require device verification. HTML/CSS component previews are not applicable to this native renderer. The sidecar records native metadata; screenshots under `.scratch/final-captures` and `.scratch/compact-captures` provide visual evidence.

## Do's and Don'ts

- Do preserve the light neutral surfaces, blue actions and stable workspace.
- Do keep keyboard focus and textual learning/save states visible.
- Do distinguish practice completion from independent first-attempt evidence.
- Don't imply that one correct question proves mastery.
- Don't let optional AI configuration block authored offline learning.
- Don't describe these custom controls as native accessible text fields or browser components.

### Guided progression

Home and completed summaries offer the next unfinished lesson in the selected track using the existing heading, paragraph and primary-button components. When available starter practice is complete, show an explicit completion message and retain access to review. Progress lists individual practice statuses; fresh-check evidence remains separate. Optional invalid walkthroughs/comparisons use the existing unavailable-state text and keep valid learning routes accessible.

### Focused mistake review

Review mistakes is a navigation destination using the existing tokens and controls. Its filters are All tracks / C only / C++ only. Queue entries name the track, lesson and Practice 1/2 or Fresh check, with prompt and status. Focused retries retain the lesson workspace and highlight Review mistakes in navigation. Cleared feedback, Next review question and Back to review list provide the next action. Escape returns to the queue; an empty filter explains how mistakes enter the list and offers Browse lessons.

### Exact resume

Home states the saved practice question, fresh check, summary or walkthrough step before Continue. Continue restores that position after restart using the existing lesson workspace. Unknown/changed content starts from the current explanation. This extends status copy and navigation only; existing tokens/components remain unchanged.

### Code-panel controls

Code panels own their caption, Copy code action, body, focus outline and overflow controls. Keep source at the chosen lesson text size. When long lines overflow, show Scroll left / Scroll right and a position indicator; retain independent offsets for example, walkthrough and comparison panels. Focused bodies support Left/Right, Home/End and Command+C. A brief Copied label applies only while that panel still shows the copied source. The footer/header clip both drawing and pointer activation.

### Undo and redo

Text-answer fields have a compact Undo / Redo row and the Command+Z / Shift+Command+Z shortcut hint. Disabled actions use the existing disabled style and are excluded from traversal. After an action, focus returns to the text field. History restores text/cursor/selection for the current question and remains session-local. Changing a draft after grading replaces stale result copy with an explanation that the previous learning result is unchanged; undo does not change saved evidence.
