#include "stitching.h"

int main() {
  /*
  CImg<float> srcA = CImg<float>("./images1/1.bmp");
  for (char i = '2'; i <= '4'; ++i) {
    char filename[100] = "./images1/0.bmp";
    filename[10] = i;
    CImg<float> srcB = CImg<float>(filename);
    sift s(srcA);
    vector<keyPoint*> kpsA = s.getKeyPoints();
    sift s2(srcB);
    vector<keyPoint*> kpsB = s2.getKeyPoints();
    match m(kpsA, kpsB, srcA, srcB);
    point matchedVec = m.getMatchedVec();
    stitch st(srcA, srcB, matchedVec.col, matchedVec.row);

    srcA = st.getStitchedImg();
  }
  */

  CImg<float> srcA = CImg<float>("./images2/1.JPG");
  for (char i = '2'; i <= '9'; ++i) {
    char filename[100] = "./images2/0.JPG";
    filename[10] = i;
    CImg<float> srcB = CImg<float>(filename);
    sift s(srcA);
    vector<keyPoint*> kpsA = s.getKeyPoints();
    sift s2(srcB);
    vector<keyPoint*> kpsB = s2.getKeyPoints();
    match m(kpsA, kpsB, srcA, srcB);
    point matchedVec = m.getMatchedVec();
    stitch st(srcA, srcB, matchedVec.col, matchedVec.row);

    srcA = st.getStitchedImg();
    char saveName[100] = "./images4/1s.jpg";
    saveName[10] = i;
    srcA.save(saveName);
  }
  for (char i = '0'; i <= '8'; ++i) {
    char filename[100] = "./images2/10.JPG";
    filename[11] = i;
    CImg<float> srcB = CImg<float>(filename);
    sift s(srcA);
    vector<keyPoint*> kpsA = s.getKeyPoints();
    sift s2(srcB);
    vector<keyPoint*> kpsB = s2.getKeyPoints();
    match m(kpsA, kpsB, srcA, srcB);
    point matchedVec = m.getMatchedVec();
    stitch st(srcA, srcB, matchedVec.col, matchedVec.row);

    srcA = st.getStitchedImg();
    char saveName[100] = "./images4/11s.jpg";
    saveName[11] = i;
    srcA.save(saveName);
  }

  return 0;
}
