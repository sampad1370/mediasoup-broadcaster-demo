#include "ffmpgDecoder.h"
#include <iostream>

using namespace std;
using namespace cv;

DecoderMp4::DecoderMp4(std::string filename)
{
    m_filename = filename;

    m_videoCapture = new VideoCapture(filename);
    // Check if camera opened successfully
    if(!m_videoCapture->isOpened()){
        cout << "Error opening video stream or file" << endl;
        exit(0);
    }
//    else
//    {
//        while(1){
//            // Display the resulting frame
//            imshow( "Frame", frame );
//            // Press  ESC on keyboard to exit
//            char c=(char)waitKey(25);
//            if(c==27)
//              break;
//          }
//          // When everything done, release the video capture object
//          // Closes all the frames
//          destroyAllWindows();

//    }
}

DecoderMp4::~DecoderMp4() {
    //Free();
    m_videoCapture->release();
    delete m_videoCapture;
    m_videoCapture=nullptr;
}

Mat DecoderMp4::getNextFrame()
{
    Mat frame;
    // Capture frame-by-frame
    (*m_videoCapture) >> frame;
    // If the frame is empty, break immediately
    if(frame.empty())
    {
        if(m_videoCapture->open(m_filename.c_str()))
        {
            cout << "Reopened file file" << endl;
            (*m_videoCapture) >> frame;
        }
        else
        {
            cout << "Can't reopened file file" << endl;
        }
    }
    return frame;
}
