#include "hw3_1.h"
#include <cmath>

// ----------------------imageIO class-----------------------------
// transform the source image into grey image
void imageIO::toGrey() {
  unsigned int width = srcImg.width(), height = srcImg.height();
  greyImg = CImg<double>(width, height, 1, 1);

  for (int i = 0; i < width; ++i) {
      for (int j = 0; j < height; ++j) {
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

// ----------------------A4EdgeExtract-----------------------------
// constructor
A4EdgeExtract::A4EdgeExtract(CImg<double>& gray, int blur, double grad, double diff, double threshold) {
  blurDegree = blur;
  gradLimit = grad;
  houghDiff = diff;
  houghThreshold = threshold;
  grayImg = CImg<double>(gray);
  edgeImg = CImg<double>(grayImg.width(), grayImg.height(), 1, 1, 0);
  int maxDist = calDistance(grayImg.width(), grayImg.height());
  houghImg = CImg<double>(360, maxDist, 1, 1, 0);

  gaussianFilter();
  calEdgeAndHoughImg();
}

// use gaussian filter to blur image
void A4EdgeExtract::gaussianFilter() { grayImg.blur(blurDegree); }

// calculate the distance between the point and the origin
// param: x the x value of the point
// param: y the y value of the point
double A4EdgeExtract::calDistance(double x, double y) { return sqrt(x * x + y * y); }

// calculate edge and hough image
void A4EdgeExtract::calEdgeAndHoughImg() {
  CImg_3x3(I, double);
  int cnt = 0;
  cimg_for3x3(grayImg, x, y, 0, 0, I, double) {
    const double gradX = Inc - Ipc;
    const double gradY = Icp - Icn;
    double gradVal = sqrt(gradX * gradX + gradY * gradY);

    if (gradVal > gradLimit) {
      edgeImg(x, y) = gradVal;
      cimg_forX(houghImg, angle) {
        double rangle = (double)angle * PI / 180.0;
        int polar = (int)(x * cos(rangle) + y * sin(rangle));
        if (polar >= 0 && polar < houghImg.height()) { houghImg(angle, polar) += 1; }
      }
    }
  }
}

// get the edge image
CImg<double> A4EdgeExtract::getEdgeImg() { return edgeImg; }

// get the hough image
CImg<double> A4EdgeExtract::getHoughImg() { return houghImg; }

// get the blurred image
CImg<double> A4EdgeExtract::getBlurImg() { return grayImg; }
