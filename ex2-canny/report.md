# Ex2 Package Canny Edge Detector Library

This project encapsulates the given source code of canny edge detector. Then we use the packaged library to test the four images and analyze how the parameters of canny detector affect the edge detection.

## Test Environment

+ Operating System: Ubuntu 16.04.1 LTS
+ Compiler: Linux version 4.4.0-116-generic (buildd@lgw01-amd64-021) (gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.9) )

## Test Data

Images:

+ ./images/bigben.bmp
+ ./images/lena.bmp
+ ./images/stpietro.bmp
+ ./images/twows.bmp

## Packaging

After packaging, we create two class, `imageIO` class for reading source images and outputting corresponding grey images, and `canny` class for detecting the edges. We only provide four interfaces to users, which work like this:

```c++
// read the source image
imageIO img(filename);
// get the grey image
CImg<unsigned char> grey = img.getGreyImg();
// canny detector
canny edge(grey, lowThreshold, highthreshold, gaussiankernelradius, gaussiankernelwidth, contrastnormalised);
// get edge image
CImg<unsigned char> rst = edge.getEdgeImg();
// display the edge image
rst.display();
```

We use the `CImg` Library to implement all the IO and other operations of images.

## Test Result
