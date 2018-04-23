#include "hw3_1.h"

int main() {
  imageIO img("./images/A4_6.bmp");
  CImg<double> src = img.getSrcImg();
  CImg<double> gray = img.getGreyImg();
  A4EdgeExtract edgeEct(src, gray);
  CImg<double> edge = edgeEct.getEdgeImg();
  CImg<double> hough = edgeEct.getHoughImg();
  CImg<double> result = edgeEct.getResultImg();

  //edge.display();
  //hough.display();
  edgeEct.printLines();
  edgeEct.printIntersections();
  result.display();

  return 0;
}
