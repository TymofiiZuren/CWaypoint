#pragma once
#include <cstddef>
#include <deque>
#include <string>
namespace cw {
// Single-line ASCII answer editing, independent of window/input APIs.
class TextInput {
  std::string text;
  std::size_t caret = 0, anchor = 0;
  struct State {
    std::string text;
    std::size_t caret, anchor;
  };
  std::deque<State> past, future;
  State snapshot() const;
  void restore(State);
  void rememberEdit();

public:
  const std::string &value() const { return text; }
  std::size_t cursor() const { return caret; }
  std::size_t selectionStart() const;
  std::size_t selectionEnd() const;
  std::string selected() const;
  void set(const std::string &value);
  void insert(const std::string &value);
  void moveTo(std::size_t position, bool extend = false);
  void move(int direction, bool extend = false, bool byWord = false);
  void selectAll();
  void backspace();
  void eraseForward();
  bool undo();
  bool redo();
  bool canUndo() const;
  bool canRedo() const;
};
} // namespace cw
