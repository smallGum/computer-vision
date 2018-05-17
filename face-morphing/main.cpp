#include <iostream>
#include "face_morphing.h"

#define NUM_FACE_POINTS 83

using namespace std;
using namespace cimg_library;

int main() {
  while (true) {
    cout << "+---------------------------------+" << endl;
    cout << "|     My face morphing Program    |" << endl;
    cout << "+---------------------------------+" << endl;
    cout << endl;
    cout << "      0. Exit                      " << endl;
    cout << "      1. man-to-girl morphing      " << endl;
    cout << "      2. dataset 2                 " << endl;
    cout << "      3. dataset 3                 " << endl;
    cout << endl;
    cout << "Please enter your choice (0-3): ";

    int choice;
    cin >> choice;
    cin.get();

    string imgA, imgB, imgACtrPoints, imgBCtrPoints;
    switch (choice) {
      case 0:
        return 0;
      case 1:
        imgA = "./testData/data1/man.bmp";
        imgB = "./testData/data1/girl.bmp";
        imgACtrPoints = "./testData/data1/man_face_points.txt";
        imgBCtrPoints = "./testData/data1/girl_face_points.txt";
        break;
      case 2:
        imgA = "./testData/data2/man.bmp";
        imgB = "./testData/data2/woman.bmp";
        imgACtrPoints = "./testData/data2/man_face_points.txt";
        imgBCtrPoints = "./testData/data2/woman_face_points.txt";
        break;
      case 3:
        imgA = "./testData/data3/woman.bmp";
        imgB = "./testData/data3/chun.bmp";
        imgACtrPoints = "./testData/data3/woman_face_points.txt";
        imgBCtrPoints = "./testData/data3/chun_face_points.txt";
        break;
      default:
        cout << "no such choice! quit!" << endl;
        return 0;
    }

    controlInfo ci(imgA, imgB, imgACtrPoints, imgBCtrPoints, NUM_FACE_POINTS);
    double rate = 0.0;
    for (int i = 0; i < 11; ++i) {
      cout << "dealing middle image with rate = " << rate << endl;
      morph mh(ci, rate);
      CImg<double> result = mh.getTargetImg();
      char file[100];
      sprintf(file, "./result/data%d/rate%lf.png", choice, rate);
      result.save(file);
      rate += 0.1;
    }
  }

  return 0;
}
