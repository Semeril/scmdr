#include "integer_rect.hpp"
#include <algorithm>
#include <array>

using namespace std;

IntegerRect::IntegerRect(array<int, 4> rect)
    : position{rect[0], rect[1]}, size{rect[2], rect[3]} {}

IntegerRect IntegerRect::intersect(const IntegerRect &other) const {
  return IntegerRect{
      {clamp(position[0], other.position[0], other.position[0] + other.size[0]),
       clamp(position[1], other.position[1], other.position[1] + other.size[1]),
       clamp(position[0] + size[0], other.position[0],
             other.position[0] + other.size[0]) -
           clamp(position[0], other.position[0],
                 other.position[0] + other.size[0]),
       clamp(position[1] + size[1], other.position[1],
             other.position[1] + other.size[1]) -
           clamp(position[1], other.position[1],
                 other.position[1] + other.size[1])}};
}