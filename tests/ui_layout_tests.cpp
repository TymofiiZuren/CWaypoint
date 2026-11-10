#include "ui_layout.hpp"
#include <cassert>
#include <iostream>
int main() {
 for (int w : {960,1280,1600}) {
  auto l = cw::workspaceLayout(w, 640);
  assert(l.x >= 24 && l.x + l.width <= w - 24);
  assert(l.top >= 88 && l.bottom <= 640 - 44);
  if(l.columns) assert(l.codeWidth >= 360 && l.answerWidth >= 400);
  assert(!cw::contentHit(l, 30, 35));
  assert(!cw::contentHit(l, 30, 620));
  assert(cw::contentHit(l, l.x + 5, l.top + 5));
 }
 assert(!cw::workspaceLayout(960,640).columns);
 assert(cw::workspaceLayout(1280,840).columns);
 std::cout << "Workspace layout passed\n";
}
