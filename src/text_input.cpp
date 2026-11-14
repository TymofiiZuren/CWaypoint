#include "text_input.hpp"
#include <algorithm>
#include <cctype>
#include <utility>
namespace cw {
std::size_t TextInput::selectionStart() const {
  return std::min(caret, anchor);
}
std::size_t TextInput::selectionEnd() const { return std::max(caret, anchor); }
std::string TextInput::selected() const {
  return text.substr(selectionStart(), selectionEnd() - selectionStart());
}
void TextInput::set(const std::string &value) {
  text.clear();
  caret = anchor = 0;
  insert(value);
  past.clear();
  future.clear();
}
void TextInput::insert(const std::string &value) {
  std::string clean;
  for (unsigned char ch : value)
    if (ch >= 32 && ch < 127)
      clean += static_cast<char>(ch);
  if (clean.empty())
    return;
  const auto first = selectionStart(), last = selectionEnd();
  const auto capacity = 1024 - (text.size() - (last - first));
  clean.resize(std::min(clean.size(), capacity));
  if (text.substr(first, last - first) != clean)
    rememberEdit();
  text.replace(first, last - first, clean);
  caret = anchor = first + clean.size();
}
void TextInput::moveTo(std::size_t position, bool extend) {
  caret = std::min(position, text.size());
  if (!extend)
    anchor = caret;
}
namespace {
bool wordCharacter(unsigned char c) { return std::isalnum(c) || c=='_'; }
size_t previousPosition(const std::string& text,size_t next,bool byWord) {
  if(next==0) return next;
  --next;
  if(byWord) {
    while(next>0 && !wordCharacter(text[next])) --next;
    while(next>0 && wordCharacter(text[next-1])) --next;
  }
  return next;
}
size_t nextPosition(const std::string& text,size_t next,bool byWord) {
  if(next>=text.size()) return next;
  if(!byWord) return next+1;
  while(next<text.size() && !wordCharacter(text[next])) ++next;
  while(next<text.size() && wordCharacter(text[next])) ++next;
  return next;
}
}
void TextInput::move(int direction,bool extend,bool byWord) {
  if(!direction) return;
  if(!extend && caret!=anchor) {
    moveTo(direction<0 ? selectionStart() : selectionEnd()); return;
  }
  moveTo(direction<0 ? previousPosition(text,caret,byWord) : nextPosition(text,caret,byWord),extend);
}
void TextInput::selectAll() {
  anchor = 0;
  caret = text.size();
}
void TextInput::backspace() {
  auto first = selectionStart(), last = selectionEnd();
  if (first == last) {
    if (caret == 0)
      return;
    first = caret - 1;
  }
  rememberEdit();
  text.erase(first, last - first);
  caret = anchor = first;
}
void TextInput::eraseForward() {
  auto first = selectionStart(), last = selectionEnd();
  if (first == last) {
    if (caret == text.size())
      return;
    last = caret + 1;
  }
  rememberEdit();
  text.erase(first, last - first);
  caret = anchor = first;
}
TextInput::State TextInput::snapshot() const { return {text, caret, anchor}; }
void TextInput::restore(State state) {
  text = std::move(state.text);
  caret = state.caret;
  anchor = state.anchor;
}
void TextInput::rememberEdit() {
  if (past.size() == 32)
    past.pop_front();
  past.push_back(snapshot());
  future.clear();
}
bool TextInput::undo() {
  if (past.empty())
    return false;
  future.push_back(snapshot());
  restore(std::move(past.back()));
  past.pop_back();
  return true;
}
bool TextInput::redo() {
  if (future.empty())
    return false;
  past.push_back(snapshot());
  restore(std::move(future.back()));
  future.pop_back();
  return true;
}
bool TextInput::canUndo() const { return !past.empty(); }
bool TextInput::canRedo() const { return !future.empty(); }
} // namespace cw
