#include "scmdr/scmdr.hpp"
#include "stb_image.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

inline string setCursor(int row, int column) {
  return fmt::format("\033[{};{}H", row, column);
}

inline string getColoredBackground(Color color) {
  return fmt::format("\033[48;2;{};{};{}m", color.rgba[0], color.rgba[1],
                     color.rgba[2]);
}

inline string getColoredForeground(Color color) {
  return fmt::format("\033[38;2;{};{};{}m", color.rgba[0], color.rgba[1],
                     color.rgba[2]);
}

auto operator!=(Color c, Color c2) { return c.rgba != c2.rgba; }
} // namespace

Color::Color() { rgba.fill(255); }
Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : rgba{r, g, b, a} {};

Color Color::over(const Color &other) const {
  double alpha1 = rgba[3] / 255.;
  double alpha2 = other.rgba[3] / 255.;
  double out_alpha = alpha1 + alpha2 * (1. - alpha1);
  if (out_alpha == 0.) {
    return Color{0, 0, 0, 0};
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
  return Color{out_red, out_green, out_blue, (uint8_t)(out_alpha * 255)};
}

void Canvas::DeltaTime::update() {
  frame_end = clock::now();
  value = chrono::duration<double>(frame_end - frame_start).count();
  frame_start = clock::now();
}
double Canvas::DeltaTime::get() const { return value; }

Texture::Texture(shared_ptr<vector<Color>> colors, Rect<int> fragment,
                 shared_ptr<Rect<int>> originalFragment)
    : colors(colors), fragment(fragment), originalFragment(originalFragment) {}

Texture Texture::loadPNG(string_view filename) {
  if (!(filesystem::exists(filename) && filesystem::is_regular_file(filename) &&
        filesystem::path(filename).extension() == ".png")) {
    throw invalid_argument("filename");
  }
  int chb, width, height;
  auto img = stbi_load(filename.data(), &width, &height, &chb, 4);
  shared_ptr<vector<Color>> colors = make_shared<vector<Color>>();
  for (int i = 0; i < width * height * 4; i += 4) {
    colors->emplace_back(img[i], img[i + 1], img[i + 2], img[i + 3]);
  }
  stbi_image_free(img);
  return Texture{
      colors,
      {{0, 0}, {(int)width, (int)height}},
      make_shared<Rect<int>>(Rect<int>{{0, 0}, {(int)width, (int)height}})};
}
Color Texture::at(array<int, 2> position) const {
  if (position[0] >= fragment.size[0] || position[1] >= fragment.size[1]) {
    throw invalid_argument("position");
  }
  return colors->at((position[1] + fragment.position[1]) *
                        originalFragment->size[0] +
                    (position[0] + fragment.position[0]));
}
Texture Texture::subTexture(Rect<int> fragment) const {
  if (fragment.position[0] + fragment.size[0] > this->fragment.size[0] ||
      fragment.position[1] + fragment.size[1] > this->fragment.size[1] ||
      fragment.size[0] == 0 || fragment.size[1] == 0) {
    throw invalid_argument("fragment");
  }
  return Texture{colors,
                 {{this->fragment.position[0] + fragment.position[0],
                   this->fragment.position[1] + fragment.position[1]},
                  fragment.size},
                 originalFragment};
}

Canvas::Canvas(int width, int height)
    : size{width, height}, colors(width * height, Color()) {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
#if defined(_WIN32)
  SetConsoleOutputCP(CP_UTF8);
#endif
  cout << "\033[?25l";
  // colors_.assign(height, vector<Color>(width, Color()));
  // text_.assign(height, vector<Text>(width, Text{'\0', {0, 0, 0}}));
  colors2 = colors;
  refresh(false);
}
// float Canvas::getDeltaTime() { return dt.get(); }
// void Canvas::setFPS(unsigned int fps) { fps_ = fps; }

Color Canvas::at(array<int, 2> position) const {
  if (position[0] >= size[0] || position[1] >= size[1]) {
    throw invalid_argument("position");
  }
  return colors[position[1] * size[0] + position[0]];
}

void Canvas::refresh(bool optimize) {
  string output;
  output.append("\033[0m");
  auto colors_get = [&](int x, int y) { return colors[y * size[0] + x]; };
  auto colors2_get = [&](int x, int y) { return colors2[y * size[0] + x]; };
  auto has_changed = [&](int col, int row) {
    return colors_get(col, row) != colors2_get(col, row) ||
           colors_get(col, row + 1) != colors2_get(col, row + 1);
  };

  for (int row = 0; row < size[1]; row += 2) {
    for (int col = 0; col < size[0]; col++) {
      if (has_changed(col, row) || !optimize) {
        output.append(setCursor(row / 2 + 1, col + 1))
            .append(getColoredForeground(colors_get(col, row)))
            .append(getColoredBackground(colors_get(col, row + 1)))
            .append("\u2580");
      }
      // if (text_[row][col].char_) {
      //   output << pos(row / 2 + 1, col + 1) << fg(text_[row][col].color)
      //          << bg(colors_(col, row + 1)) << text_[row][col].char_;
      // }
    }
  }
  output.append("\033[0m");
  colors2 = colors;
  cout << output;
  deltaTime.update();
  // if (fps_ != -1 && dt.get() < (1 / float(fps_))) {
  //   auto tdt = 1.f / fps_;
  //   this_thread::sleep_for(chrono::duration<float>(tdt - dt.get()));
  // }
}

void Canvas::fill(Color color) { colors = vector(colors.size(), color); }

void Canvas::fillRect(Color color, Rect<int> rect) {
  const auto &[position, size] = rect.intersect(Rect{{0u, 0u}, this->size});
  for (int row = position[1]; row < size[1]; row++) {
    for (int col = position[0]; col < size[0]; col++) {
      if (row < 0 || row >= size[1] || col < 0 || col >= size[0])
        continue;
      colors[row * size[0] + col] = color.over(colors[row * size[0] + col]);
    }
  }
}

void Canvas::draw(const Texture &texture, array<int, 2> position) {
  const auto textureRect = Rect<int>{position, texture.size()};
  for (int row = textureRect.position[1];
       row < textureRect.position[1] + textureRect.size[1]; row++) {
    for (int col = textureRect.position[0];
         col < textureRect.position[0] + textureRect.size[0]; col++) {
      if (row < 0 || row >= size[1] || col < 0 || col >= size[0])
        continue;
      colors[row * size[0] + col] = texture
                                        .at({col - textureRect.position[0],
                                             row - textureRect.position[1]})
                                        .over(colors[row * size[0] + col]);
    }
  }
}

// void Canvas::text(string text, Position position, Color color) {
//   // cout << pos(position.y + 1, position.x + 1) << fg(color) << text;
//   int y = (position.y % 2) ? position.y - 1 : position.y;
//   for (int i = position.x; i < clamp(position.x + (int)text.size(), 0,
//   width);
//        i++) {
//     text_[y][i] = Text{text.at(i - position.x), color};
//   }
// }
