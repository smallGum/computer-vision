#include "canny.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace cimg_library;
using namespace std;

struct point {
  double x, y;
  point(double _x, double _y): x(_x), y(_y) {}
};

struct pixel {
  point pt;
  double val;
  pixel(double _x, double _y, double v) : pt(point(_x, _y)), val(v) {}
  bool operator<(const pixel& d) { return val < d.val; }
};

struct num_idx {
  int idx;
  double num;
  num_idx(int i, int n): idx(i), num(n) {}
  bool operator<(const num_idx& d) { return num < d.num; }
};

class circleExtract {
private:
	CImg<unsigned char> grayImg;
	CImg<unsigned char> edgeImg;
	CImg<double> xGradImg;
	CImg<double> yGradImg;
  vector<pixel> circles;

	double minDist;
	double cannyHighThreshold;
	double houghThreshold;
	int minRadius;
	int maxRadius;

	bool isZero(double num) { return fabs(num) < 10e-6; }
	int mround(double num) { return num < 0.0 ? (int)(num - 0.5) : (int)(num + 0.5); }

public:
	circleExtract(CImg<unsigned char>& _image, double min_dist, double cThreshold, double hThreshold, int _minRadius, int _maxRadius);
	void findCircles();
  vector<pixel> getCircles() { return circles; }
};

circleExtract::circleExtract(CImg<unsigned char>& _image, double min_dist, double cThreshold, double hThreshold, int _minRadius, int _maxRadius) {
	grayImg = CImg<unsigned char>(_image);
  grayImg.display();
	canny edgeCatch(grayImg);
	edgeImg = edgeCatch.getEdgeImg();
  edgeImg.display();
	xGradImg = edgeCatch.getXGradImg();
	yGradImg = edgeCatch.getYGradImg();
  xGradImg.display();
  yGradImg.display();

	minDist = min_dist;
	cannyHighThreshold = cThreshold;
	houghThreshold = hThreshold;

	minRadius = max(_minRadius, 0);
	if( _maxRadius <= 0 )
    maxRadius = max(grayImg.width(), grayImg.height());
  else if( _maxRadius <= _minRadius )
    maxRadius = _minRadius + 2;

  findCircles();
}

void circleExtract::findCircles() {
	int min_radius = minRadius, max_radius = maxRadius;
	const int SHIFT = 10, ONE = 1 << SHIFT;
	int width = edgeImg.width();
	int height = edgeImg.height();
	CImg<double> accum(width, height, 1, 1, 0);
  vector<num_idx> dist_buf;
	vector<int> sort_buf;
	vector<pixel> centers;
	vector<point*> nz;

	double min_radius2 = (double)min_radius * min_radius;
  double max_radius2 = (double)max_radius * max_radius;

	cimg_forXY(edgeImg, x, y) {
		double vx = xGradImg(x, y), vy = yGradImg(x, y);
		if (edgeImg(x, y) && (!isZero(vx) || !isZero(vy))) {
			double mag = sqrt(vx * vx + vy * vy);
			int sx = mround(-vx * ONE / mag);
      int sy = mround(vy * ONE / mag);
			int x0 = x * ONE;
      int y0 = y * ONE;

			for(int k1 = 0; k1 < 2; k1++ )
      {
          int x1 = x0 + min_radius * sx;
          int y1 = y0 + min_radius * sy;

          for( int r = min_radius; r <= max_radius; x1 += sx, y1 += sy, r++ )
          {
              int x2 = x1 >> SHIFT, y2 = y1 >> SHIFT;

              if( (unsigned)x2 >= (unsigned)width || (unsigned)y2 >= (unsigned)height )
                break;

              accum(x2, y2)++;
          }

          sx = -sx; sy = -sy;
        }

			point* pt = new point(x, y);
			nz.push_back(pt);
		}
	}

  cout << nz.size() << endl;

	if (!nz.size()) { return ; }

	for( int x = 1; x < width - 1; x++ )
  {
    for( int y = 1; y < height - 1; y++ )
    {
      //如果当前的值大于阈值，并在4邻域内它是最大值，则该点被认为是圆心
      if( accum(x, y) > houghThreshold &&
        accum(x, y) > accum(x - 1, y) && accum(x, y) > accum(x + 1, y) &&
        accum(x, y) > accum(x, y - 1) && accum(x, y) > accum(x, y + 1) ) {
        //把当前点的地址压入圆心序列centers中
        pixel px(x, y, accum(x, y));
        centers.push_back(px);
      }
     }
  }

  cout << centers.size() << endl;

  if (!centers.size()) { return ; }

  accum.display();

  sort(centers.begin(), centers.end());
  double min_dist = minDist * minDist;
  for (int i = centers.size() - 1; i >= 0; --i) {
    double cx = (double)(centers[i].pt.x + 0.5), cy = (double)(centers[i].pt.y + 0.5);
    double start_dist, dist_sum;
    double r_best = 0;
    int max_count = 0;
    int j, k;

    for (j = 0; j < circles.size(); ++j) {
      pixel c = circles[j];
      if ((c.pt.x - cx) * (c.pt.x - cx) + (c.pt.y - cy) * (c.pt.y - cy) < min_dist) { break; }
    }
    if (j < circles.size()) { continue; }

    for (j = 0, k = 0; j < nz.size(); ++j) {
      double _dx = cx - nz[j] -> x, _dy = cy - nz[j] -> y;
      double _r2 = _dx * _dx + _dy * _dy;
      if (min_radius2 <= _r2 && _r2 <= max_radius2) {
        num_idx tmp(k, sqrt(_r2));
        dist_buf.push_back(tmp);
        k++;
      }
    }

    int nz_count1 = k, start_idx = 0;
    if (nz_count1 == 0) { continue; }
    sort(dist_buf.begin(), dist_buf.end());
    for (int l = 0; l < nz_count1; ++l) { sort_buf.push_back(dist_buf[l].idx); }

    dist_sum = start_dist = dist_buf[sort_buf[0]].num;
    for (j = 1; j < nz_count1; ++j) {
      double d = dist_buf[sort_buf[j]].num;
      if (d > maxRadius) { break; }
      if (d - start_dist > 1) {
        double r_cur = dist_buf[sort_buf[(j + start_idx) / 2]].num;
        if ((double)(j - start_idx) * r_best >= max_count * r_cur || (r_best < 10e-6 && j - start_idx >= max_count)) {
          r_best = r_cur;
          max_count = j - start_idx;
        }
        start_dist = d;
        start_idx = j;
        dist_sum = 0;
      }
      dist_sum += d;
    }

    if (max_count > houghThreshold) {
      pixel px(cx, cy, r_best);
      circles.push_back(px);
    }
  }

  //return centers;
  //cout << centers.size() << endl;
}
