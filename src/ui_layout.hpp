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
inline bool contentHit(const WorkspaceLayout &l, float x, float y) {
  return x >= l.x - 16 && x < l.x + l.width + 16 &&
         y >= 88 && y < l.bottom;
}
} // namespace cw
