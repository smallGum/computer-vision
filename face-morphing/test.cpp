#include <fstream>
#include <iostream>
#include <vector>
#include "CImg.h"

#define NUM_FACE_POINTS 83

using namespace std;
using namespace cimg_library;

int main() {
    CImg<double> src("./testData/data1/girl.bmp");
    ifstream in("./testData/data1/girl_face_points.txt");
    int X;
    int Y;
    const double color[] = { 255, 255, 0 };

    for (int i = 0; i < NUM_FACE_POINTS; ++i) {
        in >> X;
        in >> Y;
        src.draw_circle(X, Y, 1, color);
    }

    in.close();
    src.display();
    return 0;
}
