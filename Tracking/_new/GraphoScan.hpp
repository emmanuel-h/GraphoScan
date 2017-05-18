#pragma once

#define NAME_L ("video_Trajectoire.avi")
#define NAME_R ("video_Roi+Suivi.avi")
#define NAME_ALL ("video_Trajectoire+Roi+Suivi.avi")

#include <iostream>
#include <fstream>

//#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
//#include <opencv2/videoio.hpp>
//#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class GraphoScan {
public:
  GraphoScan();
  ~GraphoScan();
  
  //look-up table: gamma correction
  uchar lut[256];
  
  float fgamma;
  char saveVideo;
  bool isTransPerspective;
  
  cv::Size frameSize;
  
  vector<Point2f> mRoi;
  
  // initialisé dans myTrackerKCF, ce sont des images vides de la taille de imgSrc
  cv::Mat imgSrc;
  cv::Mat imgTrajectoire;
  cv::Mat imgTrajectoire_cor;
  
  
  //image réalisée par HOG
  cv::Mat imgPtsObjet;
  cv::Mat imgRoi;
  cv::Mat M;
  
  vector<Point2f> modelPoint;
  vector<Point2f> modelImagePoint;

  cv::VideoWriter outputVideo1;
  cv::VideoWriter outputVideo2;
  cv::VideoWriter outputVideo3;

  // centre de toutes les ROI capturées pendant le tracking
  vector<Point2f> ptsObjet;

  //getter
  vector<Point2f> getPtsTrajectoire();

  //functions
  cv::Mat nettoyageImage(const cv::Mat& imgDst, vector<Point2f> ptsObjet, int eps);

  void calcImgPtsAndImgTrack();

  void showImgTrackAndHog(string imgTraj,string imgTraj_cor);
  
  void saveImgPtsAndImgTraject(const char* filename_imgPtsObjet,const char* filename_imgTrajectoire_cor);

  void saveTrajectoire(const char* filename);

  void saveTrajectoire(vector<cv::Point2f> pts, const char* filenName);

  void readTracjectoire(const char* filename);

  void readImages(const char* filename_imgPtsObjet,const char* filename_imgTrajectoire_cor);

  cv::Mat findPoints();

  void myGammaCorrection(cv::Mat& img, float fgamma);

  void mySelectBg(const char* fileName_video, const char* fileName_pt_bg);

  void myTrackerKCF(const char* filename, bool isTransPerspective = false);

  void myTrackerMatchTemplate(const char* filename, bool isTransPerspective = false, bool ifUpdateTemplate = false);

  void myMatchTemplate(cv::Mat& imgSrc, cv::Mat &model, Rect2d &trackBox, bool ifUpdateTemplate = false);

  void drawTrack();

  void init_VideoWritter()
  {
    std::cout << "Video will be saved" << endl;
    outputVideo1.open(NAME_L, CV_FOURCC('X', 'V', 'I', 'D'), 10, frameSize, true);
    if (!outputVideo1.isOpened()) {
      std::cout << "could not open the output video1 for write" << endl;
      return;
    }

    //problem here => outputVideo1 is bigger than outputVideo
    outputVideo2.open(NAME_R, CV_FOURCC('X', 'V', 'I', 'D'), 10, frameSize, true);
    if (!outputVideo2.isOpened()) {
      std::cout << "could not open the output video2 for write" << endl;
      return;
    }

    outputVideo3.open(NAME_ALL, CV_FOURCC('X', 'V', 'I', 'D'), 10, Size(frameSize.width * 2, frameSize.height), true);
    if (!outputVideo3.isOpened()) {
      std::cout << "could not open the output video3 for write" << endl;
      return;
    }
  }

  void init()
  {
    for (int i = 0; i < 256; i++)
      {
	lut[i] = saturate_cast<uchar>(pow((float)(i / 255.0f), fgamma)*255.0f);
      }
    mRoi.clear();
  }

  cv::Mat getImgTrajectoire_Cor(vector<Point2f> ptsObjet);

  cv::Mat transformPerspective(const cv::Mat& imgSrc);

  static void calAndSavePointsOf3D(Size sizeImg, const char* filename_left = "objectPoints_left.txt",
                                   const char* filename_right = "objectPoints_right.txt", const char* filename_output = "vCoordinates.txt",  const char * filename_outputZ="vCoordinatesZ.txt");

  void selectPointManuel(const char* filename);
	
  void insertPoints(int n = 4);

  void insertPoints(vector<Point2f>& pts, int n = 4);
  
};

void mouseSelectPoint(int event, int x, int y, int flags, void* userData);
