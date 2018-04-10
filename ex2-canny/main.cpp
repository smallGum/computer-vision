#include "canny.h"
#include <iostream>

using namespace std;

int main() {
  imageIO img("images/lena.bmp");
  CImg<unsigned char> grey = img.getGreyImg();
  canny edge(grey);
  CImg<unsigned char> rst = edge.getEdgeImg();
  rst.display();

  return 0;
}
