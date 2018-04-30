#ifndef _PREWITT_
#define _PREWITT_

#include "CImg.h"
#include <cmath>

using namespace cimg_library;

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

class prewitt {
private:
  CImg<double> grayImg;
  CImg<double> edgeImg;

  double gradLimit;
  int blurDegree;

  void calEdgeImg();
  void gaussianFilter() { grayImg.blur(blurDegree); }

public:
  prewitt(CImg<double>& gray, int blur, double grad);
  CImg<double> getEdgeImg() { return edgeImg; }
};

// ----------------------imageIO class-----------------------------
// transform the source image into grey image
void imageIO::toGrey() {
  unsigned int width = srcImg.width(), height = srcImg.height();
  greyImg = CImg<double>(width, height, 1, 1);

  for (unsigned int i = 0; i < width; ++i) {
      for (unsigned int j = 0; j < height; ++j) {
          double b = srcImg(i, j, 0, 0);
          double g = srcImg(i, j, 0, 1);
          double r = srcImg(i, j, 0, 2);

          double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
          greyImg(i, j, 0, 0) = newValue;
      }
  }
}

// constructor read the source image from the file
// param: filename name of the file
imageIO::imageIO(const char* filename) {
  srcImg = CImg<double>(filename);
  toGrey();
}

// get the source image
// return: CImg<unsigned char> the source image
CImg<double> imageIO::getSrcImg() { return srcImg; }

// get the grey image
// return: CImg<unsigned char> the grey image
CImg<double> imageIO::getGreyImg() { return greyImg; }
// ----------------------imageIO class-----------------------------

void prewitt::calEdgeImg() {
  CImg_3x3(I, double);
  cimg_for3x3(grayImg, x, y, 0, 0, I, double) {
    const double gradX = Inc - Ipc;
    const double gradY = Icp - Icn;
    double gradVal = sqrt(gradX * gradX + gradY * gradY);

    if (gradVal > gradLimit) { edgeImg(x, y) = gradVal; }
  }
}

prewitt::prewitt(CImg<double>& gray, int blur, double grad) {
  grayImg = CImg<double>(gray);
  edgeImg = CImg<double>(gray.width(), gray.height(), 1, 1, 0);
  blurDegree = blur;
  gradLimit = grad;

  gaussianFilter();
  calEdgeImg();
}

#endif
