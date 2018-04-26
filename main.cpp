#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <tesseract-ocr/api/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <fstream>
#include "handlingdefine.h"
#include "image.h"
#include "utility.h"
#include <locale>
#include <codecvt>
using namespace std;
using namespace cv;

int main()
{
    CUtility Utility;
    CImage imageInfo;

    //Utility.GetBlocksImage();
    Utility.CutImage(imageInfo);
    Utility.ReadTextOfList(imageInfo);
    Utility.MergeMenu(imageInfo);
    return 0;
}
