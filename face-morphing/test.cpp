#include <iostream>
#include "face_morphing.h"

#define NUM_FACE_POINTS 83

using namespace std;
using namespace cimg_library;

int main() {
  double rate;
  cout << "enter rate: " << endl;
  cin >> rate;
  string man("./testData/data1/man.bmp");
  string girl("./testData/data1/girl.bmp");
  string manFile("./testData/data1/man_face_points.txt");
  string girlFile("./testData/data1/girl_face_points.txt");

  controlInfo ci(man, girl, manFile, girlFile, NUM_FACE_POINTS);
  morph mh(ci, rate);

  return 0;
}
