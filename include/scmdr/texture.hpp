#pragma once
#include "scmdr/color.hpp"
#include "scmdr/integer_rect.hpp"
#include <array>
#include <memory>
#include <string_view>
#include <vector>

struct Texture {
private:
  const std::shared_ptr<std::vector<Color>> colors;
  const std::shared_ptr<IntegerRect> originalFragment;
  const IntegerRect fragment;

  Texture(std::shared_ptr<std::vector<Color>> colors, IntegerRect fragment,
          std::shared_ptr<IntegerRect> originalFragment);

public:
  static Texture loadPNG(std::string_view filename);

  Color at(std::array<int, 2> position) const;
  std::array<int, 2> size() const;
  Texture subTexture(IntegerRect fragment) const;
};
