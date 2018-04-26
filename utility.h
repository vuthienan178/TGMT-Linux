#pragma once
#include<stdafx.h>
#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
//#include <tchar.h>
#include "image.h"

struct FinalMenu
{
    std::wstring NameFood;
    std::wstring Price;
};

class CUtility
{
public:
    CUtility();
    void CutImage(CImage& imageInfo);
    std::wstring ReadTextImage(cv::Mat imgGray, int idx);
    void RectangleImage();
    char* ImageToText(cv::Mat Image);
    void ReadTextOfList(CImage& imageInfo);
    void MergeMenu(CImage& imageInfo);
    void HandlingListText(CImage& imageInfo, CImage& ListFood, CImage& ListPrice);
    void WriteFile(std::vector<FinalMenu>);
    void GetBlocksImage();
    void Case1(CImage& imageInfo);
    void Case2(CImage& imageInfo);
    ~CUtility();
};
