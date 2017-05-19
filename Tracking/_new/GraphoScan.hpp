#pragma once

#define NAME_L ("video_Trajectoire.avi")
#define NAME_R ("video_Roi+Suivi.avi")
#define NAME_ALL ("video_Trajectoire+Roi+Suivi.avi")

#include <iostream>
#include <fstream>
#include <opencv2/tracking.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/*!
 * \class GraphoScan
 * \brief class for Tracking
 *
 * Class which use Tracking labrary to track on an object and create results files and coordinates points for 3D reconstruct
 */
class GraphoScan {
  
public:
  
  //look-up table: gamma correction
  uchar lut[256];

  //HOG paramater
  float fgamma;
  // y if you want to save video else n
  char saveVideo;
  // true for activate perspective
  bool isTransPerspective;

  cv::Size frameSize;

  // current region of interest
  vector<cv::Point2f> mRoi;
  
  // matrixes save during tracking
  cv::Mat imgSrc;
  cv::Mat imgTrajectoire;
  cv::Mat imgTrajectoire_cor;
  
  // matrixes save during HOG construct and Roi
  cv::Mat imgPtsObjet;
  cv::Mat imgRoi;
  cv::Mat M;

  //vector use for perspective
  vector<Point2f> modelPoint;
  vector<Point2f> modelImagePoint;

  // output video if saveVideo == true
  cv::VideoWriter outputVideo1;
  cv::VideoWriter outputVideo2;
  cv::VideoWriter outputVideo3;

  // Roi centers during tracking
  vector<Point2f> ptsObjet;

  /*!
   * \fn GraphoScan()
   * \brief Constructor
   * 
   * Class constructor initialize tracking context
   */
  GraphoScan(){
    init();
  };

  /*!
   * \fn ~GraphoScan()
   * \brief class destructor
   * finished video to save and save them
   */
  ~GraphoScan(){
    if ((saveVideo == 'y') || (saveVideo == 'Y')) {
      outputVideo1.release();
      std::cout << "outputVideo1 est bien relachee." << endl;
      outputVideo2.release();
      std::cout << "outputVideo2 est bien relachee." << endl;
      outputVideo3.release();
      std::cout << "outputVideo3 est bien relachee." << endl;
    }
  };

  /*!
   * \fn vector<Point2f> getPtsTrajectoire()
   * \brief getter ptsObjet parameter
   * 
   * Unused function seems
   *
   * \return ptsObjet type vector<Point2f>
   */
  vector<Point2f> getPtsTrajectoire();

  
  /*!
   * \fn cv::Mat getImgTrajectoire_Cor(vector<Point2f> ptsObjet)
   * \brief return HOG picture trajectory
   * 
   * Connect points of HOG trajectory and return matrix of it
   *
   * \return ptsObjet type vector<Point2f>
   */
  cv::Mat getImgTrajectoire_Cor(vector<Point2f> ptsObjet);

  /*!
   * \fn void calcImgPtsAndImgTrack()
   * \brief apply HOG on video
   * 
   * Apply HOG on video to obtain written word thanks to HOG contrast
   */
  void calcImgPtsAndImgTrack();

  /*!
   * \fn showImgTrackAndHog(string imgTraj, string imgTraj_cor)
   * \brief Show trajectories
   * 
   * Open with the trajectory of the algorithm applied and HOG trajectory
   *
   * \param imgTraj filename of tracking algortihm picture
   * \param imgTraj_cor filename of HOG picture
   */
  void showImgTrackAndHog(string imgTraj, string imgTraj_cor);

  /*!
   * \fn void saveImgPtsAndImgTraject(const char* filename_imgPtsObjet,const char* filename_imgTrajectoire_cor)
   * \brief save HOG and Trajectory pictures
   * 
   * Save HOG pictures in a file named filename_imgPtsObjet and Tracking picture in a file named filename_imgTrajectoire_cor
   *
   * \param filename_imgPtsObjet filename of tracking algortihm picture
   * \param filename_imgTrajectoire_cor filename of HOG picture
   */
  void saveImgPtsAndImgTraject(const char* filename_imgPtsObjet,const char* filename_imgTrajectoire_cor);

  /*!
   * \fn void saveTrajectoire(const char* filename)
   * \brief save coordinates points of Tracking trajectory
   * 
   * Save coordinates points of Tracking trajectory in a file named filename
   *
   * \param filename name of file to save points
   */
  void saveTrajectoire(const char* filename);

  /*!
   * \fn void saveTrajectoire(vector<cv::Point2f> pts, const char* filename)
   * \brief save coordinates points of pts parameter
   * 
   * Save coordinates points of pts parameter in a file named filename
   *
   * \param pts points to save in filename
   * \param filename name of file to save points
   */
  void saveTrajectoire(vector<cv::Point2f> pts, const char* filenName);

  /*!
   * \fn void readTracjectoire(const char* filename)
   * \brief read coordinates points in file name filename
   * 
   * Read coordinates points in the file named filename and save them in ptsObjet
   *
   * \param filename name of file which contains points
   */
  void readTracjectoire(const char* filename);

  /*!
   * NOT USED
   * \fn void mySelectBg(const char* fileName_video, const char* fileName_pt_bg)
   * \brief select workzone for tracking
   *
   * Permit to select work Zone from 4 points
   *
   * \param fileName_video filename of the used video
   * \param fileName_pt_bg filename of file to save work zone
   */
  void mySelectBg(const char* fileName_video, const char* fileName_pt_bg);

  /*!
   * \fn void myTrackerKCF(const char* filename, bool isTransPerspective = false)
   * \brief apply KCF algorithm on the video
   *
   * Select Roi on the Video and Apply KCF Algortihm to calculate coordinate points of tracking
   *
   * \param filename video name 
   * \param isTransPerspective if you want perspective or not
   */
  void myTrackerKCF(const char* filename, bool isTransPerspective = false);

  /*!
   * \fn void init_VideoWritter()
   * \brief initalize videoWritter
   *
   * Initialize videoWritter for saved videos at the end of the program
   */
  void init_VideoWritter();

  /*!
   * \fn void init()
   * \brief initalize lut parameter
   *
   * Initialize lut parameter use for gamma correction
   */
  void init();

  /*!
   * \fn cv::Mat transformPerspective(const cv::Mat& imgSrc)
   * \brief select a more precise tracking zone
   *
   * Select a more precise tracking zone and zoomed into
   *
   * \param imgSrc matrix of the video
   */
  cv::Mat transformPerspective(const cv::Mat& imgSrc);

  /*!
   * NOT USED
   * \fn void selectPointManuel(const char* filename)
   * \brief select a manuel points for 3D reconstruct
   *
   * Select manually tracking object
   *
   * \param filename name of file to save points
   */
  void selectPointManuel(const char* filename);

  /*!
   * \fn void insertPoints(int n = 4)
   * \brief insert n points between 3D points
   *
   * Insert n points between two points before 3D reconstruct in ptsObjet
   *
   * \param n number of points to insert
   */
  void insertPoints(int n = 4);

  /*!
   * \fn void insertPoints(int n = 4)
   * \brief insert n points between 3D points
   *
   * Insert n points between two points before 3D reconstruct int pts
   * 
   * \param pts vector to save new points
   * \param n number of points to insert
   */
  void insertPoints(vector<Point2f>& pts, int n = 4);

  /*!
   * \fn void calAndSavePointsOf3D(Size sizeImg, const char* filename_left, const char* filename_right, const char* filename_output, const char * filename_outputZ)
   * \brief calculate 3D points for 3D reconstruction
   *
   * Calculate 3D points from tracking points file of the two cameras and create the corresponding file
   * 
   * \param sizeImg video size
   * \param filename_left filename of camera left points file
   * \param filename_right filename of camera right points file
   * \param filename_output filename which contains 3D points
   * \param filename_outputZ filename which contains 3D points in depending on time
   */
  static void calAndSavePointsOf3D(Size sizeImg, const char* filename_left = "objectPoints_left.txt",const char* filename_right = "objectPoints_right.txt", const char* filename_output = "vCoordinates.txt",  const char * filename_outputZ="vCoordinatesZ.txt");
  
};

/*!
 * \fn void mouseSelectPoint(int event, int x, int y, int flags, void* userData)
 * \brief select points on video
 *
 * Insert n points between two points before 3D reconstruct int pts
 * 
 * \param event mouse event catched
 * \param x x coordinates of the mouse
 * \param y y coordinates of the mouse
 * \param flags 
 * \param usedData
 */
void mouseSelectPoint(int event, int x, int y, int flags, void* userData);
