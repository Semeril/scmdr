#include "scmdr/canvas.hpp"
#include <fmt/format.h>
#include <iostream>
#include <stdexcept>
#if defined(_WIN32)
#include <windows.h>
#endif

using namespace std;

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

void Canvas::DeltaTime::update() {
  frame_end = clock::now();
  value = chrono::duration<double>(frame_end - frame_start).count();
  frame_start = clock::now();
}
double Canvas::DeltaTime::get() const { return value; }

Canvas::Canvas(int width, int height)
    : size{width, height}, colors(width * height, Color()) {
  if (height % 2) {
    throw invalid_argument("height");
  }
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
#if defined(_WIN32)
  SetConsoleOutputCP(CP_UTF8);
#endif
  cout << "\033[?25l";
  // colors_.assign(height, vector<Color>(width, Color()));
  // text_.assign(height, vector<Text>(width, Text{'\0', {0, 0, 0}}));
  colors2 = colors;
  refresh_simple();
}
// float Canvas::getDeltaTime() { return dt.get(); }
// void Canvas::setFPS(unsigned int fps) { fps_ = fps; }

Color Canvas::at(array<int, 2> position) const {
  if (position[0] >= size[0] || position[1] >= size[1]) {
    throw invalid_argument("position");
  }
  return colors[position[1] * size[0] + position[0]];
}

void Canvas::refresh_if_changed() { // much faster
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
      if (has_changed(col, row)) {
        output.append(setCursor(row / 2 + 1, col + 1))
            .append(getColoredForeground(colors_get(col, row)))
            .append(getColoredBackground(colors_get(col, row + 1)))
            .append("\u2580");
      }
    }
  }
  output.append("\033[0m");
  colors2 = colors;
  cout << output;
  deltaTime.update();
}
void Canvas::refresh_simple() {
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
      output.append(setCursor(row / 2 + 1, col + 1))
          .append(getColoredForeground(colors_get(col, row)))
          .append(getColoredBackground(colors_get(col, row + 1)))
          .append("\u2580");
    }
  }
  output.append("\033[0m");
  colors2 = colors;
  cout << output;
  deltaTime.update();
}

void Canvas::fill(Color color) { colors = vector(colors.size(), color); }

void Canvas::fillRect(Color color, IntegerRect rect) {
  const auto &[position, size] =
      rect.intersect(IntegerRect{{0u, 0u}, this->size});
  for (int row = position[1]; row < size[1]; row++) {
    for (int col = position[0]; col < size[0]; col++) {
      if (row < 0 || row >= size[1] || col < 0 || col >= size[0])
        continue;
      colors[row * size[0] + col] = color.over(colors[row * size[0] + col]);
    }
  }
}

void Canvas::draw(const Texture &texture, array<int, 2> position) {
  auto textureRect = IntegerRect(position, texture.size());
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