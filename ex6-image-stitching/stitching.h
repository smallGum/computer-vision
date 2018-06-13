#ifndef _STITCHING_
#define _STITCHING_

#include <vector>
#include <cstdlib>
#include <iostream>
#include <vl/generic.h>
#include <vl/sift.h>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

// dimension of the descriptors
#define DIM_DESCRIPTORS 128
// feature description gap
#define FEAT_DESC_GAP 1.0
// inliners gap
#define INLIERS_GAP 500.0

// keyPoint struct represents one sift key point of the image
struct keyPoint {
  float row, col;
  float descrip[DIM_DESCRIPTORS];

  keyPoint(float _row, float _col): row(_row), col(_col) {}
};

// point struct represents one point of the image
struct point {
  int col, row;
  point(): col(-1), row(-1) {}
  point(int _col, int _row): col(_col), row(_row) {}
};

struct matchedPair {
  point keyPointA;
  point keyPointB;
  float minDis;
  matchedPair(point _pa, point _pb, float _minDis): keyPointA(_pa), keyPointB(_pb), minDis(_minDis) {}
};

// sift class
// find out the key points of an image using sift algorithm
class sift {
private:
  CImg<float> src;
  CImg<float> gray;
  vector<keyPoint*> keyPoints;

  void toGray();

public:
  sift(CImg<float>& srcImg);
  vector<keyPoint*>& getKeyPoints() { return keyPoints; }
};



void sift::toGray() {
  gray = CImg<float>(src.width(), src.height(), 1, 1);
  cimg_forXY(src, x, y) {
    float b = src(x, y, 0, 0);
    float g = src(x, y, 0, 1);
    float r = src(x, y, 0, 2);

    float newValue = (float)(r * 0.2126 + g * 0.7152 + b * 0.0722);
    gray(x, y, 0, 0) = newValue;
  }
}

sift::sift(CImg<float>& srcImg) {
  src = CImg<float>(srcImg);
  toGray();
  int noctaves = 4, nlevels = 2, o_min = 0;
  vl_sift_pix *ImageData = new vl_sift_pix[gray.width() * gray.height()];
  cimg_forXY(gray, x, y) { ImageData[y * gray.width() + x] = gray(x, y, 0, 0); }
  VlSiftFilt *SiftFilt = NULL;
  SiftFilt = vl_sift_new(gray.width(), gray.height(), noctaves, nlevels, o_min);

  int KeyPoint = 0;
  int idx = 0;
  if (vl_sift_process_first_octave(SiftFilt, ImageData) != VL_ERR_EOF) {
    while (true) {
      vl_sift_detect(SiftFilt);
      KeyPoint += SiftFilt -> nkeys;
      cout << "number of key points: " << KeyPoint << endl;
      VlSiftKeypoint *pKeyPoint = SiftFilt -> keys;
      for (int i = 0; i < SiftFilt -> nkeys; ++i) {
        VlSiftKeypoint TemptKeyPoint = *pKeyPoint;
        keyPoint* newKP = new keyPoint(TemptKeyPoint.x, TemptKeyPoint.y);
        pKeyPoint++;
        unsigned char yellow[] = { 255, 255, 0 };
        src.draw_circle(TemptKeyPoint.x, TemptKeyPoint.y, TemptKeyPoint.sigma / 2, yellow, 1, ~0U);
        idx++;

        double angles[4];
        int angleCount = vl_sift_calc_keypoint_orientations(SiftFilt, angles, &TemptKeyPoint);
        for (int j = 0; j < angleCount; ++j) {
          double TemptAngle = angles[j];
          vl_sift_calc_keypoint_descriptor(SiftFilt, newKP -> descrip, &TemptKeyPoint, TemptAngle);
        }

        keyPoints.push_back(newKP);
      }

      if (vl_sift_process_next_octave(SiftFilt) == VL_ERR_EOF) { break; }
      KeyPoint = 0;
    }
  }

  vl_sift_delete(SiftFilt);
  delete [] ImageData;
  ImageData = NULL;
  src.display();
}

#endif
