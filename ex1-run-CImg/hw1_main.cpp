#include "hw1.h"
#include <iostream>

using namespace std;

int main() {
    // read the image
    CImg<unsigned char> img("1.bmp");
    // receive the task and do the operation
    while (true) {
        cout << "+---------------------------------+" << endl;
        cout << "|     My Little CImg Program      |" << endl;
        cout << "+---------------------------------+" << endl;
        cout << endl;
        cout << "      0. Exit                      " << endl;
        cout << "      1. Display the image         " << endl;
        cout << "      2. Change color of some area " << endl;
        cout << "      3. Paint a blue circle       " << endl;
        cout << "      4. Paint a yellow circle     " << endl;
        cout << endl;
        cout << "Please enter your choice (0-4): " << endl;

        int task;
        cin >> task;

        switch(task) {
            case 0:
                return 0;
            case 1:
                task1(img);
                break;
            case 2:
                task2(img);
                break;
            case 3:
                task3(img);
                break;
            case 4:
                task4(img);
                break;
            default:
                cout << "no such choice! Please enter again!" << endl;
                break;
        }
    }

    return 0;
}
