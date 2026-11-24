#include "reference_layout.hpp"
#include <initializer_list>
#include <stdexcept>

int main() {
  for (int width : {960, 1280, 1600, 2036}) {
    for (int height : {640, 840}) {
      const auto layout = cw::referenceLayout(width, height);
      if (layout.width > 760 || layout.width < 680 || layout.x * 2 + layout.width != width)
        throw std::runtime_error("Lesson content must fit in one centered column");
      if (layout.bodyBottom - layout.bodyTop < 450 || layout.bodyBottom + layout.footerHeight > height - 24)
        throw std::runtime_error("Lesson content and fixed actions must fit inside the window");
    }
  }
}
