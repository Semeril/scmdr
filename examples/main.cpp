#include <scmdr/scmdr.hpp>

using namespace std;

int main()
{
    Texture t("example.png");
    Canvas canvas(13, 14);

    canvas.blit(t, {0, 0});
    canvas.refresh();
};