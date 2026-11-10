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
 for (int w : {960,1280,1600,2036,2560}) {
  auto c = cw::courseLayout(w);
  assert(c.moduleWidth >= 160 && c.assistantWidth >= 224);
  assert(c.materialWidth >= 400);
  assert(c.materialX >= c.moduleX + c.moduleWidth + 16);
  assert(c.assistantX >= c.materialX + c.materialWidth + 16);
  assert(c.assistantX + c.assistantWidth <= w - 16);
  // The material uses the full course layout, not the narrower catalogue.
  assert(cw::courseMaterialHit(c,1000,c.materialX+24,200));
  assert(cw::courseMaterialHit(c,1000,c.assistantX-17,200));
  assert(!cw::courseMaterialHit(c,1000,c.moduleX+20,200));
  assert(!cw::courseMaterialHit(c,1000,c.assistantX+20,200));
  assert(!cw::courseMaterialHit(c,1000,c.materialX+24,50));
  assert(!cw::courseMaterialHit(c,1000,c.materialX+24,970));
 }
 std::cout << "Workspace layout passed\n";
}
