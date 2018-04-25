#include "hw3_2.h"

int main() {
  // read the source image
  imageIO img("6.bmp");
  // get the grey image
  CImg<unsigned char> grey = img.getGreyImg();
  CImg<unsigned char> src = img.getSrcImg();

  circleExtract circle(grey, grey.height() / 10, 7.5, 50, 0, 0);
  vector<pixel> rst = circle.getCircles();

  unsigned char yellow[] = { 255, 255, 0 };
  unsigned char blue[] = {0, 255, 255};
  for (int i = 0; i< rst.size(); ++i) {
    src.draw_circle(rst[i].pt.x, rst[i].pt.y, 20, blue);
    src.draw_circle(rst[i].pt.x, rst[i].pt.y, rst[i].val, yellow, 1, ~0U);
  }
  src.display();

  /*
  // canny detector
  canny edge(grey);
  // get edge image
  CImg<unsigned char> rst = edge.getEdgeImg();
  CImg<double> res(rst);
  res.display();

  CImg<double> x = edge.getXGradImg();
  x.display();

  CImg<double> y = edge.getYGradImg();
  y.display();
  */
  return 0;
}
