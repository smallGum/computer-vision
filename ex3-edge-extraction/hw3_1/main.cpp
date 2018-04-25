#include "hw3_1.h"
#include <iostream>

int main() {
  // get the opertaion from user
  while (true) {
    cout << "+---------------------------------+" << endl;
    cout << "|    My edge extraction Program   |" << endl;
    cout << "+---------------------------------+" << endl;
    cout << endl;
    cout << "      0. Exit                      " << endl;
    cout << "      1. A4_1 image                " << endl;
    cout << "      2. A4_2 image                " << endl;
    cout << "      3. A4_3 image                " << endl;
    cout << "      4. A4_4 image                " << endl;
    cout << "      5. A4_5 image                " << endl;
    cout << "      6. A4_6 image                " << endl;
    cout << endl;
    cout << "Please choose your image (0-6): " << endl;

    int image;
    char filename[] = "images/A4_1.bmp";
    cin >> image;
    cin.get();

    int blur = 3;
    double grad = 20;
    double diff = 200;
    double threshold = 650;

    switch(image) {
        case 0:
            return 0;
        case 1:
            filename[10] = '1';
            blur = 5;
            grad = 20;
            threshold = 500;
            diff = 50;
            break;
        case 2:
            filename[10] = '2';
            grad = 20;
            blur = 1;
            threshold = 800;
            diff = 400;
            break;
        case 3:
            filename[10] = '3';
            break;
        case 4:
            filename[10] = '4';
            blur = 1;
            grad = 20;
            threshold = 400;
            diff = 400;
            break;
        case 5:
            filename[10] = '5';
            break;
        case 6:
            filename[10] = '6';
            break;
        default:
            cout << "no such image! Please enter again!" << endl;
            break;
    }

    imageIO img(filename);
    CImg<double> src = img.getSrcImg();
    CImg<double> gray = img.getGreyImg();
    A4EdgeExtract edgeEct(src, gray, blur, grad, diff, threshold);
    CImg<double> edge = edgeEct.getEdgeImg();
    CImg<double> hough = edgeEct.getHoughImg();
    CImg<double> result = edgeEct.getResultImg();

    edge.display();
    hough.display();
    edgeEct.printLines();
    edgeEct.printIntersections();
    result.display();
  }

  return 0;
}
