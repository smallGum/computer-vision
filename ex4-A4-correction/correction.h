#include "hw3_1.h"
#include <algorithm>
#include <cstdlib>

int cnt = 0;

struct position {
  double x, y;

  position(double _x, double _y): x(_x), y(_y) {}
  double distance(const position& another) { return sqrt((x - another.x) * (x - another.x) + (y - another.y) * (y - another.y)); }
  bool operator==(const position& another) { return (fabs(x - another.x) < 1e-6) && (fabs(y - another.y) < 1e-6); }
  position operator-(const position& another) const { return position(x - another.x, y - another.y); }
  double dot(const position& another) const { return x * another.x + y * another.y; }
};

bool cmp(const position* a, const position* b) { return a -> y < b -> y; }

void swap(position* a, position* b) {
  position tmp(a -> x, a -> y);
  a -> x = b -> x;
  a -> y = b -> y;
  b -> x = tmp.x;
  b -> y = tmp.y;
}

position* intersect(line l1, line l2) {
  double m0 = l1.m;
  double m1 = l2.m;
  double b0 = l1.b;
  double b1 = l2.b;
  double x = (b1 - b0) / (m0 - m1);
  double y = (m0 * b1 - m1 * b0) / (m0 - m1);

  return new position(x, y);
}

struct triangle {
  position a, b, c;

  triangle(position _a, position _b, position _c): a(_a), b(_b), c(_c) {}
};

bool positionInTriangle(triangle t, position p) {
  position v0 = t.c - t.a;
  position v1 = t.b - t.a;
  position v2 = p - t.a;

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

class A4Correct {
private:
  vector<position*> vertices;
  CImg<double> targetImg;
  CImg<double> srcImg;

  void reorder();
  position* morph(triangle source, triangle target, position p);
  void calTargetImg();
  double bilinear(double x, double y, int z, int channel);

public:
  A4Correct(CImg<double>& src, vector<point*>& v);
  CImg<double> getTargetImg() { return targetImg; }
};

A4Correct::A4Correct(CImg<double>& src, vector<point*>& v) {
  srcImg = CImg<double>(src);
  for (int i = 0; i < v.size(); ++i) { vertices.push_back(new position(v[i] -> x, v[i] -> y)); }

  reorder();

  int width = (int)vertices[0] -> distance(*(vertices[1]));
  int height = (int)vertices[1] -> distance(*(vertices[2]));
  targetImg = CImg<double>(width + 20, height + 20, 1, 3, 0);

  calTargetImg();
}

void A4Correct::reorder() {
  sort(vertices.begin(), vertices.end(), cmp);
  double minimum = -5.0;
  int index;

  for (int i = 1; i < vertices.size(); ++i) {
    double dist = vertices[0] -> distance(*vertices[i]);
    if (minimum < 0.0 || minimum > dist) {
      minimum = dist;
      index = i;
    }
  }
  swap(vertices[1], vertices[index]);

  if (vertices[1] -> y > vertices[2] -> y || vertices[1] -> y > vertices[3] -> y) {
    swap(vertices[0], vertices[1]);
    if (vertices[2] -> y > vertices[3] -> y) { swap(vertices[2], vertices[3]); }
  } else {
    if (vertices[0] -> x > vertices[1] -> x) { swap(vertices[0], vertices[1]); }
    if (vertices[2] -> x < vertices[3] -> x) { swap(vertices[2], vertices[3]); }
  }
}

position* A4Correct::morph(triangle source, triangle target, position p) {
  position a = source.a, b = source.b, c = source.c;
  position at = target.a, bt = target.b, ct = target.c;

  if (p == a) { return new position(at.x, at.y); }
  if (p == b) { return new position(bt.x, bt.y); }
  if (p == c) { return new position(ct.x, ct.y); }

  line cp(c.x, c.y, p.x, p.y);
  line ab(a.x, a.y, b.x, b.y);
  position* d = (cp.m == 0 && cp.b == 0) ? new position(b) : intersect(cp, ab);

  double ABrate = (b.x - a.x != 0) ? (d->x - a.x) / (b.x - a.x) : (d->y - a.y) / (b.y - a.y);
  double CDrate = (d->x - c.x != 0) ? (p.x - c.x) / (d->x - c.x) : (p.y - c.y) / (d->y - c.y);

  double dtx = ABrate*(bt.x - at.x) + at.x;
  double dty = ABrate*(bt.y - at.y) + at.y;

  double ptx = CDrate*(dtx - ct.x) + ct.x;
  double pty = CDrate*(dty - ct.y) + ct.y;
  return new position(ptx, pty);
}

void A4Correct::calTargetImg() {
  position At(10, 10);
  position Bt(targetImg.width() - 10, 10);
  position Ct(targetImg.width() - 10, targetImg.height() - 10);
  position Dt(10, targetImg.height() - 10);

  position A(vertices[0] -> x, vertices[0] -> y);
  position B(vertices[1] -> x, vertices[1] -> y);
  position C(vertices[2] -> x, vertices[2] -> y);
  position D(vertices[3] -> x, vertices[3] -> y);

  cimg_forXY(targetImg, x, y) {
    position P(x, y), *p = NULL;
    if (positionInTriangle(triangle(At, Bt, Ct), P)) {
      p = morph(triangle(At, Bt, Ct), triangle(A, B, C), P);
    }
    else if (positionInTriangle(triangle(Ct, Dt, At), P)) {
      p = morph(triangle(Ct, Dt, At), triangle(C, D, A), P);
    }

    if (p != NULL) {
      cimg_forC(targetImg, c)
      targetImg(x, y, 0, c) = bilinear(p->x, p->y, 0, c);
    }
  }
}

double A4Correct::bilinear(double x, double y, int z, int channel) {
  int srcX = (int)x;
  int srcY = (int)y;
  double u = x - (double)srcX;
  double v = y - (double)srcY;

  return (1.0 - u) * (1.0 - v) * srcImg(srcX, srcY, z, channel) +
         (1.0 - u) * v * srcImg(srcX, srcY + 1, z, channel) +
         u * (1.0 - v) * srcImg(srcX + 1, srcY, z, channel) +
         u * v * srcImg(srcX + 1, srcY + 1);
}
