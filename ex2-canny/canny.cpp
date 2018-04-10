//
//  canny.cpp
//  Canny Edge Detector
//
//  Packaged by Jack Cheng on 10/04/18.
//  All rights reserved.
//

#include "canny.h"
#include <cstdlib>
#include <iostream>

using namespace std;

#define ffabs(x) ( (x) >= 0 ? (x) : -(x) )
#define GAUSSIAN_CUT_OFF 0.005f
#define MAGNITUDE_SCALE 100.0f
#define MAGNITUDE_LIMIT 1000.0f
#define MAGNITUDE_MAX ((int) (MAGNITUDE_SCALE * MAGNITUDE_LIMIT))

// ----------------------imageIO class-----------------------------
// transform the source image into grey image
void imageIO::toGrey() {
  unsigned int width = srcImg.width(), height = srcImg.height();
  greyImg = CImg<unsigned char>(width, height, 1, 1);

  for (int i = 0; i < width; ++i) {
      for (int j = 0; j < height; ++j) {
          int b = srcImg(i, j, 0, 0);
          int g = srcImg(i, j, 0, 1);
          int r = srcImg(i, j, 0, 2);

          double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
          greyImg(i, j, 0, 0) = (unsigned char) newValue;
      }
  }
}

// constructor read the source image from the file
// param: filename name of the file
imageIO::imageIO(const char* filename) {
  srcImg = CImg<unsigned char>(filename);
  toGrey();
}

// get the source image
// return: CImg<unsigned char> the source image
CImg<unsigned char> imageIO::getSrcImg() { return srcImg; }

// get the grey image
// return: CImg<unsigned char> the grey image
CImg<unsigned char> imageIO::getGreyImg() { return greyImg; }
// ----------------------imageIO class-----------------------------

// ----------------------canny class-----------------------------
// allocate buffers for related variables
// param: grey grey image
void canny::allocatebuffers(unsigned char *grey) {
  data = new unsigned char[width * height];
  answer = new unsigned char[width * height];
  idata = new int[width * height];
  magnitude = new int[width * height];
  xConv = new float[width * height];
  yConv = new float[width * height];
  xGradient = new float[width * height];
  yGradient = new float[width * height];

  if (!data || !answer || !idata || !magnitude || !xConv || !yConv || !xGradient || !yGradient) {
    catchErr("fail to allocate buffers!");
  }

  memcpy(data, grey, width * height);
}

// release all the buffers allocated
void canny::killbuffers() {
  delete [] data;
  delete [] answer;
  delete [] idata;
  delete [] magnitude;
  delete [] xConv;
  delete [] yConv;
  delete [] xGradient;
  delete [] yGradient;
}

// compute the gradients of the grey image
// param: kernelRadius radius of gaussian kernel
// param: kernelWidth width of gaussian kernel
void canny::computeGradients(float kernelRadius, int kernelWidth) {
  float *kernel;
	float *diffKernel;
	int kwidth;

	int initX;
	int maxX;
	int initY;
	int maxY;

	int x, y;
	int i;
	int flag;

	kernel = new float[kernelWidth];
	diffKernel = new float[kernelWidth];
	if(!kernel || !diffKernel)
		catchErr("fail to create kernel");

	/* initialise the Gaussian kernel */
	for (kwidth = 0; kwidth < kernelWidth; kwidth++)
	{
		float g1, g2, g3;
		g1 = gaussian((float) kwidth, kernelRadius);
		if (g1 <= GAUSSIAN_CUT_OFF && kwidth >= 2)
			break;
		g2 = gaussian(kwidth - 0.5f, kernelRadius);
		g3 = gaussian(kwidth + 0.5f, kernelRadius);
		kernel[kwidth] = (g1 + g2 + g3) / 3.0f / (2.0f * (float) 3.14 * kernelRadius * kernelRadius);
		diffKernel[kwidth] = g3 - g2;
	}

	initX = kwidth - 1;
	maxX = width - (kwidth - 1);
	initY = width * (kwidth - 1);
	maxY = width * (height - (kwidth - 1));

	/* perform convolution in x and y directions */
	for(x = initX; x < maxX; x++)
	{
		for(y = initY; y < maxY; y += width)
		{
			int index = x + y;
			float sumX = data[index] * kernel[0];
			float sumY = sumX;
			int xOffset = 1;
			int yOffset = width;
			while(xOffset < kwidth)
			{
				sumY += kernel[xOffset] * (data[index - yOffset] + data[index + yOffset]);
				sumX += kernel[xOffset] * (data[index - xOffset] + data[index + xOffset]);
				yOffset += width;
				xOffset++;
			}

			yConv[index] = sumY;
			xConv[index] = sumX;
		}

	}

    for (x = initX; x < maxX; x++)
	{
		for (y = initY; y < maxY; y += width)
		{
			float sum = 0.0f;
			int index = x + y;
			for (i = 1; i < kwidth; i++)
				sum += diffKernel[i] * (yConv[index - i] - yConv[index + i]);

			xGradient[index] = sum;
		}

	}

	for(x = kwidth; x < width - kwidth; x++)
	{
		for (y = initY; y < maxY; y += width)
		{
			float sum = 0.0f;
			int index = x + y;
			int yOffset = width;
			for (i = 1; i < kwidth; i++)
			{
				sum += diffKernel[i] * (xConv[index - yOffset] - xConv[index + yOffset]);
				yOffset += width;
			}

			yGradient[index] = sum;
		}

	}

	initX = kwidth;
	maxX = width - kwidth;
	initY = width * kwidth;
	maxY = width * (height - kwidth);
	for(x = initX; x < maxX; x++)
	{
		for(y = initY; y < maxY; y += width)
		{
			int index = x + y;
			int indexN = index - width;
			int indexS = index + width;
			int indexW = index - 1;
			int indexE = index + 1;
			int indexNW = indexN - 1;
			int indexNE = indexN + 1;
			int indexSW = indexS - 1;
			int indexSE = indexS + 1;

			float xGrad = xGradient[index];
			float yGrad = yGradient[index];
			float gradMag = hypotenuse(xGrad, yGrad);

			/* perform non-maximal supression */
			float nMag = hypotenuse(xGradient[indexN], yGradient[indexN]);
			float sMag = hypotenuse(xGradient[indexS], yGradient[indexS]);
			float wMag = hypotenuse(xGradient[indexW], yGradient[indexW]);
			float eMag = hypotenuse(xGradient[indexE], yGradient[indexE]);
			float neMag = hypotenuse(xGradient[indexNE], yGradient[indexNE]);
			float seMag = hypotenuse(xGradient[indexSE], yGradient[indexSE]);
			float swMag = hypotenuse(xGradient[indexSW], yGradient[indexSW]);
			float nwMag = hypotenuse(xGradient[indexNW], yGradient[indexNW]);
			float tmp;
			/*
			 * An explanation of what's happening here, for those who want
			 * to understand the source: This performs the "non-maximal
			 * supression" phase of the Canny edge detection in which we
			 * need to compare the gradient magnitude to that in the
			 * direction of the gradient; only if the value is a local
			 * maximum do we consider the point as an edge candidate.
			 *
			 * We need to break the comparison into a number of different
			 * cases depending on the gradient direction so that the
			 * appropriate values can be used. To avoid computing the
			 * gradient direction, we use two simple comparisons: first we
			 * check that the partial derivatives have the same sign (1)
			 * and then we check which is larger (2). As a consequence, we
			 * have reduced the problem to one of four identical cases that
			 * each test the central gradient magnitude against the values at
			 * two points with 'identical support'; what this means is that
			 * the geometry required to accurately interpolate the magnitude
			 * of gradient function at those points has an identical
			 * geometry (upto right-angled-rotation/reflection).
			 *
			 * When comparing the central gradient to the two interpolated
			 * values, we avoid performing any divisions by multiplying both
			 * sides of each inequality by the greater of the two partial
			 * derivatives. The common comparand is stored in a temporary
			 * variable (3) and reused in the mirror case (4).
			 *
			 */
			flag = ( (xGrad * yGrad <= 0.0f) /*(1)*/
				? ffabs(xGrad) >= ffabs(yGrad) /*(2)*/
					? (tmp = ffabs(xGrad * gradMag)) >= ffabs(yGrad * neMag - (xGrad + yGrad) * eMag) /*(3)*/
						&& tmp > fabs(yGrad * swMag - (xGrad + yGrad) * wMag) /*(4)*/
					: (tmp = ffabs(yGrad * gradMag)) >= ffabs(xGrad * neMag - (yGrad + xGrad) * nMag) /*(3)*/
						&& tmp > ffabs(xGrad * swMag - (yGrad + xGrad) * sMag) /*(4)*/
				: ffabs(xGrad) >= ffabs(yGrad) /*(2)*/
					? (tmp = ffabs(xGrad * gradMag)) >= ffabs(yGrad * seMag + (xGrad - yGrad) * eMag) /*(3)*/
						&& tmp > ffabs(yGrad * nwMag + (xGrad - yGrad) * wMag) /*(4)*/
					: (tmp = ffabs(yGrad * gradMag)) >= ffabs(xGrad * seMag + (yGrad - xGrad) * sMag) /*(3)*/
						&& tmp > ffabs(xGrad * nwMag + (yGrad - xGrad) * nMag) /*(4)*/
				);
            if(flag)
			{
				magnitude[index] = (gradMag >= MAGNITUDE_LIMIT) ? MAGNITUDE_MAX : (int) (MAGNITUDE_SCALE * gradMag);
				/*NOTE: The orientation of the edge is not employed by this
				implementation. It is a simple matter to compute it at
				this point as: Math.atan2(yGrad, xGrad); */
			}
			else
			{
				magnitude[index] = 0;
			}
		}
	}
	delete kernel;
	delete diffKernel;
}

// perform hysteresis
// param: low the low threshold
// param: high the high threshold
void canny::performHysteresis(int low, int high) {
  int offset = 0;
  int x, y;

  memset(idata, 0, width * height * sizeof(int));

  for(y = 0; y < height; y++)
  {
    for(x = 0; x < width; x++)
    {
      if(idata[offset] == 0 && magnitude[offset] >= high)
      follow(x, y, offset, low);
      offset++;
    }
  }
}

// follow
// param: x1 the point's x position
// param: y1 the point's y position
// param: i1 the index
// param: threshold the threshold
void canny::follow(int x1, int y1, int i1, int threshold) {
  int x, y;
  int x0 = x1 == 0 ? x1 : x1 - 1;
  int x2 = x1 == width - 1 ? x1 : x1 + 1;
  int y0 = y1 == 0 ? y1 : y1 - 1;
  int y2 = y1 == height -1 ? y1 : y1 + 1;

  idata[i1] = magnitude[i1];
  for (x = x0; x <= x2; x++)
  {
    for (y = y0; y <= y2; y++)
    {
      int i2 = x + y * width;
      if ((y != y1 || x != x1) && idata[i2] == 0 && magnitude[i2] >= threshold)
        follow(x, y, i2, threshold);
    }
  }
}

// normalize the contrast of the grey image
void canny::normalizeContrast() {
  int histogram[256] = {0};
  int remap[256];
  int sum = 0;
  int j = 0;
  int k;
  int target;
  int i;

  for (i = 0; i < width * height; i++)
      histogram[data[i]]++;


  for (i = 0; i < 256; i++)
  {
      sum += histogram[i];
      target = (sum*255)/(width * height);
      for (k = j+1; k <= target; k++)
        remap[k] = i;
      j = target;
  }

  for (i = 0; i < width * height; i++)
    data[i] = remap[data[i]];
}

// get hypotenuse
// param: x one right-angle side
// param: y another right-angle side
// return: the value of the hypotenuse
float canny::hypotenuse(float x, float y) { return (float) sqrt(x*x +y*y); }

// get gaussian value
// param: x the original
// param: sigma sigma value
// return: the value after gaussian filtering
float canny::gaussian(float x, float sigma) { return (float) exp(-(x * x) / (2.0f * sigma * sigma)); }

// catch and print the error
// param: error error message
void canny::catchErr(std::string error) {
  cout << error << endl;
  killbuffers();
  exit(1);
}

// constructor
// param: greyImg the grey image
// param: lowThreshold the low threshold
// param: highthreshold the high threshold
// param: gaussiankernelradius the radius of gaussion kernel
// param: gaussiankernelwidth the width of gaussion kernel
// param: contrastnormalised true if normalize the contrast of the grey image
canny::canny(CImg<unsigned char>& greyImg, float lowThreshold, float highthreshold, float gaussiankernelradius, int gaussiankernelwidth, int contrastnormalised) {
  height = greyImg.height();
  width = greyImg.width();
  unsigned char * grey = greyImg.data();
  answer = 0;
  int low, high;
  int i;

  allocatebuffers(grey);

  if (contrastnormalised)
    normalizeContrast();

  computeGradients(gaussiankernelradius, gaussiankernelwidth);

  low = (int) (lowThreshold * MAGNITUDE_SCALE + 0.5f);
  high = (int) ( highthreshold * MAGNITUDE_SCALE + 0.5f);
  performHysteresis(low, high);

  for(i=0;i<width*height;i++)
    answer[i] = idata[i] > 0 ? 1 : 0;
}

// get the edge image
// return: the edge image of the grey image
CImg<unsigned char> canny::getEdgeImg() {
  CImg<unsigned char> rst(width, height, 1, 1);
  memcpy(rst._data, answer, width * height);
  return rst;
}

// destructor
canny::~canny() { killbuffers(); }

// ----------------------canny class-----------------------------
