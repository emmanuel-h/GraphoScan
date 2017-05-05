#pragma once
#include <opencv2\opencv.hpp>
#include <iostream>
#include <fstream>

#define NUM_FILE 7

using namespace std;

class CameraCalibrator
{
public:
	CameraCalibrator();
	~CameraCalibrator();

	//setter
	void setCountOfImage(const int&);
	void setBoardSize(const cv::Size& boardSize);
	void setFilenames(const char* filename,int n);

	//getter
	cv::Mat getCameraMatrix();
	cv::Mat getDistCoeffs();

	//getter - stereo
	cv::Mat getCameraMatrix_left();
	cv::Mat getCameraMatrix_right();
	cv::Mat getDistCoeffs_left();
	cv::Mat getDistCoeffs_right();

	cv::Mat getR();
	cv::Mat getT();
	cv::Mat getE();
	cv::Mat getF();

	vector<cv::Mat> getRvecs();
	vector<cv::Mat> getTvecs();

	void findChessboardPoints_File();
	void findChessboardPoints_Stereo(const char* filename_left, const char* filename_right);

	void stereoCalibration(const char* filename,int n, const char* filename_left, const char* filename_right);

	void calibrate(const cv::Mat& );
	void rectifyMap(const cv::Mat&, cv::Mat&);
	
	bool isCalibrated();
	bool isPhotocatched();

	bool saveInnerParam(const char* filename);
	bool readInnerParam(const char* filename);

	bool saveStereoParam(const char* filename, cv::Mat matrix);

private:
	vector<cv::String> m_filename;
	cv::Size m_boardSize;
	vector<vector<cv::Point2f>> m_srcPoints;
	vector<vector<cv::Point3f>> m_dstPoints;
	cv::Mat m_distCoeffs;
	cv::Mat m_cameraMatrix;

	cv::Mat m_c1, m_c2;
	cv::Mat m_d1, m_d2;
	
	cv::Mat m_R, m_T, m_E, m_F;

	vector<cv::Mat> m_rvecs;
	vector<cv::Mat> m_tvecs;
	
	int m_count = 0;
	bool m_isCalibrated = false;
	bool m_isPhotocatched = false;

};

//using namespace cv;
//using namespace std;
//
//class CameraCalibrator {
//private:
//	vector<string> m_filenames;
//	Size m_borderSize;
//	vector<vector<Point2f> > m_srcPoints;
//	vector<vector<Point3f> > m_dstPoints;
//public:
//	void setFilename();
//	void setBorderSize(const Size &borderSize);
//	void addChessboardPoints();
//	void addPoints(const vector<Point2f> &imageCorners, const vector<Point3f> &objectCorners);
//	void calibrate(const Mat &src, Mat &dst);
//};


