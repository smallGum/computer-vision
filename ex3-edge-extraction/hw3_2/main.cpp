#include "hw3_2.h"

int main() {
      // get the opertaion from user
    while (true) {
        cout << "+---------------------------------+" << endl;
        cout << "|   My circle extraction Program  |" << endl;
        cout << "+---------------------------------+" << endl;
        cout << endl;
        cout << "      0. Exit                      " << endl;
        cout << "      1. circle_1 image                " << endl;
        cout << "      2. circle_2 image                " << endl;
        cout << "      3. circle_3 image                " << endl;
        cout << "      4. circle_4 image                " << endl;
        cout << "      5. circle_5 image                " << endl;
        cout << "      6. circle_6 image                " << endl;
        cout << endl;
        cout << "Please choose your image (0-6): " << endl;

        int image;
        char filename[] = "images/circle_1.bmp";
        cin >> image;
        cin.get();

        switch(image) {
            case 0:
                return 0;
            case 1:
                filename[14] = '1';
                break;
            case 2:
                filename[14] = '2';
                break;
            case 3:
                filename[14] = '3';
                break;
            case 4:
                filename[14] = '4';
                break;
            case 5:
                filename[14] = '5';
                break;
            case 6:
                filename[14] = '6';
                break;
            default:
                cout << "no such image! Please enter again!" << endl;
                break;
        }

        // read the source image
        imageIO img(filename);
        // get the grey image
        CImg<unsigned char> grey = img.getGreyImg();
        CImg<unsigned char> src = img.getSrcImg();
        src.display();

        circleExtract circle(grey, 200, 320, 420);
        vector<pixel> rst = circle.getCircles();
        CImg<unsigned char> edge = circle.getEdge();

        unsigned char yellow[] = { 255, 255, 0 };
        unsigned char red[] = {255, 0, 0};
        for (int i = 0; i< rst.size(); ++i) {
            src.draw_circle(rst[i].pt.x, rst[i].pt.y, 20, yellow);
            src.draw_circle(rst[i].pt.x, rst[i].pt.y, rst[i].val, red, 1, ~0U);
        }

        edge.display();
        src.display();

        cout << "number of circles: " << rst.size() << endl;
    }

    return 0;
}
