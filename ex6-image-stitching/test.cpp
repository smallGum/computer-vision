#include "CImg.h"
#include <vl/generic.h>
#include <vl/sift.h>
#include <iostream>
#include <cstdlib>

using namespace cimg_library;
using namespace std;

int main() {
  VL_PRINT("My sift test.\n");
  CImg<float> src = CImg<float>("./images1/pano1_0010.bmp");
  CImg<float> gray = CImg<float>(src.width(), src.height(), 1, 1);
  cimg_forXY(src, x, y) {
    float b = src(x, y, 0, 0);
    float g = src(x, y, 0, 1);
    float r = src(x, y, 0, 2);

    float newValue = (float)(r * 0.2126 + g * 0.7152 + b * 0.0722);
    gray(x, y, 0, 0) = newValue;
  }
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
        pKeyPoint++;
        unsigned char yellow[] = { 255, 255, 0 };
        src.draw_circle(TemptKeyPoint.x, TemptKeyPoint.y, TemptKeyPoint.sigma / 2, yellow, 1, ~0U);
        idx++;
        double angles[4];
        int angleCount = vl_sift_calc_keypoint_orientations(SiftFilt, angles, &TemptKeyPoint);
        for (int j = 0; j < angleCount; ++j) {
          double TemptAngle = angles[j];
          cout << j << ": " << TemptAngle << endl;
          float *Descriptors = new float[128];
          vl_sift_calc_keypoint_descriptor(SiftFilt, Descriptors, &TemptKeyPoint, TemptAngle);
          int k = 0;
          while (k < 128) {
            cout << k << ": " << Descriptors[k] << "; ";
            k++;
          }
          cout << endl;
          delete [] Descriptors;
          Descriptors = NULL;
        }
      }
      if (vl_sift_process_next_octave(SiftFilt) == VL_ERR_EOF) { break; }
      KeyPoint = NULL;
    }
  }

  vl_sift_delete(SiftFilt);
  delete [] ImageData;
  ImageData = NULL;
  src.display();
  return 0;
}
