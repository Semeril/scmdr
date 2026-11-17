#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>
#if defined(_WIN32)
#include <windows.h>
#endif

using namespace std;

template <typename T> struct Rect {
  array<T, 2> position, size;
  Rect<T> intersect(const Rect<T> &other) const {
    return Rect{{clamp(position[0], other.position[0],
                       other.position[0] + other.size[0]),
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
  void print() {
    cout << format("x:{} y:{} w:{} h:{}", position[0], position[1], size[0],
                   size[1]);
  }
  Rect(array<T, 2> position, array<T, 2> size)
      : position(position), size(size) {}
};

struct Color {
  array<uint8_t, 4> rgba;
  Color();
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  Color over(const Color &other) const;
  void print() {
    cout << format("r:{} g:{} b:{} a:{}", rgba[0], rgba[1], rgba[2], rgba[3]);
  }
};

struct Texture {
private:
  const shared_ptr<vector<Color>> colors;
  const shared_ptr<Rect<int>> originalFragment;
  const Rect<int> fragment;

  Texture(shared_ptr<vector<Color>> colors, Rect<int> fragment,
          shared_ptr<Rect<int>> originalFragment);

public:
  static Texture loadPNG(string_view filename);

  Color at(array<int, 2> position) const;
  constexpr auto size() const { return fragment.size; }
  Texture subTexture(Rect<int> fragment) const;
};

// struct ColoredChar  {
//   char ch;
//   Color color;
// };

class Canvas {
  vector<Color> colors;
  vector<Color> colors2;
  // vector<vector<ColoredChar>> text;

public:
  const array<int, 2> size;
  class DeltaTime {
    using clock = std::chrono::steady_clock;

    double value = .016;
    clock::time_point frame_start = clock::now();
    clock::time_point frame_end = frame_start;

    void update();

  public:
    friend class Canvas;
    double get() const;
  } deltaTime;

  Canvas(int width = 120, int height = 58);

  // float getDeltaTime();
  // void setFPS(unsigned int fps);
  Color at(array<int, 2> position) const;
  void refresh(bool optimize = false);
  void fill(Color color);
  void fillRect(Color color, Rect<int> rect);
  void draw(const Texture &texture, array<int, 2> position);
  // void text(string text, array<int, 2> position, Color color);
};
