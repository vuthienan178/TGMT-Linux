#pragma once
#include <vector>
#include <stdafx.h>
#include <image.h>
#include <handlingdefine.h>
struct CutImageInfo_t
{
    cv::Rect RectCut;
    char pathCutImage[_MAX_PATH];
    std::wstring NameFood;
};

class CImage
{

public:
    CImage();
    std::vector<CutImageInfo_t> m_listCutImage;
    std::vector<CutImageInfo_t> GetImageInfo() { return m_listCutImage; };
    void AddCutImage(CutImageInfo_t cutImage) { m_listCutImage.push_back(cutImage); };
    ~CImage();
};
