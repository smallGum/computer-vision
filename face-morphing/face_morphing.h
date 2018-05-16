#ifndef _FACE_MORPHING_
#define _FACE_MORPHING_

#include "CImg.h"
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <iostream>
#include <cstdlib>

using namespace cimg_library;
using namespace std;

// position structure represents one position in the image
struct position {
  double x, y;
  int num;

  position(double xVal, double yVal, int value): x(xVal), y(yVal), num(value) {}
  position(const position& p): x(p.x), y(p.y), num(p.num) {}
  bool operator==(const position& another) { return (fabs(x - another.x) < 1e-6) && (fabs(y - another.y) < 1e-6); }
  position operator-(const position& another) const { return position(x - another.x, y - another.y, 0); }
  double dot(const position& another) const { return x * another.x + y * another.y; }
};

struct triangle {
  position a, b, c;
  position center;
  double radius;

  triangle(position _a, position _b, position _c): a(_a), b(_b), c(_c), center(position(0, 0, 0)) {
    double x1  =  a.x;
    double x2  =  b.x;
    double x3  =  c.x;
    double y1  =  a.y;
    double y2  =  b.y;
    double y3  =  c.y;

    double A = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    double B = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
    double C = sqrt((x2 - x3) * (x2 - x3) + (y2 - y3) * (y2 - y3));
    double p = (A + B + C) / 2;
    double S = sqrt(p * (p - A) * (p - B) * (p - C));
    radius = A * B * C / (4 * S);

    double t1 = x1 * x1 + y1 * y1;
    double t2 = x2 * x2 + y2 * y2;
    double t3 = x3 * x3 + y3 * y3;
    double temp = x1 * y2 + x2 * y3 + x3 * y1 - x1 * y3 - x2 * y1 - x3 * y2;
    double x = (t2 * y3 + t1 * y2 + t3 * y1 - t2 * y1 - t3 * y2 - t1 * y3) / temp / 2;
    double y = (t3 * x2 + t2 * x1 + t1 * x3 - t1 * x2 - t2 * x3 - t3 * x1) / temp / 2;

    center.x = x;
    center.y = y;
  }

  bool inCircle(position* p) {
    double x2 = (p -> x - center.x) * (p -> x - center.x);
    double y2 = (p -> y - center.y) * (p -> y - center.y);
    return sqrt(x2 + y2) <= radius;
  }

  bool inTriangle(position* p) {
    position v0 = c - a;
    position v1 = b - a;
    position v2 = *p - a;

    double dot00 = v0.dot(v0);
    double dot01 = v0.dot(v1);
    double dot02 = v0.dot(v2);
    double dot11 = v1.dot(v1);
    double dot12 = v1.dot(v2);

    double inverDeno = 1.0 / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
    double v = (dot00 * dot12 - dot01 * dot02) * inverDeno;

    if (u < 0 || u > 1) { return false; }
    if (v < 0 || v > 1) { return false; }
    return u + v <= 1;
  }
};

struct squareMatrix3 {
  double a11, a21, a31;
  double a12, a22, a32;
  double a13, a23, a33;
};

class controlInfo {
private:
  CImg<double> srcImgA;
  CImg<double> srcImgB;
  vector<position*> AcontrolPoints;
  vector<position*> BcontrolPoints;
  vector<triangle*> AcontrolTriangles;
  vector<triangle*> BcontrolTriangles;

  void getTriangles();

public:
  controlInfo(string imgA, string imgB, string imgAFile, string imgBFile, int controlPointsNum);
  void drawTriangle(CImg<double>& imgA, CImg<double>& imgB);
  CImg<double>& getSrcImgA() { return srcImgA; }
  CImg<double>& getSrcImgB() { return srcImgB; }
  vector<triangle*>& getAControlTriangles() { return AcontrolTriangles; }
  vector<triangle*>& getBControlTriangles() { return BcontrolTriangles; }
};

controlInfo::controlInfo(string imgA, string imgB, string imgAFile, string imgBFile, int controlPointsNum){
  srcImgA = CImg<double>(imgA.c_str());
  srcImgB = CImg<double>(imgB.c_str());

  AcontrolPoints.push_back(new position(0, 0, 0));
  AcontrolPoints.push_back(new position(0, srcImgA.height() - 1, 1));
  AcontrolPoints.push_back(new position(srcImgA.width() - 1, srcImgA.height() - 1, 2));
  AcontrolPoints.push_back(new position(srcImgA.width() - 1, 0, 3));

  BcontrolPoints.push_back(new position(0, 0, 0));
  BcontrolPoints.push_back(new position(0, srcImgB.height() - 1, 1));
  BcontrolPoints.push_back(new position(srcImgB.width() - 1, srcImgB.height() - 1, 2));
  BcontrolPoints.push_back(new position(srcImgB.width() - 1, 0, 3));

  ifstream inA(imgAFile.c_str());
  ifstream inB(imgBFile.c_str());
  double Ax, Ay, Bx, By;
  int number = 4;

  for (int i = 0; i < controlPointsNum; ++i) {
    inA >> Ax >> Ay;
    inB >> Bx >> By;
    AcontrolPoints.push_back(new position(Ax, Ay, number));
    BcontrolPoints.push_back(new position(Bx, By, number));
    number++;
  }
  inA.close();
  inB.close();

  getTriangles();
  drawTriangle(srcImgA, srcImgB);
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

        bool find = true;
        for (int m = 0; m < numPoints; ++m) {
          position* p = AcontrolPoints[m];
          if (*p == *a || *p == *b || *p == *c) { continue; }
          if (tri -> inCircle(p)) { find = false; break; }
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

void controlInfo::drawTriangle(CImg<double>& imgA, CImg<double>& imgB) {
  const double color[] = { 0, 255, 0 };
  CImgDisplay TAdisp(imgA), TBdisp(imgB);
  int i = 0;
  while (!TAdisp.is_closed()) {
    TAdisp.wait();
    if (TAdisp.button() && i < AcontrolTriangles.size()) {
      imgA.draw_line(AcontrolTriangles[i] -> a.x, AcontrolTriangles[i] -> a.y, AcontrolTriangles[i] -> b.x, AcontrolTriangles[i] -> b.y, color);
      imgA.draw_line(AcontrolTriangles[i] -> a.x, AcontrolTriangles[i] -> a.y, AcontrolTriangles[i] -> c.x, AcontrolTriangles[i] -> c.y, color);
      imgA.draw_line(AcontrolTriangles[i] -> b.x, AcontrolTriangles[i] -> b.y, AcontrolTriangles[i] -> c.x, AcontrolTriangles[i] -> c.y, color);
      imgA.display(TAdisp);

      imgB.draw_line(BcontrolTriangles[i] -> a.x, BcontrolTriangles[i] -> a.y, BcontrolTriangles[i] -> b.x, BcontrolTriangles[i] -> b.y, color);
      imgB.draw_line(BcontrolTriangles[i] -> a.x, BcontrolTriangles[i] -> a.y, BcontrolTriangles[i] -> c.x, BcontrolTriangles[i] -> c.y, color);
      imgB.draw_line(BcontrolTriangles[i] -> b.x, BcontrolTriangles[i] -> b.y, BcontrolTriangles[i] -> c.x, BcontrolTriangles[i] -> c.y, color);
      imgB.display(TBdisp);

      ++i;
    }
  }
}


class morph {
private:
  CImg<double> srcImgA;
  CImg<double> srcImgB;
  CImg<double> targetImgA;
  CImg<double> targetImgB;
  CImg<double> targetImg;
  double rate;

  triangle* calMiddleTriangle(triangle* A, triangle* B);
  squareMatrix3 calTransCoeff(triangle* A, triangle* B);
  void getMiddleTriangles(vector<triangle*>& ATris, vector<triangle*>& BTris, vector<triangle*>& MTris);
  void trans(vector<triangle*>& ATris, vector<triangle*>& BTris, vector<triangle*>& MTris);

public:
  morph(controlInfo& ci, double _rate);
};

morph::morph(controlInfo& ci, double _rate) {
  srcImgA = ci.getSrcImgA();
  targetImgA = CImg<double>(srcImgA);
  srcImgB = ci.getSrcImgB();
  targetImgB = CImg<double>(srcImgB);
  int width = max(srcImgA.width(), srcImgB.width());
  int height = max(srcImgA.height(), srcImgB.height());
  targetImg = CImg<double>(width, height, 1, 3, 0);
  rate = _rate;

  vector<triangle*> MTris;
  getMiddleTriangles(ci.getAControlTriangles(), ci.getBControlTriangles(), MTris);
  trans(ci.getAControlTriangles(), ci.getBControlTriangles(), MTris);


  targetImg.display();
}

triangle* morph::calMiddleTriangle(triangle* A, triangle* B) {
  double ax = rate * (A -> a.x) + (1 - rate) * (B -> a.x);
	double ay = rate * (A -> a.y) + (1 - rate) * (B -> a.y);
	double bx = rate * (A -> b.x) + (1 - rate) * (B -> b.x);
	double by = rate * (A -> b.y) + (1 - rate) * (B -> b.y);
	double cx = rate * (A -> c.x) + (1 - rate) * (B -> c.x);
	double cy = rate * (A -> c.y) + (1 - rate) * (B -> c.y);

	return new triangle(position(ax, ay, 0), position(bx, by, 0), position(cx, cy, 0));
}

squareMatrix3 morph::calTransCoeff(triangle* A, triangle* B) {
  double u0 = A -> a.x, v0 = A -> a.y;
  double u1 = A -> b.x, v1 = A -> b.y;
  double u2 = A -> c.x, v2 = A -> c.y;
  double x0 = B -> a.x, y0 = B -> a.y;
  double x1 = B -> b.x, y1 = B -> b.y;
  double x2 = B -> c.x, y2 = B -> c.y;

  int detA = u0 * v1 + u1 * v2 + u2 * v0 - u2 * v1 - u0 * v2 - u1 * v0;
  int A11, A12, A13, A21, A22, A23, A31, A32, A33;
  A11 = v1 - v2;
  A21 = -(v0 - v2);
  A31 = v0 - v1;
  A12 = -(u1 - u2);
  A22 = u0 - u2;
  A32 = -(u0 - u1);
  A13 = u1 * v2 - u2 * v1;
  A23 = -(u0 * v2 - u2 * v0);
  A33 = u0 * v1 - u1 * v0;

  squareMatrix3 result;
  result.a11 = (double)(x0 * A11 + x1 * A21 + x2 * A31) / detA;
	result.a21 = (double)(y0 * A11 + y1 * A21 + y2 * A31) / detA;
	result.a31 = (double)(A11 + A21 + A31) / detA;
	result.a12 = (double)(x0 * A12 + x1 * A22 + x2 * A32) / detA;
	result.a22 = (double)(y0 * A12 + y1 * A22 + y2 * A32) / detA;
	result.a32 = (double)(A12 + A22 + A32) / detA;
	result.a13 = (double)(x0 * A13 + x1 * A23 + x2 * A33) / detA;
	result.a23 = (double)(y0 * A13 + y1 * A23 + y2 * A33) / detA;
	result.a33 = (double)(A13 + A23 + A33) / detA;

  return result;
}

void morph::getMiddleTriangles(vector<triangle*>& ATris, vector<triangle*>& BTris, vector<triangle*>& MTris) {
  for (int i = 0; i < ATris.size(); ++i) {
    triangle* newTri = calMiddleTriangle(ATris[i], BTris[i]);
    MTris.push_back(newTri);
  }
}

void morph::trans(vector<triangle*>& ATris, vector<triangle*>& BTris, vector<triangle*>& MTris) {
  vector<squareMatrix3*> AtoB, BtoA;

  for (int i = 0; i < ATris.size(); ++i) {
    squareMatrix3 HAM = calTransCoeff(ATris[i], MTris[i]);
    AtoB.push_back(new squareMatrix3(HAM));
    squareMatrix3 HBM = calTransCoeff(BTris[i], MTris[i]);
    BtoA.push_back(new squareMatrix3(HBM));
  }

  int width = targetImgA.width();
  int height = targetImgA.height();
  cimg_forXY(targetImgA, x, y) {
    bool isFind = false;
    for (int i = 0; i < BTris.size(); ++i) {
      if (BTris[i] -> inTriangle(new position(x, y, 0))) {
        double tx = x * BtoA[i] -> a11 + y * BtoA[i] -> a12 + BtoA[i] -> a13;
        double ty = x * BtoA[i] -> a21 + y * BtoA[i] -> a22 + BtoA[i] -> a23;
        if (tx >= 0 && tx < width && ty >= 0 && ty < height) {
          cimg_forC(srcImgA, c) { targetImgA(x, y, 0, c) = srcImgA.linear_atXY(tx, ty, 0, c); }
        }
        isFind = true;
        break;
      }
    }
    if (!isFind) {
      cimg_forC(srcImgA, c) { targetImgA(x, y, 0, c) = srcImgA.linear_atXY(x, y, 0, c); }
    }
  }
  targetImgA.display();

  width = targetImgB.width();
  height = targetImgB.height();
  cimg_forXY(targetImgB, x, y) {
    bool isFind = false;
    for (int i = 0; i < ATris.size(); ++i) {
      if (ATris[i] -> inTriangle(new position(x, y, 0))) {
        double tx = x * AtoB[i] -> a11 + y * AtoB[i] -> a12 + AtoB[i] -> a13;
        double ty = x * AtoB[i] -> a21 + y * AtoB[i] -> a22 + AtoB[i] -> a23;
        if (tx >= 0 && tx < width && ty >= 0 && ty < height) {
          cimg_forC(srcImgB, c) { targetImgB(x, y, 0, c) = srcImgB.linear_atXY(tx, ty, 0, c); }
        }
        isFind = true;
        break;
      }
    }
    if (!isFind) {
      cimg_forC(srcImgB, c) { targetImgB(x, y, 0, c) = srcImgB.linear_atXY(x, y, 0, c); }
    }
  }
  targetImgB.display();

  cimg_forXYZC(targetImg, x, y, z, c) {
    targetImg(x, y, z, c) = rate * targetImgB(x, y, z, c) + (1 - rate) * targetImgA(x, y, z, c);
  }
}

#endif
