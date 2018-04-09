//
//  canny.h
//  Canny Edge Detector
//
//  Packaged by Jack Cheng on 10/04/18.
//  All rights reserved.
//

#ifndef _CANNY_
#define _CANNY_

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
  imagesIO(char* filename);
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
  int width;
  int height;
  int *idata;            /* output for edges */
  int *magnitude;        /* edge magnitude as detected by Gaussians */
  float *xConv;          /* temporary for convolution in x direction */
  float *yConv;          /* temporary for convolution in y direction */
  float *xGradient;      /* gradients in x direction, as detected by Gaussians */
  float *yGradient;      /* gradients in x direction,a s detected by Gaussians */


};

#endif
