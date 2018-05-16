#ifndef _FACE_MORPHING_
#define _FACE_MORPHING_

#include "CImg.h"
#include <string>
#include <vector>
#include <fstream>
#include <cmath>

using namespace cimg_library;
using namespace std;

// position structure represents one position in the image
struct position {
  double x, y;
  int num;

  point(double xVal, double yVal, int value): x(xVal), y(yVal), num(value) {}
  bool operator==(const position& another) { return (fabs(x - another.x) < 1e-6) && (fabs(y - another.y) < 1e-6); }
};

struct triangle {
  position a, b, c;

  triangle(position _a, position _b, position _c): a(_a), b(_b), c(_c) {}
};

class controlInfo {
private:
  vector<position*> AcontrolPoints;
  vector<position*> BcontrolPoints;
  vector<triangle*> AcontrolTriangles;
  vector<triangle*> BcontrolTriangles;

  void getTriangles();
  void getTriangleCircle(triangle* tri, position* center, double& radius);
  bool inCircle(position* p, position* center, double radius);

public:
  controlInfo(CImg<double>& imgA, CImg<double>& imgB, string imgAFile, string imgBFile, int controlPointsNum);
};

controlInfo::controlInfo(CImg<double>& imgA, CImg<double>& imgB, string imgAFile, string imgBFile, int controlPointsNum){
  AcontrolPoints.push_back(new position(0, 0, 0));
  AcontrolPoints.push_back(new position(0, imgA.height() - 1, 1));
  AcontrolPoints.push_back(new position(imgA.width() - 1, imgA.height() - 1, 2));
  AcontrolPoints.push_back(new position(imgA.width() - 1, 0, 3));

  BcontrolPoints.push_back(new position(0, 0, 0));
  BcontrolPoints.push_back(new position(0, imgB.height() - 1, 1));
  BcontrolPoints.push_back(new position(imgB.width() - 1, imgB.height() - 1, 2));
  BcontrolPoints.push_back(new position(imgB.width() - 1, 0, 3));

  ifstream inA(imgAFile);
  ifstream inB(imgBFile);
  double Ax, Ay, Bx, By;
  int number = 4;

  for (int i = 0; i < controlPointsNum; ++i) {
    inA >> Ax >> Ay;
    inB >> Bx >> By;
    AcontrolPoints.push_back(new position(Ax, Ay, number));
    AcontrolPoints.push_back(new position(Bx, By, number));
    number++;
  }

  getTriangles();
}

void controlInfo::getTriangles() {
  int numPoints = AcontrolPoints.size();

  for (int i = 0; i < numPoints - 2; ++i) {
    for (int j = i + 1; j < numPoints - 1; ++j) {
      for (int k = j + 1; k < numPoints; ++k) {
        position* a = AcontrolPoints[i];
        position* b = AcontrolPoints[j];
        position* c = AcontrolPoints[k];
        triangle* tri = new triangle(*a, *b, *c);
        position* center;
        double radius;

        getTriangleCircle(tri, center, radius);

        bool find = true;
        for (int m = 0; m < numPoints; ++m) {
          position* p = AcontrolPoints[m];
          if (*p == *a || *p == *b || *p == *c) { continue; }
          if (inCircle(p, center, radius)) { find = false; break; }
        }
        if (find) { AcontrolTriangles.push_back(tri); }
      }
    }
  }

  for (int i = 0; i < AcontrolTriangles.size(); ++i) {
    position* a = BcontrolPoints[(AcontrolTriangles[i] -> a).num];
    position* b = BcontrolPoints[(AcontrolTriangles[i] -> b).num];
    position* c = BcontrolPoints[(AcontrolTriangles[i] -> c).num];
    BcontrolTriangles.push_back(new triangle(*a, *b, *c));
  }
}

void controlInfo::getTriangleCircle(triangle* tri, position* center, double& radius) {
  double x1  =  (tri -> a).x;
  double x2  =  (tri -> b).x;
  double x3  =  (tri -> c).x;
  double y1  =  (tri -> a).y;
  double y2  =  (tri -> b).y;
  double y3  =  (tri -> c).y;

  double a = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
  double b = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
  double c = sqrt((x2 - x3) * (x2 - x3) + (y2 - y3) * (y2 - y3));
  double p = (a + b + c) / 2;
  double S = sqrt(p * (p - a) * (p - b) * (p - c));
  radius = a * b * c / (4 * S);

  double t1 = x1 * x1 + y1 * y1;
  double t2 = x2 * x2 + y2 * y2;
  double t3 = x3 * x3 + y3 * y3;
  double temp = x1 * y2 + x2 * y3 + x3 * y1 - x1 * y3 - x2 * y1 - x3 * y2;
  double x = (t2 * y3 + t1 * y2 + t3 * y1 - t2 * y1 - t3 * y2 - t1 * y3) / temp / 2;
  double y = (t3 * x2 + t2 * x1 + t1 * x3 - t1 * x2 - t2 * x3 - t3 * x1) / temp / 2;

  center = new position(x, y, 0);
}

bool controlInfo::inCircle(position* p, position* center, double radius) {
  double x2 = (p -> x - center -> x) * (p -> x - center -> x);
  double y2 = (p -> y - center -> y) * (p -> y - center -> y);
  return sqrt(x2 + y2) <= radius;
}

#endif
