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
    double rate;
    cin >> choice; cin.get();
    if (choice == 0) { break; }
    cout << "Please enter the rate: ";
    cin >> rate; cin.get();

    string imgA, imgB, imgACtrPoints, imgBCtrPoints;
    switch (choice) {
      case 1:
        imgA = "./testData/data1/man.bmp";
        imgB = "./testData/data1/girl.bmp";
        imgACtrPoints = "./testData/data1/man_face_points.txt";
        imgBCtrPoints = "./testData/data1/girl_face_points.txt";
        break;
      default:
        cout << "no such choice! Please enter again!" << endl;
        break;
    }

    controlInfo ci(imgA, imgB, imgACtrPoints, imgBCtrPoints, NUM_FACE_POINTS);
    morph mh(ci, rate);
  }

  return 0;
}
