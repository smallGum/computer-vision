#include "correction.h"
#include "prewitt.h"

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

    // set the parameters
    int blur;
    double grad;
    double diff;
    double threshold;

    switch(image) {
        case 0:
            return 0;
        case 1:
            filename[10] = '1';
            blur = 5;
            grad = 20;
            diff = 50;
            threshold = 500;
            break;
        case 2:
            filename[10] = '2';
            blur = 1;
            grad = 20;
            diff = 400;
            threshold = 800;
            break;
        case 3:
            filename[10] = '3';
            blur = 3;
            grad = 20;
            diff = 200;
            threshold = 650;
            break;
        case 4:
            filename[10] = '4';
            blur = 1;
            grad = 20;
            diff = 400;
            threshold = 400;
            break;
        case 5:
            filename[10] = '5';
            blur = 3;
            grad = 20;
            diff = 200;
            threshold = 650;
            break;
        case 6:
            filename[10] = '6';
            blur = 3;
            grad = 20;
            diff = 200;
            threshold = 650;
            break;
        default:
            cout << "no such image! Please enter again!" << endl;
            break;
    }

    imageIO img(filename);
    CImg<double> src = img.getSrcImg();
    CImg<double> grey = img.getGreyImg();
    prewitt edge(grey, blur, grad);
    CImg<double> edgeImg = edge.getEdgeImg();
    A4EdgeExtract edgeEct(src, edgeImg, diff, threshold);
    CImg<double> resultImg = edgeEct.getResultImg();
    vector<point*> v = edgeEct.getIntersections();
    A4Correct cr(src, v);
    CImg<double> targetImg = cr.getTargetImg();

    resultImg.display();
    targetImg.display();
  }

  return 0;
}
