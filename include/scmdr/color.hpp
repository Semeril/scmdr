#pragma once
#include <array>
#include <cstdint>

struct Color {
  std::array<uint8_t, 4> rgba;
  Color();
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  Color over(const Color &other) const;
};