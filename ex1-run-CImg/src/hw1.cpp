#include "../header/hw1.h"

// display the image
// return true if success
bool task1(const CImg<unsigned char>& img) {
  if (img.display()) { return true; }
  return false;
}

// change the white (255, 255, 255) area of the image to red (255, 0, 0),
// and the black (0, 0, 0) area to green (0, 255, 0)
// return true if success
bool task2(CImg<unsigned char> img) {
    // traverse all the pixels
    for (int i = 0; i < img.width(); ++i) {
        for (int j = 0; j < img.height(); ++j) {
            // change white pixel to red pixel
            if ((int)img(i, j, 0, 0) == 255 && (int)img(i, j, 0, 1) == 255 && (int)img(i, j, 0, 2) == 255) {
                img(i, j, 0, 1) = img(i, j, 0, 2) = 0;
            }
            // change black pixel to green pixel
            if ((int)img(i, j, 0, 0) == 0 && (int)img(i, j, 0, 1) == 0 && (int)img(i, j, 0, 2) == 0) {
                img(i, j, 0, 1) = 255;
            }
        }
    }

    // display the changed image
    if (img.display()) { return true; }
    return false;
}

// paint a blue circle on (50, 50) with radius == 30
// return true if success
bool task3(CImg<unsigned char> img) {
    unsigned char blue[] = { 0, 0, 255 };
    if (img.draw_circle(50, 50, 30, blue)) {
      img.display();
      return true;
    }

    return false;
}

// paint a yellow circle on (50, 50) with radius == 3
// return true if success
bool task4(CImg<unsigned char> img) {
    unsigned char yellow[] = { 255, 255, 0 };
    if (img.draw_circle(50, 50, 3, yellow)) {
      img.display();
      return true;
    }

    return false;
}
