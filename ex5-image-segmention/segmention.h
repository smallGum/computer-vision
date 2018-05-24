#include "CImg.h"
#include <cmath>
#include <iostream>

using namespace std;

using namespace cimg_library;

#define DELTA_THRESHOLD 0.1

class iterativeSegment {
private:
    CImg<double> srcImg;
    CImg<double> grayImg;
    CImg<double> segmentedImg;
    double threshold;

    void calGrayImg();
    void findBestThreshold();
    void segment();

public:
    iterativeSegment(CImg<double>& src);
    CImg<double>& getSegmentedImg() { return segmentedImg; }
};

class ostuSegment {
private:
    CImg<double> srcImg;
    CImg<double> grayImg;
    CImg<double> segmentedImg;
    double threshold;

    void calGrayImg();
    void findBestThreshold();
    void segment();

public:
    ostuSegment(CImg<double>& src);
    CImg<double>& getSegmentedImg() { return segmentedImg; }
};

void iterativeSegment::calGrayImg() {
    for (unsigned int i = 0; i < srcImg.width(); ++i) {
        for (unsigned int j = 0; j < srcImg.height(); ++j) {
            double b = srcImg(i, j, 0, 0);
            double g = srcImg(i, j, 0, 1);
            double r = srcImg(i, j, 0, 2);

            double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
            grayImg(i, j, 0, 0) = newValue;
        }
    }
}

void iterativeSegment::findBestThreshold() {
    while (true) {
        int G1Num = 0, G2Num = 0;
        double G1Sum = 0.0, G2Sum = 0.0;

        cimg_forXY(grayImg, x, y) {
            if (grayImg(x, y, 0, 0) < threshold) {
                G1Num++;
                G1Sum += grayImg(x, y, 0, 0);
            } else {
                G2Num++;
                G2Sum += grayImg(x, y, 0, 0);
            }
        }

        double newThreshold = (G1Sum / (double)G1Num + G2Sum / (double)G2Num) / 2.0;
        if (fabs(newThreshold - threshold) < DELTA_THRESHOLD) {
            threshold = newThreshold;
            break;
        } else {
            threshold = newThreshold;
        }
    }
}

void iterativeSegment::segment() {
    cimg_forXY(grayImg, x, y) {
        segmentedImg(x, y, 0, 0) = grayImg(x, y, 0, 0) < threshold ? 0.0 : 255.0;
    }
}

iterativeSegment::iterativeSegment(CImg<double>& src) {
    srcImg = src;
    grayImg = CImg<double>(src.width(), src.height(), 1, 1, 0.0);
    segmentedImg = CImg<double>(src.width(), src.height(), 1, 1, 0.0);

    calGrayImg();
    double tmpSum = 0.0;
    cimg_forXY(grayImg, x, y) { tmpSum += grayImg(x, y, 0, 0); }
    threshold = tmpSum / (src.width() * src.height());
    findBestThreshold();
    segment();
}

void ostuSegment::calGrayImg() {
    for (unsigned int i = 0; i < srcImg.width(); ++i) {
        for (unsigned int j = 0; j < srcImg.height(); ++j) {
            double b = srcImg(i, j, 0, 0);
            double g = srcImg(i, j, 0, 1);
            double r = srcImg(i, j, 0, 2);

            double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
            grayImg(i, j, 0, 0) = newValue;
        }
    }
}

void ostuSegment::findBestThreshold() {
    double  TT = 0.0;
    for (int T = 0; T <= 255; ++T) {
        int G1Num = 0, G2Num = 0;
        double G1Sum = 0.0, G2Sum = 0.0;
        double W1 = 0.0, W2 = 0.0, U1 = 0.0, U2 = 0.0, g = 0.0;

        cimg_forXY(grayImg, x, y) {
            if (grayImg(x, y, 0, 0) > (double)T) {
                G2Num++;
                G2Sum += grayImg(x, y, 0, 0);
            } else {
                G1Num++;
                G1Sum += grayImg(x, y, 0, 0);
            }
        }

        W1 = ((double)G1Num / (double)(G1Num + G2Num));
        W2 = 1.0 - W1;
        U1 = (G1Num == 0 ? 0.0 : (G1Sum / (double)G1Num));
        U2 = (G2Num == 0 ? 0.0 : (G2Sum / (double)G2Num));
        g = W1 * W2 * (U1 - U2) * (U1 - U2);
        if (g > TT) {
            TT = g;
            threshold = (double)T;
        }
    }
}

void ostuSegment::segment() {
    cimg_forXY(grayImg, x, y) {
        segmentedImg(x, y, 0, 0) = grayImg(x, y, 0, 0) > threshold ? 255.0 : 0.0;
    }
}

ostuSegment::ostuSegment(CImg<double>& src) {
    srcImg = src;
    grayImg = CImg<double>(src.width(), src.height(), 1, 1, 0.0);
    segmentedImg = CImg<double>(src.width(), src.height(), 1, 1, 0.0);
    threshold = 0.0;

    calGrayImg();
    findBestThreshold();
    segment();
}
