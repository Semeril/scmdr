#pragma once

#include <bits/stdc++.h>

using namespace std;

template <typename T>
class Grid
{
public:
    Grid() = default;
    Grid(std::size_t width, std::size_t height)
        : m_width(width),
          m_height(height),
          m_data(width * height) {}

    Grid(std::size_t width, std::size_t height, const T &value)
        : m_width(width),
          m_height(height),
          m_data(width * height, value) {}

    T &operator()(int x, int y)
    {
        return m_data[y * m_width + x];
    }
    const T &operator()(int x, int y) const
    {
        return m_data[y * m_width + x];
    }

    std::size_t width() const { return m_width; }
    std::size_t height() const { return m_height; }

    std::vector<T> &data() { return m_data; }

private:
    std::size_t m_width;
    std::size_t m_height;
    std::vector<T> m_data;
};

class ModuleNotInitialized : public std::runtime_error
{
    using runtime_error::runtime_error;
};

class PngNotFound : public std::runtime_error
{
    using runtime_error::runtime_error;
};

struct Position
{
    int x = 0;
    int y = 0;
    auto operator<=>(const Position &) const = default;
};
struct Size
{
    int width, height;
};

struct Rect
{
    Position position;
    Size size;
};

struct Color
{
    int red = 255;
    int green = 255;
    int blue = 255;
    int alpha = 255;
    bool operator!=(const Color &other) const
    {
        return std::tie(red, green, blue, alpha) != std::tie(other.red, other.green, other.blue, other.alpha);
    };
    Color over(const Color &other) const;
};

class Texture
{

    Grid<Color> colors_;
    Size size_;

public:
    int z_index;
    Texture(const char *filename);
    Texture() = default;
    Texture(Grid<Color> colors, Size size, int z_index = 0) : colors_(colors), size_(size), z_index(z_index) {}

    Size size() const;

    virtual Color getPixel(Position position) const;
};
class TextureView : public Texture
{
    Texture tex;
    Position pos;
    Size size;

public:
    int z_index = 0;
    TextureView(Texture texture, Position position, Size size, int z_index = 0) : Texture(Grid(0, 0, Color()), size), tex(texture), pos(position), z_index(z_index) {}
    TextureView(TextureView texture, Position position, Size size, int z_index = 0) : Texture(Grid(0, 0, Color()), size), tex(texture), pos({position.x + texture.pos.x, position.y + texture.pos.y}), z_index(z_index) {}
    virtual Color getPixel(Position position) const override;
};

struct Text
{
    char char_;
    Color color;
};

class Canvas
{
    Grid<Color> colors_;
    Grid<Color> colors_prev_;
    vector<vector<Text>> text_;
    class DeltaTime
    {
        using clock = std::chrono::steady_clock;

        float valueInSec_ = 0.016;
        clock::time_point frame_start = clock::now();
        clock::time_point frame_end = clock::now();

    public:
        void update();
        float get();
    };
    DeltaTime dt;
    int fps_ = -1;

public:
    const int width;
    const int height;

    Canvas(int width = 120, int height = 58);

    float getDeltaTime();

    void setFPS(unsigned int fps);

    void paintPixel(Position position, Color color);

    void refresh(bool optimize = false);

    void fill(Color color);

    void fill(Color color, const Rect &rect);

    void blit(const Texture &texture, Position position);

    void text(string text, Position position, Color color);
};
