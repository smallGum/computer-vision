#include "CImg.h"
using namespace cimg_library;
int main()
{
    // read the image
    CImg<unsigned char> img("1.bmp");
    // show the image
    img.display("My first CImg image");

    return 0;
}