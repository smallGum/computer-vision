#include "CImg.h"
#include <iostream>

using namespace cimg_library;
using namespace std;

// display the image
void task1(const CImg<unsigned char>& img) { img.display(); }

// change the white (255, 255, 255) area of the image to red (255, 0, 0), 
// and the black (0, 0, 0) area to green (0, 255, 0)
void task2(CImg<unsigned char> img) {
    // traverse all the pixels
    for (int i = 0; i < img.width(); ++i) {
        for (int j = 0; j < img.height(); ++j) {
            // change white pixel to red pixel
            if ((int)img(i, j, 0, 0) == 255 && (int)img(i, j, 0, 1) == 255 && (int)img(i, j, 0, 2) == 255) {
                img(i, j, 0, 1) = img(i, j, 0, 2) = 0;
            }
            // change black pixel to green pixel
            if ((int)img(i, j, 0, 0) == 0 && (int)img(i, j, 0, 1) == 0 && (int)img(i, j, 0, 2) == 0) {
                img(i, j, 0, 1) = 255;
            }
        }
    }

    // display the changed image
    img.display();
}

// paint a blue circle on (50, 50) with radius == 30
void task3(CImg<unsigned char> img) {
    unsigned char blue[] = { 0, 0, 255 };
    img.draw_circle(50, 50, 30, blue);
    img.display();
}

// paint a yellow circle on (50, 50) with radius == 3
void task4(CImg<unsigned char> img) {
    unsigned char yellow[] = { 255, 255, 0 };
    img.draw_circle(50, 50, 3, yellow);
    img.display();
}

int main() {
    // read the image
    CImg<unsigned char> img("1.bmp");
    // receive the task and do the operation
    while (true) {
        cout << "+---------------------------------+" << endl;
        cout << "|     My Little CImg Program      |" << endl;
        cout << "+---------------------------------+" << endl;
        cout << endl;
        cout << "      0. Exit                      " << endl;
        cout << "      1. Display the image         " << endl;
        cout << "      2. Change color of some area " << endl;
        cout << "      3. Paint a blue circle       " << endl;
        cout << "      4. Paint a yellow circle     " << endl;
        cout << endl;
        cout << "Please enter your choice (0-4): " << endl;

        int task;
        cin >> task;

        switch(task) {
            case 0:
                return 0;
            case 1:
                task1(img);
                break;
            case 2:
                task2(img);
                break;
            case 3:
                task3(img);
                break;
            case 4:
                task4(img);
                break;
            default:
                cout << "no such choice! Please enter again!" << endl;
                break;
        }
    }

    return 0;
}