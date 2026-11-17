#pragma once
#include <array>

struct IntegerRect {
  std::array<int, 2> position, size;
  IntegerRect intersect(const IntegerRect &other) const;
  IntegerRect(std::array<int, 2> position, std::array<int, 2> size)
      : position(position), size(size) {}
};