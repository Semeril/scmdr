#include <scmdr/scmdr.hpp>

using namespace std;

int main()
{
    Texture t("example.png");
    Canvas canvas;

    canvas.blit(t, {0, 0});
    canvas.refresh();
};