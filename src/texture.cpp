#include "scmdr/texture.hpp"
#include "stb_image.h"
#include <array>
#include <filesystem>

using namespace std;

Texture::Texture(shared_ptr<vector<Color>> colors, IntegerRect fragment,
                 shared_ptr<IntegerRect> originalFragment)
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
      make_shared<IntegerRect>(IntegerRect{{0, 0}, {(int)width, (int)height}})};
}
Color Texture::at(array<int, 2> position) const {
  if (position[0] >= fragment.size[0] || position[1] >= fragment.size[1]) {
    throw invalid_argument("position");
  }
  return colors->at((position[1] + fragment.position[1]) *
                        originalFragment->size[0] +
                    (position[0] + fragment.position[0]));
}
array<int, 2> Texture::size() const { return fragment.size; }

Texture Texture::subTexture(IntegerRect fragment) const {
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
