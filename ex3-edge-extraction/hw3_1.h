#ifndef _HW3_1_
#define _HW3_1_

#include <string>
#include "CImg.h"
#include <vector>

using namespace std;

using namespace cimg_library;

#define PI 3.14159265358979323846

// point structure represents one pixel in the image
struct point {
  int x, y, val;
  point(int xVal, int yVal, int value): x(xVal), y(yVal), val(value) {}
};

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
  int blurDegree;             // degree of blur
  double gradLimit;           // threshold of gradiant
  double houghDiff;           // difference between adjacnet points in the hough image
  double houghThreshold;      // threshold for hough peak value
  vector<point*> peakPoints;  // store the peak pixel points in the hough image

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
  A4EdgeExtract(CImg<double>& gray, int blur = 2, double grad = 20, double diff = 200, double threshold = 650);
  // get the edge image
  CImg<double> getEdgeImg();
  // get the hough image
  CImg<double> getHoughImg();
  // get the blur image
  CImg<double> getBlurImg();
};

#endif
