#include "stitching.h"

int main() {
  CImg<float> srcA = CImg<float>("./images1/pano1_0008.bmp");
  CImg<float> srcB = CImg<float>("./images1/pano1_0009.bmp");
  sift s(srcA);
  vector<keyPoint*> kpsA = s.getKeyPoints();
  sift s2(srcB);
  vector<keyPoint*> kpsB = s2.getKeyPoints();
  match m(kpsA, kpsB, srcA, srcB);

  return 0;
}
