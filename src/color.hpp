#pragma once
#include <array>
#include <cstdint>

struct Color {
  std::array<uint8_t, 4> rgba;
  Color(std::array<uint8_t, 4> rgba);
  Color over(const Color &other) const;
};