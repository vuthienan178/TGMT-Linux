#include "stdafx.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
//#include <tchar.h>
using namespace std;
using namespace cv;

#define IMG_STEEP_1	"OutPut\\OutPut2.jpg"
#define POS_X 29
#define POS_Y 115
#define HEIGHT 25
#define WIDTH 121
void main2()
{
    Mat Handling = imread(IMG_STEEP_1);

    Rect RectCrop(POS_X, POS_Y, WIDTH, HEIGHT);

    Mat CropIMG = Handling(RectCrop);
    imshow("OutPut2", CropIMG);
    cv::waitKey();
    getchar();
}
