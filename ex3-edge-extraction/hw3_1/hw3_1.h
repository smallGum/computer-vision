#ifndef _HW3_1_
#define _HW3_1_

#include <string>
#include "CImg.h"
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

using namespace cimg_library;

#define PI 3.14159265358979323846

// point structure represents one pixel in the image
struct point {
  double x, y, val;
  point(double xVal, double yVal, double value): x(xVal), y(yVal), val(value) {}
};

// line structure represents a line in the image
struct line {
  double m, b;
  line(double mVal, double bVal): m(mVal), b(bVal) {}
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
  int blurDegree;               // degree of blur
  double gradLimit;             // threshold of gradiant
  double houghDiff;             // difference between adjacnet points in the hough image
  double houghThreshold;        // threshold for hough peak value
  vector<point*> peakPoints;    // store the peak pixel points in the hough image
  vector<line*> lines;          // store the lines in the image
  vector<point*> intersections; // store lines' intersections in the image

  CImg<double> grayImg;    // gray image to extract edges
  CImg<double> edgeImg;    // edge image of the gray image
  CImg<double> houghImg;   // hough image of the gray image
  CImg<double> resultImg;  // source image marked the corner point

  // use gaussian filter to blur image
  void gaussianFilter();
  // calculate the distance between the point and the origin
  double calDistance(double x, double y);
  // calculate edge and hough image
  void calEdgeAndHoughImg();
  // polar coordinate intersection with x value
  const int crossX(int theta, int dist, int x);
  // polar coordinate intersection with y value
  const int crossY(int theta, int dist, int y);
  // find peak points of hough image
  void findPeakPoints();
  // calculate the edge lines' functions
  void calLinesFunctions();
  // calculate edge lines' intersections
  void calLinesIntersections();
  // draw the edges of the image
  void drawEdges();
  // draw the edge points of the image
  void drawEdgePoints();


public:
  // constructor
  A4EdgeExtract(CImg<double>& src, CImg<double>& gray, int blur = 3, double grad = 20, double diff = 200, double threshold = 650);
  // get the edge image
  CImg<double> getEdgeImg();
  // get the hough image
  CImg<double> getHoughImg();
  // get the blur image
  CImg<double> getBlurImg();
  // get the result image
  CImg<double> getResultImg();
  // print the edge lines in the image
  void printLines();
  // print the edge points in the image
  void printIntersections();
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

// ----------------------A4EdgeExtract-----------------------------
// constructor
A4EdgeExtract::A4EdgeExtract(CImg<double>& src, CImg<double>& gray, int blur, double grad, double diff, double threshold) {
  blurDegree = blur;
  gradLimit = grad;
  houghDiff = diff;
  houghThreshold = threshold;
  resultImg = CImg<double>(src);
  grayImg = CImg<double>(gray);
  edgeImg = CImg<double>(grayImg.width(), grayImg.height(), 1, 1, 0);
  int maxDist = calDistance(grayImg.width(), grayImg.height());
  houghImg = CImg<double>(360, maxDist, 1, 1, 0);

  gaussianFilter();
  calEdgeAndHoughImg();
  findPeakPoints();
  calLinesFunctions();
  calLinesIntersections();
  drawEdges();
  drawEdgePoints();
}

// use gaussian filter to blur image
void A4EdgeExtract::gaussianFilter() { grayImg.blur(blurDegree); }

// calculate the distance between the point and the origin
// param: x the x value of the point
// param: y the y value of the point
double A4EdgeExtract::calDistance(double x, double y) { return sqrt(x * x + y * y); }

// polar coordinate intersection with x value
const int A4EdgeExtract::crossX(int theta, int dist, int x) {
  double angle = (double)theta * PI / 180.0;
  double m = -cos(angle) / sin(angle);
  double b = (double)dist / sin(angle);
  return m * x + b;
}

// polar coordinate intersection with y value
const int A4EdgeExtract::crossY(int theta, int dist, int y) {
  double angle = (double)theta * PI / 180.0;
  double m = -cos(angle) / sin(angle);
  double b = (double)dist / sin(angle);
  return ((double)(y - b) / m);
}

// find peak points of hough image
void A4EdgeExtract::findPeakPoints() {
  cimg_forXY(houghImg, angle, polar) {
    if (houghImg(angle, polar) > houghThreshold) {
      bool flag = false;
      const int ymin = 0;
      const int ymax = resultImg.height() - 1;
      const int x0 = crossY(angle, polar, ymin);
      const int x1 = crossY(angle, polar, ymax);

      const int xmin = 0;
      const int xmax = resultImg.width() - 1;
      const int y0 = crossX(angle, polar, xmin);
      const int y1 = crossX(angle, polar, xmax);

      if ((x0 >= 0 && x0 <= xmax) || (x1 >= 0 && x1 <= xmax) || (y0 >= 0 && y0 <= ymax) || (y1 >= 0 && y1 <= ymax)) {
        for (unsigned int i = 0; i < peakPoints.size(); ++i) {
          if (calDistance(peakPoints[i] -> x - angle, peakPoints[i] -> y - polar) < houghDiff) {
            flag = true;
            if (peakPoints[i] -> val < houghImg(angle, polar)) {
              peakPoints[i] = new point(angle, polar, houghImg(angle, polar));
            }
          }
        }
        if (!flag) { peakPoints.push_back(new point(angle, polar, houghImg(angle, polar))); }
      }
    }
  }
}

// calculate edge and hough image
void A4EdgeExtract::calEdgeAndHoughImg() {
  CImg_3x3(I, double);
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

// calculate the lines' functions
void A4EdgeExtract::calLinesFunctions() {
  for (unsigned int i = 0; i < peakPoints.size(); ++i) {
    double angle = (double)peakPoints[i] -> x * PI / 180.0;
    double m = -cos(angle) / sin(angle);
    double b = (double)peakPoints[i] -> y / sin(angle);
    lines.push_back(new line(m, b));
  }
}

// calculate lines' intersections
void A4EdgeExtract::calLinesIntersections() {
  for (unsigned int i = 0; i < lines.size(); ++i) {
    for (unsigned int j = i + 1; j < lines.size(); ++j) {
      double m0 = lines[i] -> m;
      double m1 = lines[j] -> m;
      double b0 = lines[i] -> b;
      double b1 = lines[j] -> b;
      double x = (b1 - b0) / (m0 - m1);
      double y = (m0 * b1 - m1 * b0) / (m0 - m1);
      if (x >= 0 && x < resultImg.width() && y >= 0 && y < resultImg.height()) {
        intersections.push_back(new point(x, y, 0));
      }
    }
  }
}

// draw the edges of the image
void A4EdgeExtract::drawEdges() {
  for (unsigned int i = 0; i < lines.size(); ++i) {
    const int ymin = 0;
    const int ymax = resultImg.height() - 1;
    const int x0 = (double)(ymin - lines[i] -> b) / lines[i] -> m;
    const int x1 = (double)(ymax - lines[i] -> b) / lines[i] -> m;

    const int xmin = 0;
    const int xmax = resultImg.width() - 1;
    const int y0 = xmin * lines[i] -> m + lines[i] -> b;
    const int y1 = xmax * lines[i] -> m + lines[i] -> b;

    const double color[] = { 255, 255, 0 };

    if (abs(lines[i] -> m) > 1) { resultImg.draw_line(x0, ymin, x1, ymax, color); }
    else { resultImg.draw_line(xmin, y0, xmax, y1, color); }
  }
}
// draw the edge points of the image
void A4EdgeExtract::drawEdgePoints() {
  for (unsigned int i = 0; i < intersections.size(); ++i) {
    const double color[] = { 255, 255, 0 };
    resultImg.draw_circle(intersections[i] -> x, intersections[i] -> y, 50, color);
  }
}

// get the edge image
CImg<double> A4EdgeExtract::getEdgeImg() { return edgeImg; }

// get the hough image
CImg<double> A4EdgeExtract::getHoughImg() { return houghImg; }

// get the blurred image
CImg<double> A4EdgeExtract::getBlurImg() { return grayImg; }

// get the result image
CImg<double> A4EdgeExtract::getResultImg() { return resultImg; }

// print the edge lines in the image
void A4EdgeExtract::printLines() {
  cout << "edge lines\' function:" << endl;
  for (unsigned int i = 0; i < lines.size(); ++i) {
    cout << "y = (" << lines[i] -> m << ")x + " << lines[i] -> b << endl;
  }
}

// print the edge points in the image
void A4EdgeExtract::printIntersections() {
  cout << "edge points: " << endl;
  for (unsigned int i = 0; i < intersections.size(); ++i) {
    cout << "(" << intersections[i] -> x << ", " << intersections[i] -> y << ")" << endl;
  }
}

#endif
