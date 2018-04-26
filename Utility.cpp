//#include "stdafx.h"
#include "Utility.h"
#include "HandlingDefine.h"
#include "stdafx.h"

#include <lib\tesseract\include\tesseract\baseapi.h>
#include <lib\tesseract\include\leptonica\allheaders.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include <locale>
#include <codecvt>


using namespace cv;

CUtility::CUtility()
{
}


CUtility::~CUtility()
{
}

std::wstring CUtility::SetImageBackWhite(cv::Mat imgRGB, int idx)
{
	char pathFile[_MAX_PATH] = DEF_PATH_BLACK_WHITE;
	strcat(pathFile, BLACK_WHITE_IMAGE);
	std::string sIdx = std::to_string(idx);
	strcat(pathFile, sIdx.c_str());
	strcat(pathFile, ".jpg");

	Mat im_gray;
	cvtColor(imgRGB, im_gray, CV_RGB2GRAY);
	Mat img_bw = im_gray > 128;
	imwrite(pathFile, img_bw);


	//longNH26
	
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	std::wstring wide = converter.from_bytes(ImageToText(img_bw));

	std::wofstream fout("new.txt", std::ofstream::app);
	std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);
	fout.imbue(loc);
	fout << wide;
	fout.close();
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
	findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	// filter contours
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{

		Rect rect = boundingRect(contours[idx]);
		Mat maskROI(mask, rect);
		maskROI = Scalar(0, 0, 0);
		// fill the contour
		drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
		// ratio of non-zero pixels in the filled region
		double r = (double)countNonZero(maskROI) / (rect.width*rect.height);

		if (r > .45 /* assume at least 45% of the area is filled if it contains text */
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

			idxConvert = large.rows / rgb.rows;
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

			imwrite(pathFile, CropIMG);
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