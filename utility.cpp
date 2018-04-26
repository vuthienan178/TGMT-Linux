//#include "stdafx.h"
#include "utility.h"
#include "handlingdefine.h"
#include "stdafx.h"
#include <unordered_map>
#include <baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
//#include <tchar.h>
#include <locale>
#include <codecvt>
extern std::unordered_map<wchar_t, unsigned short> MapPrice;
extern std::unordered_map<wchar_t, unsigned short> MapFoodName;


using namespace cv;

CUtility::CUtility()
{
}


CUtility::~CUtility()
{
}

void CUtility::GetBlocksImage()
{
    //Mat large = imread(INPUT_FILE);
    //Mat rgb;
    //int nIdxCut = 0;
    //// downsample and use it for processing
    //pyrDown(large, rgb);
    //Mat small;
    //cvtColor(rgb, small, CV_BGR2GRAY);
    //// morphological gradient
    //Mat grad;
    //Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    //morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
    //// binarize
    //Mat bw;
    //threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
    //// connect horizontally oriented regions
    //Mat connected;
    //morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
    //morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
    //// find contours
    //Mat mask = Mat::zeros(bw.size(), CV_8UC1);
    //std::vector<std::vector<Point>> contours;
    //std::vector<Vec4i> hierarchy;
    //findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    //// filter contours
    //for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
    //{

    //	Rect rect = boundingRect(contours[idx]);
    //	Mat maskROI(mask, rect);
    //	maskROI = Scalar(0, 0, 0);
    //	// fill the contour
    //	drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
    //	// ratio of non-zero pixels in the filled region
    //	double r = (double)countNonZero(maskROI) / (rect.width*rect.height);

    //	if (r > .90 /* assume at least 45% of the area is filled if it contains text */
    //		&&(rect.height > 50 && rect.width > 50) /* constraints on region size */)
    //	{
    //		rectangle(rgb, rect, Scalar(0, 255, 0), 1);
    //	}
    //}
    //imwrite(std::string(OUTPUT_FILE), rgb);

    // Load image
    Mat orig = imread(INPUT_FILE, IMREAD_COLOR);

    std::vector<int> PNGwriteOptions;
    PNGwriteOptions.push_back(IMWRITE_PNG_COMPRESSION);
    PNGwriteOptions.push_back(9);

    // Get greyscale and Otsu-thresholded version
    Mat bw, grey;
    cvtColor(orig, grey, COLOR_RGB2GRAY);
    threshold(grey, bw, 0, 255, THRESH_BINARY | THRESH_OTSU);

    // Find vertical centreline by looking for lowest column average - i.e. darkest vertical bar
    Mat colsums;
    reduce(bw, colsums, 0, REDUCE_AVG);
    double min, max;
    Point min_loc, max_loc;
    minMaxLoc(colsums, &min, &max, &min_loc, &max_loc);
    std::cout << "Centreline at column: " << min_loc.x << std::endl;

    namedWindow("test", WINDOW_AUTOSIZE);

    // Split image into left and right
    Rect leftROI(0, 0, min_loc.x, bw.rows);
    Mat  leftbw = bw(leftROI);
    Rect rightROI(min_loc.x + 8, 0, bw.cols - min_loc.x - 8, bw.rows);
    Mat  rightbw = bw(rightROI);
    imshow("test", leftbw);
    waitKey(0);
    imshow("test", rightbw);
    waitKey(0);

    // Trim surrounding whitespace off
    Mat Points;
    Mat inverted = cv::Scalar::all(255) - leftbw;
    findNonZero(inverted, Points);
    Rect bRect = boundingRect(Points);
    Mat lefttrimmed = leftbw(bRect);

    inverted = cv::Scalar::all(255) - rightbw;
    findNonZero(inverted, Points);
    bRect = boundingRect(Points);
    Mat righttrimmed = rightbw(bRect);

    imwrite("lefttrimmed.png", lefttrimmed, PNGwriteOptions);
    imwrite("righttrimmed.png", righttrimmed, PNGwriteOptions);

    // Box filter with 55x45 rectangle to match size of paragraph indent on left
    Mat lBoxFilt, rBoxFilt;
    boxFilter(lefttrimmed, lBoxFilt, -1, Size(55, 45));
    normalize(lBoxFilt, lBoxFilt, 0, 255, NORM_MINMAX, CV_8UC1);
    threshold(lBoxFilt, lBoxFilt, 254, 255, THRESH_BINARY_INV);
    imwrite("leftBoxed.png", lBoxFilt, PNGwriteOptions);
}

std::wstring CUtility::ReadTextImage(cv::Mat imgRGB, int idx)
{
    //longNH26

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    std::wstring wide = converter.from_bytes(ImageToText(imgRGB));
    if (wide.find(L"\n\n") != std::wstring::npos)
    {
        wide.replace(wide.end() - 1 ,wide.end(),L"");
    }
    std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);
    return wide;
}

void CUtility::CutImage(CImage& imageInfo)
{
    Mat large = imread(INPUT_FILE);
    Mat rgb;
    int nIdxCut = 0;
    // downsample and use it for processing
    pyrDown(large, rgb);
    Mat small;
    cvtColor(rgb, small, CV_BGR2GRAY);
    // morphological gradient
    Mat grad;
    Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
    // binarize
    Mat bw;
    threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
    // connect horizontally oriented regions
    Mat connected;
    morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
    morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
    // find contours
    Mat mask = Mat::zeros(bw.size(), CV_8UC1);
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(connected, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));
    // filter contours
    for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
    {

        Rect rect = boundingRect(contours[idx]);
        Mat maskROI(mask, rect);
        maskROI = Scalar(0, 0, 0);
        // fill the contour
        drawContours(mask, contours, idx, Scalar(255, 255, 255), FILLED);
        // ratio of non-zero pixels in the filled region
        double r = (double)countNonZero(maskROI) / (rect.width*rect.height);

        if (r > .40 /* assume at least 45% of the area is filled if it contains text */
            &&
            (rect.height > 8 && rect.width > 8) /* constraints on region size */
                                                /* these two conditions alone are not very robust. better to use something
                                                like the number of significant peaks in a horizontal projection as a third condition */
            )
        {
            Rect rectConvert;
            rectConvert = rect;
            float idxConvert = 0;
            CutImageInfo_t cutImage;

            idxConvert = (float)large.rows / (float)rgb.rows;
            rectConvert.x = rectConvert.x*idxConvert;
            rectConvert.y = rectConvert.y*idxConvert;
            rectConvert.width = rectConvert.width *idxConvert;
            rectConvert.height = rectConvert.height *idxConvert;

            char pathFile[_MAX_PATH] = CUT_FOLDER;
            strcat(pathFile, CUT_IMGE);
            std::string sIdx = std::to_string(nIdxCut);
            strcat(pathFile, sIdx.c_str());
            strcat(pathFile, ".jpg");

            Mat Handling = imread(INPUT_FILE);
            Mat CropIMG = Handling(rectConvert);

            strcpy(cutImage.pathCutImage, pathFile);
            cutImage.RectCut = rectConvert;
            imageInfo.AddCutImage(cutImage);

            Mat im_gray;
            cvtColor(CropIMG, im_gray, COLOR_RGB2GRAY);
            Mat img_bw = im_gray > 128;
            imwrite(pathFile, img_bw);
            nIdxCut++;

            rectangle(rgb, rect, Scalar(0, 255, 0), 1);
        }
    }
    imwrite( std::string(OUTPUT_FILE), rgb);
}

void CUtility::RectangleImage()
{

}


char* CUtility::ImageToText(Mat Image)
{
    char* Result;
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(NULL, "vie"))
    {
        fprintf(stderr, "Could not initialize tesseract.\n");
    }
    api->SetImage((uchar*)Image.data, (size_t)Image.size().width, (size_t)Image.size().height, (size_t)Image.channels(), (size_t)Image.step1());
    api->Recognize(0);
    //Get OCR Result
    Result = api->GetUTF8Text();
    delete api;
    return Result;
}

void CUtility::ReadTextOfList(CImage& imageInfo)
{
    for (int i = 0; i < imageInfo.m_listCutImage.size(); i++)
    {
        Mat idxImage = imread(imageInfo.m_listCutImage[i].pathCutImage);
        if (idxImage.data)
        {
            imageInfo.m_listCutImage[i].NameFood = ReadTextImage(idxImage, i);
        }
    }
}

void CUtility::MergeMenu(CImage& imageInfo)
{
    for (int i = 0; i < imageInfo.m_listCutImage.size(); i++)
    {
        if (MapPrice[imageInfo.m_listCutImage[i].NameFood[0]] > 1 && MapPrice[imageInfo.m_listCutImage[i].NameFood[0]] <=15)
        {
            Case1(imageInfo);
            return;
        }
    }

}

void CUtility::Case2(CImage& imageInfo)
{

}

void CUtility::Case1(CImage& imageInfo)
{
    CImage ListFood, ListPrice;
    std::vector<FinalMenu> final;

    HandlingListText(imageInfo, ListFood, ListPrice);

    for (size_t i = 0; i < ListFood.m_listCutImage.size(); i++)
    {
        int nIdxPrice = -1;
        FinalMenu temp;
        std::vector<CutImageInfo_t> itemPrice;

        temp.NameFood = ListFood.m_listCutImage[i].NameFood;

        for (int j = 0; j < ListPrice.m_listCutImage.size(); j++)
        {
            int centerfood, centerPrice;
            int FoodYHeigt, PriceYHeigt;
            int FoodY, PriceY;
            centerfood = (ListFood.m_listCutImage[i].RectCut.y + ListFood.m_listCutImage[i].RectCut.y + ListFood.m_listCutImage[i].RectCut.height) / 2;
            FoodYHeigt = ListFood.m_listCutImage[i].RectCut.y + ListFood.m_listCutImage[i].RectCut.height;
            FoodY = ListFood.m_listCutImage[i].RectCut.y;

            centerPrice = (ListPrice.m_listCutImage[j].RectCut.y + ListPrice.m_listCutImage[j].RectCut.y + ListPrice.m_listCutImage[j].RectCut.height) / 2;
            PriceYHeigt = ListPrice.m_listCutImage[j].RectCut.y + ListPrice.m_listCutImage[j].RectCut.height;
            PriceY = ListPrice.m_listCutImage[j].RectCut.y;

            if (centerfood <= PriceYHeigt && centerfood >= PriceY)
            {
                itemPrice.push_back(ListPrice.m_listCutImage[j]);
                if (ListPrice.m_listCutImage[j].RectCut.x > ListFood.m_listCutImage[i].RectCut.x)
                {
                    temp.Price = ListPrice.m_listCutImage[j].NameFood;
                    nIdxPrice = ListPrice.m_listCutImage[j].RectCut.x;
                }
            }
        }


        for (int l = 0; l < itemPrice.size(); l++)
        {
            if (itemPrice[l].RectCut.x >  ListFood.m_listCutImage[i].RectCut.x && itemPrice[l].RectCut.x < nIdxPrice)
            {
                temp.Price = itemPrice[l].NameFood;
                nIdxPrice = itemPrice[l].RectCut.x;
            }
        }
        final.push_back(temp);
    }
    WriteFile(final);
}

void CUtility::WriteFile(std::vector<FinalMenu> Menu)
{
    std::wofstream fout("final.txt", std::ofstream::out);
    for (int i = 0; i < Menu.size(); i++)
    {
        if (Menu[i].Price != L"" && Menu[i].NameFood != L"")
        {
            std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);
            fout.imbue(loc);
            fout << Menu[i].NameFood;

            fout << Menu[i].Price;

            fout << L"\n----------------\n";

        }
    }
    fout.close();
}

#define DEF_VND L'\x111'
void CUtility::HandlingListText(CImage& imageInfo, CImage& ListFood, CImage& ListPrice)
{
    for (int i = 0; i < imageInfo.m_listCutImage.size(); i++)
    {
        std::wstring food = imageInfo.m_listCutImage[i].NameFood;
        int a = food.length();
        const wchar_t sVnD = DEF_VND;
        const wchar_t sVnK = L'K';
        if (food[a - 2] == sVnK || food[a - 2] == sVnD)
        {
            ListPrice.AddCutImage(imageInfo.m_listCutImage[i]);
        }
        else
        {
            ListFood.AddCutImage(imageInfo.m_listCutImage[i]);
        }
    }
}
