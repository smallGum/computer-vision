#include "hw3_1.h"

int main() {
  imageIO img("A4.bmp");
  CImg<double> gray = img.getGreyImg();
  A4EdgeExtract edgeEct(gray, 3, 25);
  CImg<double> edge = edgeEct.getEdgeImg();
  CImg<double> hough = edgeEct.getHoughImg();

  edge.display();
  hough.display();

  return 0;
}
