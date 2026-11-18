#include "scmdr/texture.hpp"
#include "stb_image.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

Texture::Texture(shared_ptr<vector<array<uint8_t, 4>>> colors,
                 array<int, 4> fragment,
                 shared_ptr<array<int, 4>> originalFragment)
    : colors(colors), fragment(fragment), originalFragment(originalFragment) {}

Texture Texture::loadPNG(string_view filename) {
  static unordered_map<string, tuple<weak_ptr<vector<array<uint8_t, 4>>>,
                                     weak_ptr<array<int, 4>>>>
      cache;
  if (!(filesystem::exists(filename) && filesystem::is_regular_file(filename) &&
        filesystem::path(filename).extension() == ".png")) {
    throw invalid_argument("filename");
  }
  if (cache.contains((string)filename)) {
    auto &[weak_colors, weak_fragment] = cache[(string)filename];
    if (auto colors = weak_colors.lock()) {
      return Texture{colors, *weak_fragment.lock(), weak_fragment.lock()};
    }
  }
  int chb, width, height;
  auto img = stbi_load(filename.data(), &width, &height, &chb, 4);
  shared_ptr<vector<array<uint8_t, 4>>> colors =
      make_shared<vector<array<uint8_t, 4>>>();
  for (int i = 0; i < width * height * 4; i += 4) {
    colors->push_back({img[i], img[i + 1], img[i + 2], img[i + 3]});
  }
  stbi_image_free(img);
  auto fragment =
      make_shared<array<int, 4>>(array<int, 4>{0, 0, (int)width, (int)height});
  cache[(string)filename] = {colors, fragment};
  return Texture{colors, {0, 0, (int)width, (int)height}, fragment};
}
array<uint8_t, 4> Texture::at(array<int, 2> position) const {
  if (position[0] >= fragment[2] || position[1] >= fragment[3]) {
    throw invalid_argument("position");
  }
  return colors->at((position[1] + fragment[1]) * originalFragment->at(0) +
                    (position[0] + fragment[0]));
}
array<int, 2> Texture::size() const { return {fragment[2], fragment[3]}; }

Texture Texture::subTexture(array<int, 4> fragment) const {
  if (fragment[0] + fragment[2] > this->fragment[2] ||
      fragment[1] + fragment[3] > this->fragment[3] || fragment[2] == 0 ||
      fragment[3] == 0) {
    throw invalid_argument("fragment");
  }
  return Texture{colors,
                 {this->fragment[0] + fragment[0],
                  this->fragment[1] + fragment[1], fragment[2], fragment[3]},
                 originalFragment};
}
