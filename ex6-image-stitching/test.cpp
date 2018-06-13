#include "stitching.h"

int main() {
  CImg<float> src = CImg<float>("./images1/pano1_0011.bmp");
  sift s(src);
  vector<keyPoint*> kps = s.getKeyPoints();
  cout << "total key points: " << kps.size() << endl;

  return 0;
}
