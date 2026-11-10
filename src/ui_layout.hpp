#pragma once
#include <algorithm>
namespace cw {
struct WorkspaceLayout {
  float x, top, width, bottom, codeWidth, answerWidth;
  bool columns;
};
inline WorkspaceLayout workspaceLayout(int w, int h) {
  float width = std::min(1200.f, static_cast<float>(w) - 64);
  bool columns = width >= 1000;
  float code = columns ? (width - 24) * .44f : width;
  return {(w - width) / 2, 104, width, static_cast<float>(h) - 48,
          code, columns ? width - code - 24 : width, columns};
}
struct CourseLayout {
  float moduleX, moduleWidth, materialX, materialWidth, assistantX, assistantWidth;
};
inline CourseLayout courseLayout(int w) {
  float left = w < 1100 ? 160.f : 192.f;
  float right = w < 1100 ? 224.f : 280.f;
  float materialX = 16 + left + 16;
  float assistantX = static_cast<float>(w) - 16 - right;
  return {16, left, materialX, assistantX - 16 - materialX, assistantX, right};
}
inline bool contentHit(const WorkspaceLayout &l, float x, float y) {
  return x >= l.x - 16 && x < l.x + l.width + 16 &&
         y >= 88 && y < l.bottom;
}
} // namespace cw
