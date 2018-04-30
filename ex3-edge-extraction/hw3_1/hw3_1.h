#ifndef _HW3_1_
#define _HW3_1_

#include "CImg.h"
#include <cmath>
#include <vector>
#include <queue>
#include <iostream>

using namespace cimg_library;
using namespace std;

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

class A4EdgeExtract {
private:
  unsigned int width;
  unsigned int height;
  double houghThreshold;
  double houghDiff;             // difference between adjacnet points in the hough image
  vector<point*> peakPoints;    // store the peak pixel points in the hough image
  vector<line*> lines;          // store the lines in the image
  vector<point*> intersections; // store lines' intersections in the image

  CImg<double> edgeImg;
  CImg<double> houghImg;
  CImg<double> resultImg;  // source image marked the corner point

  // polar coordinate intersection with x value
  int crossX(int theta, int dist, int x);
  // polar coordinate intersection with y value
  int crossY(int theta, int dist, int y);
  unsigned int calDist(unsigned int x, unsigned int y) { return sqrt(x * x + y * y); }
  double calDist(double x, double y) { return sqrt(x * x + y * y); }

  void calHoughImg();
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
  A4EdgeExtract(CImg<double>& src, CImg<double>& edge, double diff = 200, double threshold = 650);
  CImg<double> getHoughImg() { return houghImg; }
  // get the result image
  CImg<double> getResultImg() { return resultImg; }

  // print the edge lines in the image
  void printLines();
  // print the edge points in the image
  void printIntersections();
};

// polar coordinate intersection with x value
int A4EdgeExtract::crossX(int theta, int dist, int x) {
  double angle = (double)theta * PI / 180.0;
  double m = -cos(angle) / sin(angle);
  double b = (double)dist / sin(angle);
  return m * x + b;
}

// polar coordinate intersection with y value
int A4EdgeExtract::crossY(int theta, int dist, int y) {
  double angle = (double)theta * PI / 180.0;
  double m = -cos(angle) / sin(angle);
  double b = (double)dist / sin(angle);
  return ((double)(y - b) / m);
}

void A4EdgeExtract::calHoughImg() {
  cimg_forXY(edgeImg, x, y) {
    if (edgeImg(x, y)) {
      cimg_forX(houghImg, angle) {
        double theta = (double)angle * PI / 180.0;
        int polar = (int)round((double)x * cos(theta) + (double)y * sin(theta));
        if (polar >= 0 && polar < houghImg.height()) { houghImg(angle, polar) += 1; }
      }
    }
  }
}

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
          if (calDist(peakPoints[i] -> x - angle, peakPoints[i] -> y - polar) < houghDiff) {
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

    const double color[] = { 255, 0, 255 };

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

A4EdgeExtract::A4EdgeExtract(CImg<double>& src, CImg<double>& edge, double diff, double threshold) {
  width = edge.width();
  height = edge.height();
  houghDiff = diff;
  houghThreshold = threshold;

  edgeImg = CImg<double>(edge);
  houghImg = CImg<double>(360, calDist(width, height), 1, 1, 0);
  resultImg = CImg<double>(src);

  calHoughImg();
  findPeakPoints();
  calLinesFunctions();
  calLinesIntersections();
  drawEdges();
  drawEdgePoints();
}

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
