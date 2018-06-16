#include "stitching.h"

void run(CImg<float>& srcA, char start, char end, char images, bool over) {
  char filename[100] = "./images1/1.JPG";
  if (images == '1') { filename[12] = 'b'; filename[13] = 'm'; filename[14] = 'p'; }
  filename[8] = images;
  for (char i = start; i <= end; ++i) {
    if (!over) { filename[10] = i; }
    else { filename[11] = i; filename[12] = '.'; filename[13] = 'J'; filename[14] = 'P'; filename[15] = 'G'; }
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
}

int main() {
  while (true) {
    cout << "+---------------------------------+" << endl;
    cout << "|    My image stitching Program   |" << endl;
    cout << "+---------------------------------+" << endl;
    cout << endl;
    cout << "      0. Exit                      " << endl;
    cout << "      1. test first dataset        " << endl;
    cout << "      2. test second dataset       " << endl;
    cout << endl;
    cout << "Please choose your dataset (0-2): " << endl;

    int choice;
    cin >> choice;
    cin.get();

    switch (choice) {
      case 0:
        return 0;
      case 1:
        CImg<float> srcA("./images1/1.bmp");
        run(srcA, '2', '4', '1', false);
        srcA.display();
        srcA.save("./result/test1.jpg");
        break;
      case 2:
        CImg<float> srcA("./images2/1.JPG");
        run(srcA, '2', '9', '2', false);
        run(srcA, '0', '8', '2', true);
        srcA.display();
        srcA.save("./result/test2.jpg");
        break;
      default:
        cout << "no such image! Please enter again!" << endl;
        break
    }
  }

  return 0;
}
