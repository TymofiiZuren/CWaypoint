#pragma once
#include <algorithm>

namespace cw {
struct ReferenceLayout {
  float x, width, bodyTop, bodyBottom, footerHeight;
};

inline ReferenceLayout referenceLayout(int width, int height) {
  const float column = std::min(760.f, width - 112.f);
  constexpr float header = 80, footer = 64, status = 24;
  return {(width - column) / 2, column, header, height - footer - status, footer};
}
}
