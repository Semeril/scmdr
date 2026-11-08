#if defined(_WIN32)
#include <windows.h>
#endif

#include "stb_image.h"
#include "scmdr/scmdr.hpp"
#include <fmt/format.h>

using namespace std;

namespace
{
    inline std::string pos(int row, int column)
    {
        return fmt::format("\033[{};{}H", row, column);
    }

    inline std::string bg(Color color)
    {
        return fmt::format("\033[48;2;{};{};{}m", color.red % 256, color.green % 256, color.blue % 256);
    }

    inline std::string fg(Color color)
    {
        return fmt::format("\033[38;2;{};{};{}m", color.red % 256, color.green % 256, color.blue % 256);
    }
}

Color Color::over(const Color &other) const
{
    double alpha1 = alpha / 255.0;
    double alpha2 = other.alpha / 255.0;
    double out_alpha = alpha1 + alpha2 * (1 - alpha1);
    if (out_alpha == 0)
    {
        return Color(0, 0, 0, 0);
    }
    int out_red = (int)((red * alpha1 + other.red * alpha2 * (1 - alpha1)) / out_alpha);
    int out_green = (int)((green * alpha1 + other.green * alpha2 * (1 - alpha1)) / out_alpha);
    int out_blue = (int)((blue * alpha1 + other.blue * alpha2 * (1 - alpha1)) / out_alpha);
    return Color(out_red, out_green, out_blue, (int)(out_alpha * 255));
}
void Canvas::DeltaTime::update()
{
    frame_end = clock::now();
    valueInSec_ = chrono::duration<float>(frame_end - frame_start).count();
    frame_start = clock::now();
}
float Canvas::DeltaTime::get()
{
    return valueInSec_;
}

Texture::Texture(const char *filename)
{
    if (!(filesystem::exists(filename) && filesystem::is_regular_file(filename) && filesystem::path(filename).extension() == ".png"))
    {
        throw invalid_argument("filename");
    }
    int chb;
    auto img = stbi_load(filename, &size_.width, &size_.height, &chb, 4);
    // colors_.assign(size_.height, vector<Color>(size_.width));
    colors_ = Grid(size_.width, size_.height, Color());
    for (int row = 0; row < size_.height; row++)
    {
        for (int col = 0; col < size_.width; col++)
        {
            vector<int> color(4);
            for (int c = 0; c < 4; c++)
            {
                color[c] = img[(row * size_.width + col) * 4 + c];
            }
            colors_(col, row) = Color(color[0], color[1], color[2], color[3]);
        }
    }
    stbi_image_free(img);
}

Size Texture::size() const
{
    return size_;
}
Color Texture::getPixel(Position position) const
{
    return colors_(position.x, position.y);
}

Color TextureView::getPixel(Position position) const
{
    return tex.getPixel(Position{position.x + pos.x, position.y + pos.y});
}

Canvas::Canvas(int width, int height) : width(width), height(height), colors_(width, height, Color())
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
#endif
    cout << "\033[?25l";
    // colors_.assign(height, vector<Color>(width, Color()));
    text_.assign(height, vector<Text>(width, Text{'\0', {0, 0, 0}}));
    colors_prev_ = colors_;
    refresh(false);
}
float Canvas::getDeltaTime()
{
    return dt.get();
}
void Canvas::setFPS(unsigned int fps)
{
    fps_ = fps;
}
void Canvas::paintPixel(Position position, Color color)
{
    colors_(position.x, position.y) = color.over(colors_(position.x, position.y));
}

void Canvas::refresh(bool optimize)
{
    std::ostringstream output;
    output << "\033[0m";

    for (int row = 0; row < height; row += 2)
    {
        for (int col = 0; col < width; col++)
        {
            if (colors_(col, row) != colors_prev_(col, row) || colors_(col, row + 1) != colors_prev_(col, row + 1) || !optimize)
            {
                output << pos(row / 2 + 1, col + 1) << fg(colors_(col, row)) << bg(colors_(col, row + 1)) << "\u2580";
            }
            if (text_[row][col].char_)
            {
                output << pos(row / 2 + 1, col + 1) << fg(text_[row][col].color) << bg(colors_(col, row + 1)) << text_[row][col].char_;
            }
        }
    }
    output << "\033[0m";
    colors_prev_ = colors_;
    cout << output.str();
    dt.update();
    if (fps_ != -1 && dt.get() < (1 / float(fps_)))
    {
        auto tdt = 1.f / fps_;
        this_thread::sleep_for(chrono::duration<float>(tdt - dt.get()));
    }
}

void Canvas::fill(Color color)
{
    // colors_.assign(height, vector<Color>(width, color));
    colors_ = Grid(width, height, color);
}

void Canvas::fill(Color color, const Rect &rect)
{
    auto position = rect.position;
    Size size = rect.size;
    for (int row = clamp(position.y, 0, height); row < clamp(position.y + size.height, 0, height); row++)
    {
        for (int col = clamp(position.x, 0, width); col < clamp(position.x + size.width, 0, width); col++)
        {
            paintPixel({col, row}, color);
        }
    }
}

void Canvas::blit(const Texture &texture, Position position)
{
    auto size = texture.size();
    for (int row = clamp(position.y, 0, height); row < clamp(position.y + size.height, 0, height); row++)
    {
        for (int col = clamp(position.x, 0, width); col < clamp(position.x + size.width, 0, width); col++)
        {
            paintPixel({col, row}, texture.getPixel({col - position.x, row - position.y}));
        }
    }
}

void Canvas::text(string text, Position position, Color color)
{
    // cout << pos(position.y + 1, position.x + 1) << fg(color) << text;
    int y = (position.y % 2) ? position.y - 1 : position.y;
    for (int i = position.x; i < clamp(position.x + (int)text.size(), 0, width); i++)
    {
        text_[y][i] = Text{text.at(i - position.x), color};
    }
}
