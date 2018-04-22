#ifndef _HW3_1_
#define _HW3_1_

#include <string>
#include "CImg.h"

using namespace cimg_library;

#define PI 3.14159265358979323846

// images IO class
// read the source image and output its grey image
class imageIO {
private:
  CImg<double> srcImg;   // source image read from file
  CImg<double> greyImg;  // grey image of the source image

  // transform the source image to grey image
  void toGrey();

public:
  // constructor, read the source image from the file
  imageIO(const char* filename);
  // get the source image
  CImg<double> getSrcImg();
  // get the grey image
  CImg<double> getGreyImg();
};

// A4 edge extract class
// extract the edge of the gray image of an A4 paper
class A4EdgeExtract {
private:
  int blurDegree;        // degree of blur
  int gradLimit;         // threshold of gradiant

  CImg<double> grayImg;  // gray image to extract edges
  CImg<double> edgeImg;  // edge image of the gray image
  CImg<double> houghImg; // hough image of the gray image

  // use gaussian filter to blur image
  void gaussianFilter();
  // calculate the distance between the point and the origin
  double calDistance(double x, double y);
  // calculate edge and hough image
  void calEdgeAndHoughImg();

public:
  // constructor
  A4EdgeExtract(CImg<double> gray, int blur, int grad);
  // get the edge image
  CImg<double> getEdgeImg();
  // get the hough image
  CImg<double> getHoughImg();
};

#endif
