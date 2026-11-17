#include <iostream>
#include <scmdr/canvas.hpp>

using namespace std;

int main() {
  Texture t = Texture::loadPNG("example.png");
  // t.at({0, 0}).print();
  Canvas c;
  double x, y;

  while (true) {
    c.fill({255, 255, 255, 255});
    c.draw(t, {(int)x, (int)y});
    c.refresh(0);
    x += 1e-2;
    y += 1e-2;
    cout << "\033[61;0H" << 1. / c.deltaTime.get();
  }
  // cout << "aaaaaaaaaaaa" << "\033[1;3H\033[48;2;255;0;0m" << "bb";
};