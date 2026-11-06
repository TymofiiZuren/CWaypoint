#include "text_input.hpp"
#include <iostream>
#include <stdexcept>
using cw::TextInput;
void require(bool condition, const char *reason) {
  if (!condition)
    throw std::runtime_error(reason);
}
int main() {
  try {
    TextInput input;
    input.insert("coat");
    input.moveTo(2);
    input.insert("d");
    require(input.value() == "codat", "insert at caret rather than end");
    input.backspace();
    require(input.value() == "coat", "backspace before caret");
    input.eraseForward();
    require(input.value() == "cot", "forward delete");
    input.set("Hi Ada");
    input.moveTo(3);
    input.moveTo(6, true);
    require(input.selected() == "Ada", "shift selection range");
    input.insert("Grace");
    require(input.value() == "Hi Grace", "replace selection with paste");
    input.selectAll();
    require(input.selected() == "Hi Grace", "select all for copy");
    input.backspace();
    require(input.value().empty() && input.cursor() == 0, "delete selection");
    input.set("one two");
    input.move(-1, false, true);
    require(input.cursor() == 4, "word left");
    input.move(-1, false, true);
    require(input.cursor() == 0, "word left to start");
    input.move(1, false, true);
    require(input.cursor() == 3, "word right");
    input.moveTo(0);
    input.moveTo(3, true);
    input.move(-1);
    require(input.cursor() == 0 && input.selected().empty(),
            "left collapses selection");
    input.selectAll();
    input.move(1);
    require(input.cursor() == 7, "right collapses selection");
    input.moveTo(999);
    require(input.cursor() == 7, "caret bounded");
    input.set(std::string(1024, 'x'));
    input.insert("y");
    require(input.value().size() == 1024, "input cap");
    input.selectAll();
    input.insert("new\nanswer\t\x01");
    require(input.value() == "newanswer",
            "single-line printable answer filtering");
    input.set("Hi Ada");
    input.moveTo(3);
    input.moveTo(6, true);
    input.insert("Grace");
    require(input.undo() && input.value() == "Hi Ada" &&
                input.selected() == "Ada",
            "undo restores replaced text and selection");
    require(input.redo() && input.value() == "Hi Grace",
            "redo restores replacement");
    input.undo();
    input.insert("Lin");
    require(!input.canRedo(), "new edit discards redo branch");
    input.backspace();
    require(input.undo() && input.value() == "Hi Lin", "undo deletion");
    input.set("");
    for (int i = 0; i < 40; ++i)
      input.insert("x");
    int undone = 0;
    while (input.undo())
      ++undone;
    require(undone == 32 && input.value() == std::string(8, 'x'),
            "undo history bounded to 32 changes");
    input.insert("\n\t");
    require(input.canRedo(), "filtered no-op preserves redo");
    input.set("baseline");
    require(!input.canUndo() && !input.canRedo(),
            "new baseline clears history");
    std::cout << "Text editing checks passed\n";
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
