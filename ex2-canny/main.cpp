#include "canny.h"
#include <iostream>

using namespace std;

int main() {
  float lowThreshold = 2.5f;
  float highthreshold = 7.5f;
  float gaussiankernelradius = 2.0f;
  int gaussiankernelwidth = 16;
  int contrastnormalised = 0;

  while (true) {
    cout << "+---------------------------------+" << endl;
    cout << "|     My Packaged Canny Program   |" << endl;
    cout << "+---------------------------------+" << endl;
    cout << endl;
    cout << "      0. Exit                      " << endl;
    cout << "      1. bigben                    " << endl;
    cout << "      2. lena                      " << endl;
    cout << "      3. stpietro                  " << endl;
    cout << "      4. twows                     " << endl;
    cout << endl;
    cout << "Please choose your image (0-4): " << endl;

    int task;
    char* filename;
    cin >> task;

    switch(task) {
        case 0:
            return 0;
        case 1:
            filename = "images/bigben.bmp";
            break;
        case 2:
            filename = "images/lena.bmp";
            break;
        case 3:
            filename = "images/stpietro.bmp";
            break;
        case 4:
            filename = "images/twows.bmp";
            break;
        default:
            cout << "no such image! Please enter again!" << endl;
            break;
    }

    imageIO img(filename);
    CImg<unsigned char> grey = img.getGreyImg();
    canny edge(grey, lowThreshold, highthreshold, gaussiankernelradius, gaussiankernelwidth, contrastnormalised);
    CImg<unsigned char> rst = edge.getEdgeImg();
    rst.display();
  }

  return 0;
}
