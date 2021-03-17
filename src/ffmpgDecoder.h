#pragma once
#define VIDEOCAPTURE_API

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <string>
#include <map>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/types_c.h>

class DecoderMp4
{
public:
    DecoderMp4(std::string filename);
    ~DecoderMp4();

    cv::Mat getNextFrame();
private:
    std::string m_filename;
    std::map<uint32_t,cv::Mat> m_frames;
    cv::VideoCapture* m_videoCapture;
};

