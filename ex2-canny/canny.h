//
//  canny.h
//  Canny Edge Detector
//
//  Packaged by Jack Cheng on 10/04/18.
//  All rights reserved.
//

#ifndef _CANNY_
#define _CANNY_

#include <string>
#include "CImg.h"

using namespace cimg_library;

// images IO class
// read the source image and output its grey image
class imageIO {
private:
  CImg<unsigned char> srcImg;   // source image read from file
  CImg<unsigned char> greyImg;  // grey image of the source image

  // transform the source image to grey image
  void toGrey();

public:
  // constructor, read the source image from the file
  imageIO(const char* filename);
  // get the source image
  CImg<unsigned char> getSrcImg();
  // get the grey image
  CImg<unsigned char> getGreyImg();
};

// canny class
// detect the edge of the grey image
class canny {
private:
  unsigned char *data;   /* input image */
  unsigned char *answer; /* output edge image */
  int width;             /* input image's width */
  int height;            /* input image's height */
  int *idata;            /* output for edges */
  int *magnitude;        /* edge magnitude as detected by Gaussians */
  float *xConv;          /* temporary for convolution in x direction */
  float *yConv;          /* temporary for convolution in y direction */
  float *xGradient;      /* gradients in x direction, as detected by Gaussians */
  float *yGradient;      /* gradients in x direction,a s detected by Gaussians */

  // allocate buffers for related variables
  void allocatebuffers(unsigned char *grey);
  // release all the buffer allocated
  void killbuffers();
  // compute the gradients of the image
  void computeGradients(float kernelRadius, int kernelWidth);
  // perform hysteresis for the edge image
  void performHysteresis(int low, int high);
  // follow function
  void follow(int x1, int y1, int i1, int threshold);
  // normalization function
  void normalizeContrast();
  // hypotenuse function
  float hypotenuse(float x, float y);
  // gaussian filter
  float gaussian(float x, float sigma);
  // error function
  void catchErr(std::string error);

public:
  // constructor overload, canny's parameters are defined by user
  canny(CImg<unsigned char>& greyImg, float lowThreshold = 2.5f, float highthreshold = 7.5f, float gaussiankernelradius = 2.0f, int gaussiankernelwidth = 16, int contrastnormalised = 0);
  // get the final edge image
  CImg<unsigned char> getEdgeImg();
  // destructor
  ~canny();
};

#endif
