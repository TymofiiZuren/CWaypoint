#pragma once

namespace cw {
struct KeyboardModifiers {
  bool shift = false;
  bool command = false;
  bool control = false;
  bool alt = false;
};

inline bool acceptTextCharacter(int codepoint, const KeyboardModifiers &modifiers) {
  return codepoint >= 32 && codepoint < 127 &&
         !modifiers.command && !modifiers.control;
}

#ifdef __APPLE__
// Read the current system state, including releases consumed by system shortcuts.
KeyboardModifiers currentKeyboardModifiers();
#endif
} // namespace cw
