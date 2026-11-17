#include "scmdr/integer_rect.hpp"
#include <algorithm>

using namespace std;

IntegerRect IntegerRect::intersect(const IntegerRect &other) const {
  return IntegerRect{
      {clamp(position[0], other.position[0], other.position[0] + other.size[0]),
       clamp(position[1], other.position[1],
             other.position[1] + other.size[1])},
      {clamp(position[0] + size[0], other.position[0],
             other.position[0] + other.size[0]) -
           clamp(position[0], other.position[0],
                 other.position[0] + other.size[0]),
       clamp(position[1] + size[1], other.position[1],
             other.position[1] + other.size[1]) -
           clamp(position[1], other.position[1],
                 other.position[1] + other.size[1])}};
}