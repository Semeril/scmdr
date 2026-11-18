#include "color.hpp"
#include <array>
#include <cstdint>

using namespace std;

Color::Color(array<uint8_t, 4> rgba) : rgba(rgba) {};

Color Color::over(const Color &other) const {
  double alpha1 = rgba[3] / 255.;
  double alpha2 = other.rgba[3] / 255.;
  double out_alpha = alpha1 + alpha2 * (1. - alpha1);
  if (out_alpha == 0.) {
    return Color{{0, 0, 0, 0}};
  }
  uint8_t out_red =
      (uint8_t)((rgba[0] * alpha1 + other.rgba[0] * alpha2 * (1 - alpha1)) /
                out_alpha);
  uint8_t out_green =
      (uint8_t)((rgba[1] * alpha1 + other.rgba[1] * alpha2 * (1 - alpha1)) /
                out_alpha);
  uint8_t out_blue =
      (uint8_t)((rgba[2] * alpha1 + other.rgba[2] * alpha2 * (1 - alpha1)) /
                out_alpha);
  return Color{{out_red, out_green, out_blue, (uint8_t)(out_alpha * 255)}};
}
