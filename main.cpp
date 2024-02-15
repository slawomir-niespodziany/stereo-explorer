#include <iostream>
#include <string>
#include "model.h"
#include "view.h"

using namespace std::string_literals;

int main(int argc, char* argv[]) {
    std::string pathImgLeft, pathImgRight, pathDepth;

    if (argc > 3) {
        pathImgLeft = argv[1];
        pathImgRight = argv[2];
        pathDepth = argv[3];

    } else {
        pathImgLeft = "../Motorcycle/im0.png"s;
        pathImgRight = "../Motorcycle/im1.png"s;
        pathDepth = "../Motorcycle/disp0GT.pfm"s;
    }

    Model model(pathImgLeft, pathImgRight, pathDepth, 11, 255);

    View::getInstance().loop(model);

    return 0;
}