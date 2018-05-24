#include "segmention.h"
#include <cstdio>
//#include <iostream>

//using namespace std;

int main() {
    char srcImgFile[100];
    char targetImgFile[100];
    for (int i = 1; i <= 100; ++i) {
        sprintf(srcImgFile, "./images/%d.JPG", i);
        //sprintf(targetImgFile, "./segmented_images/iteration/%d.jpg", i);
        sprintf(targetImgFile, "./segmented_images/ostu/%d.jpg", i);
        CImg<double> src(srcImgFile);

        //iterativeSegment sg(src);
        ostuSegment sg(src);
        CImg<double> rst = sg.getSegmentedImg();
        rst.save(targetImgFile);

        cout << "No." << i << " image done." << endl;
    }

    return 0;
}
