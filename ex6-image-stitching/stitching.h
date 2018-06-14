#ifndef _STITCHING_
#define _STITCHING_

#include <vector>
#include <cstdlib>
#include <iostream>
#include <vl/generic.h>
#include <vl/sift.h>
#include "CImg.h"
#include <cmath>

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

// pair of two matched key points
struct matchedPair {
  point keyPointA;
  point keyPointB;
  float minDis;

  matchedPair(point _pa, point _pb, float _minDis): keyPointA(_pa), keyPointB(_pb), minDis(_minDis) {}
};

// transformation vector of the two images
struct transVec {
  int dx, dy;

  transVec(): dx(-1), dy(-1) {}
  transVec(int _dx, int _dy): dx(_dx), dy(_dy) {}
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

// match class
// find out the matched pairs between two images
class match {
private:
	vector<keyPoint*> AKeyPoints;
	vector<keyPoint*> BKeyPoints;
	vector<matchedPair> matchedPairs;
	point matchedVec;

	CImg<float> srcImgA, srcImgB;
  CImg<float> srcImgWithKpA, srcImgWithKpB;
  CImg<float> mixImg;
  CImg<float> fixedMatchedImg;

  void matchKeyPoints();
	void drawOriginalKeyPoints();
	void mergeImagesWithLines();
	void RANSAC();
	void drawRealKeyPoints(int maxIndex);

public:
	match(vector<keyPoint*>& A, vector<keyPoint*>& B, CImg<float>& imgA, CImg<float>& imgB);
	point getMatchedVec() { return matchedVec; }
};

// stitch class
// stitch two images according to the transformation vector
class stitch {
private:
  CImg<float> srcImgA;
  CImg<float> srcImgB;
  CImg<float> stitchedImg;
  transVec matchedVec;

  void stitching();

public:
  stitch(CImg<float>& imgA, CImg<float>& imgB, int sx, int sy);
  void saveStitchedImg(char* stitchedImgAddr);
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
  int noctaves = 2, nlevels = 1, o_min = 0;
  vl_sift_pix *ImageData = new vl_sift_pix[gray.height() * gray.width()];
  cimg_forXY(gray, x, y) { ImageData[y * gray.width() + x] = gray(x, y, 0, 0); }
  VlSiftFilt *SiftFilt = NULL;
  SiftFilt = vl_sift_new(gray.width(), gray.height(), noctaves, nlevels, o_min);

  int idx = 0;
  if (vl_sift_process_first_octave(SiftFilt, ImageData) != VL_ERR_EOF) {
    while (true) {
      vl_sift_detect(SiftFilt);
      VlSiftKeypoint *pKeyPoint = SiftFilt -> keys;
      for (int i = 0; i < SiftFilt -> nkeys; ++i) {
        VlSiftKeypoint TemptKeyPoint = *pKeyPoint;
        keyPoint* newKP = new keyPoint(TemptKeyPoint.y, TemptKeyPoint.x);
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
    }
  }

  vl_sift_delete(SiftFilt);
  delete [] ImageData;
  ImageData = NULL;
  src.display();
}

match::match(vector<keyPoint*>& A, vector<keyPoint*>& B, CImg<float>& imgA, CImg<float>& imgB) {
	AKeyPoints = A;
	BKeyPoints = B;
	srcImgA = CImg<float>(imgA);
	srcImgB = CImg<float>(imgB);
	srcImgWithKpA = CImg<float>(imgA);
	srcImgWithKpB = CImg<float>(imgB);
	mixImg = CImg<float>(srcImgA._width + srcImgB._width, max(srcImgA._height, srcImgB._height), 1, 3, 0);
	fixedMatchedImg = CImg<float>(srcImgA._width + srcImgB._width, srcImgA._height, 1, 3, 0);

	matchKeyPoints();
	drawOriginalKeyPoints();
	mergeImagesWithLines();
	RANSAC();
}

void match::matchKeyPoints() {
	for (int i = 0; i < AKeyPoints.size(); ++i) {
		keyPoint *tempDescA = AKeyPoints[i];
		float colA = tempDescA -> col;
    float rowA = tempDescA -> row;
    float *kp_desc_A = tempDescA -> descrip;
		float minSSD = 100.0;
    int minIndex = -1;
    int colB = -1;
    int rowB = -1;

		for (int j = 0; j < BKeyPoints.size(); ++j) {
			keyPoint *tempDescB = BKeyPoints[j];
			float ssd = 0;
			for (int k = 0; k < DIM_DESCRIPTORS; ++k) {
      	float descA = *(kp_desc_A + k);
        float descB = *(tempDescB -> descrip + k);
        ssd += abs(descA - descB);
      }
			if (ssd < minSSD) {
      	minSSD = ssd;
        colB = tempDescB -> col;
        rowB = tempDescB -> row;
      }
		}

		if (minSSD < FEAT_DESC_GAP) {
    	point pa(tempDescA -> col, tempDescA -> row);
      point pb(colB, rowB);

      matchedPair mpair(pa, pb, minSSD);
      matchedPairs.push_back(mpair);
    }
	}

  cout << ">>> matchedPairs.size: " << matchedPairs.size() << endl;
}

void match::drawOriginalKeyPoints() {
	const double yellow[] = { 255, 255, 0 };
	for (int i = 0; i < matchedPairs.size(); ++i) {
		srcImgWithKpA.draw_circle(matchedPairs[i].keyPointA.col, matchedPairs[i].keyPointA.row, 3, yellow, 1.0f);
		srcImgWithKpB.draw_circle(matchedPairs[i].keyPointB.col, matchedPairs[i].keyPointB.row, 3, yellow, 1.0f);
	}
	srcImgWithKpA.display("srcImgAWithKeyPoints");
	srcImgWithKpB.display("srcImgBWithKeyPoints");
}

void match::mergeImagesWithLines() {
	cimg_forXY(mixImg, x, y) {
  	if (x < srcImgA._width) {
    	if (y < srcImgA._height) {
      	mixImg(x, y, 0, 0) = srcImgWithKpA(x, y, 0, 0);
        mixImg(x, y, 0, 1) = srcImgWithKpA(x, y, 0, 1);
        mixImg(x, y, 0, 2) = srcImgWithKpA(x, y, 0, 2);
      }
      else {
      	mixImg(x, y, 0, 0) = 0;
        mixImg(x, y, 0, 1) = 0;
        mixImg(x, y, 0, 2) = 0;
      }
    }
  	else {
      if (y < srcImgB._height) {
        mixImg(x, y, 0, 0) = srcImgWithKpB(x - srcImgA._width, y, 0, 0);
        mixImg(x, y, 0, 1) = srcImgWithKpB(x - srcImgA._width, y, 0, 1);
        mixImg(x, y, 0, 2) = srcImgWithKpB(x - srcImgA._width, y, 0, 2);
      }
      else {
        mixImg(x, y, 0, 0) = 0;
        mixImg(x, y, 0, 1) = 0;
      	mixImg(x, y, 0, 2) = 0;
      }
  	}
  }
  mixImg.display("mergedImg");

	const double blue[] = { 0, 255, 255 };
  for (int i = 0; i < matchedPairs.size(); i++) {
    int xa = matchedPairs[i].keyPointA.col;
    int ya = matchedPairs[i].keyPointA.row;

    int xb = matchedPairs[i].keyPointB.col + srcImgA._width;
    int yb = matchedPairs[i].keyPointB.row;

    mixImg.draw_line(xa, ya, xb, yb, blue);
  }
	mixImg.display("mergedImgWithLine");
}

void match::RANSAC() {
	int maxInliers = 0;
  int maxIndex = -1;
  int inliersCount;

	for (int i = 0; i < matchedPairs.size(); ++i) {
		inliersCount = 0;
		int xa = matchedPairs[i].keyPointA.col;
    int ya = matchedPairs[i].keyPointA.row;
    int xb = matchedPairs[i].keyPointB.col + srcImgA._width;
    int yb = matchedPairs[i].keyPointB.row;
    int deltaX = xb - xa;
    int deltaY = yb - ya;

		for (int j = 0; j < matchedPairs.size(); ++j) {
			if (j != i) {
        int txa = matchedPairs[j].keyPointA.col;
        int tya = matchedPairs[j].keyPointA.row;
        int txb = matchedPairs[j].keyPointB.col + srcImgA._width;
        int tyb = matchedPairs[j].keyPointB.row;
        int tdeltaX = txb - txa;
        int tdeltaY = tyb - tya;
        int vectorGap = (tdeltaX - deltaX) * (tdeltaX - deltaX) + (tdeltaY - deltaY) * (tdeltaY - deltaY);

				if (vectorGap < INLIERS_GAP) { inliersCount++; }
			}
		}

		if (inliersCount > maxInliers) {
			maxInliers = inliersCount;
			maxIndex = i;
		}
	}

	cout << "maxIndex: " << maxIndex << ", maxInliers: " << maxInliers << endl;
	drawRealKeyPoints(maxIndex);
}

void match::drawRealKeyPoints(int maxIndex) {
	cimg_forXY(fixedMatchedImg, x, y) {
    if (x < srcImgA._width) {
      if (y < srcImgA._height) {
        fixedMatchedImg(x, y, 0, 0) = srcImgWithKpA(x, y, 0, 0);
        fixedMatchedImg(x, y, 0, 1) = srcImgWithKpA(x, y, 0, 1);
        fixedMatchedImg(x, y, 0, 2) = srcImgWithKpA(x, y, 0, 2);
      }
      else {
        fixedMatchedImg(x, y, 0, 0) = 0;
        fixedMatchedImg(x, y, 0, 1) = 0;
        fixedMatchedImg(x, y, 0, 2) = 0;
      }
    }
    else {
      if (y < srcImgB._height) {
        fixedMatchedImg(x, y, 0, 0) = srcImgWithKpB(x - srcImgA._width, y, 0, 0);
        fixedMatchedImg(x, y, 0, 1) = srcImgWithKpB(x - srcImgA._width, y, 0, 1);
        fixedMatchedImg(x, y, 0, 2) = srcImgWithKpB(x - srcImgA._width, y, 0, 2);
      }
      else {
        fixedMatchedImg(x, y, 0, 0) = 0;
        fixedMatchedImg(x, y, 0, 1) = 0;
        fixedMatchedImg(x, y, 0, 2) = 0;
      }
    }
  }

	int mxa = matchedPairs[maxIndex].keyPointA.col;
  int mya = matchedPairs[maxIndex].keyPointA.row;
  int mxb = matchedPairs[maxIndex].keyPointB.col + srcImgA._width;
  int myb = matchedPairs[maxIndex].keyPointB.row;
  int mdeltaX = mxb - mxa;
  int mdeltaY = myb - mya;
	matchedVec = point(mdeltaX, mdeltaY);
	cout << "Real match vector: (" << mdeltaX << ", " << mdeltaY << ")" << endl;

	const double blue[] = { 0, 255, 255 };
	for (int j = 0; j < matchedPairs.size(); ++j) {
		int txa = matchedPairs[j].keyPointA.col;
    int tya = matchedPairs[j].keyPointA.row;
    int txb = matchedPairs[j].keyPointB.col + srcImgA._width;
  	int tyb = matchedPairs[j].keyPointB.row;
  	int tdeltaX = txb - txa;
    int tdeltaY = tyb - tya;
    int vectorGap = (tdeltaX - mdeltaX) * (tdeltaX - mdeltaX) + (tdeltaY - mdeltaY) * (tdeltaY - mdeltaY);

		if (vectorGap < INLIERS_GAP) { fixedMatchedImg.draw_line(txa, tya, txb, tyb, blue); }
	}

	fixedMatchedImg.display("mergedImgWithLine_fixed");
}

#endif
