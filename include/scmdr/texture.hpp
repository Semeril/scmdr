#pragma once
// #include "scmdr/color.hpp"
// #include "scmdr/integer_rect.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

struct Texture {
private:
  const std::shared_ptr<std::vector<std::array<uint8_t, 4>>> colors;
  const std::shared_ptr<std::array<int, 4>> originalFragment;
  const std::array<int, 4> fragment;

  Texture(std::shared_ptr<std::vector<std::array<uint8_t, 4>>> colors,
          std::array<int, 4> fragment,
          std::shared_ptr<std::array<int, 4>> originalFragment);

public:
  static Texture loadPNG(std::string_view filename);
  static Texture solidColor(std::array<uint8_t, 4> color,
                            std::array<int, 2> size);

  std::array<uint8_t, 4> at(std::array<int, 2> position) const;
  std::array<int, 2> size() const;
  Texture subTexture(std::array<int, 4> fragment) const;
};
