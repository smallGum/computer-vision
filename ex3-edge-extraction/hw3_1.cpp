#include "hw3_1.h"
#include <cmath>
#include <iostream>

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

      if (x0 >= 0 && x0 <= xmax || x1 >= 0 && x1 <= xmax || y0 >= 0 && y0 <= ymax || y1 >= 0 && y1 <= ymax) {
        for (int i = 0; i < peakPoints.size(); ++i) {
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

// calculate the lines' functions
void A4EdgeExtract::calLinesFunctions() {
  for (int i = 0; i < peakPoints.size(); ++i) {
    double angle = (double)peakPoints[i] -> x * PI / 180.0;
    double m = -cos(angle) / sin(angle);
    double b = (double)peakPoints[i] -> y / sin(angle);
    lines.push_back(new line(m, b));
  }
}

// calculate lines' intersections
void A4EdgeExtract::calLinesIntersections() {
  for (int i = 0; i < lines.size(); ++i) {
    for (int j = i + 1; j < lines.size(); ++j) {
      double m0 = lines[i]->m;
      double m1 = lines[j]->m;
      double b0 = lines[i]->b;
      double b1 = lines[j]->b;
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
  for (int i = 0; i < lines.size(); ++i) {
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
  for (int i = 0; i < intersections.size(); ++i) {
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
  for (int i = 0; i < lines.size(); ++i) {
    cout << "y = (" << lines[i] -> m << ")x + " << lines[i] -> b << endl;
  }
}

// print the edge points in the image
void A4EdgeExtract::printIntersections() {
  cout << "edge points: " << endl;
  for (int i = 0; i < intersections.size(); ++i) {
    cout << "(" << intersections[i] -> x << ", " << intersections[i] -> y << ")" << endl;
  }
}
