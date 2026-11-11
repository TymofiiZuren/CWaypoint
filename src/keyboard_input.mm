#include "keyboard_input.hpp"
#import <AppKit/AppKit.h>

namespace cw {
KeyboardModifiers currentKeyboardModifiers() {
  const NSEventModifierFlags flags = [NSEvent modifierFlags];
  return {
      (flags & NSEventModifierFlagShift) != 0,
      (flags & NSEventModifierFlagCommand) != 0,
      (flags & NSEventModifierFlagControl) != 0,
      (flags & NSEventModifierFlagOption) != 0,
  };
}
} // namespace cw
