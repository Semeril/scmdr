// #include "scmdr/integer_rect.hpp"
#include "scmdr/texture.hpp"
#include <scmdr/canvas.hpp>

using namespace std;

int main() {
  // t.at({0, 0}).print();
  Canvas c;
  double x, y;
  // // auto r = IntegerRect{{0, 0}, {10, 10}}.intersect({{50, 1}, {10, 10}});
  auto t = Texture::solidColor({255, 0, 0, 255}, {20, 20});
  while (true) {
    c.fill({255, 255, 255, 255});
    c.draw(t, {(int)x, (int)y});
    c.refresh_if_changed();
    x += 1e-2;
    y += 1e-2;
    // cout << "\033[61;0H" << 1. / c.deltaTime.get();
  }
  // cout << "aaaaaaaaaaaa" << "\033[1;3H\033[48;2;255;0;0m" << "bb";
};