#include "CImg.h"

using namespace cimg_library;

// display the image
bool task1(const CImg<unsigned char>& img);

// change the white (255, 255, 255) area of the image to red (255, 0, 0),
// and the black (0, 0, 0) area to green (0, 255, 0)
bool task2(CImg<unsigned char> img);

// paint a blue circle on (50, 50) with radius == 30
bool task3(CImg<unsigned char> img);

// paint a yellow circle on (50, 50) with radius == 3
bool task4(CImg<unsigned char> img);
