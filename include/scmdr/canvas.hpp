#pragma once
#include "scmdr/color.hpp"
#include "scmdr/texture.hpp"
#include <array>
#include <chrono>
#include <vector>

class Canvas {
  std::vector<Color> colors;
  std::vector<Color> colors2;

public:
  const std::array<int, 2> size;
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

  Color at(std::array<int, 2> position) const;
  void refresh(bool optimize = false);
  void fill(Color color);
  void fillRect(Color color, IntegerRect rect);
  void draw(const Texture &texture, std::array<int, 2> position);
};