#include "CameraCalibrator.h"

using namespace std;

void CameraCalibrator::findChessboardPoints_File()
{
	for (int i = 0; i < m_filename.size(); i++)
	{
		vector<cv::Point2f> srcVecCorners;
		cv::Mat img = cv::imread(m_filename[i],CV_LOAD_IMAGE_GRAYSCALE);
		if (img.empty())
		{
			std::cout << "Cannot open the picture. Src." << i << endl;
			exit(1);
		}
		std::cout << m_filename[i] << " has been read successfully" << endl;

		//img = img(cv::Rect2d(0, 0, img.cols / 2.f, img.rows));
		img = img(cv::Rect2d(img.cols/2.f, 0, img.cols / 2.f, img.rows));

		//while (true)
		//{
		//	imshow(m_filename[i], img);
		//	if (cv::waitKey(30) == 27)
		//	{
		//		break;
		//	}
		//}

		bool found = cv::findChessboardCorners(img, m_boardSize, srcVecCorners);
		if (found == false)
		{
			cout << "Cannot find the chessboard"<< i << endl;
			exit(1);
		}

		vector<cv::Point3f> dstVecCorners;
		for (int i = 0; i<m_boardSize.height; i++) {
			for (int j = 0; j<m_boardSize.width; j++) {
				dstVecCorners.push_back(cv::Point3f(i, j, 0.0f));
			}
		}

		cv::TermCriteria param(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 0.1);
		cv::cornerSubPix(img, srcVecCorners, cv::Size(5, 5), cv::Size(-1, -1), param);
		if (srcVecCorners.size() == m_boardSize.area())
		{
			m_srcPoints.push_back(srcVecCorners);
			m_dstPoints.push_back(dstVecCorners);
		}
	}
	calibrate(cv::imread(m_filename[0]));
}

void CameraCalibrator::findChessboardPoints_Stereo(const char* filename_left, const char* filename_right)
{
	vector<vector<cv::Point2f>> vecCorners_left, vecCorners_right;
	cv::Mat imgSrc;
	for (int i = 0; i < m_filename.size(); i++)
	{
		vector<cv::Point2f> srcVecCorners_left, srcVecCorners_right;
		cv::Mat img = cv::imread(m_filename[i], CV_LOAD_IMAGE_GRAYSCALE);
		if (img.empty())
		{
			std::cout << "Cannot open the picture. Src." << i << endl;
			exit(1);
		}
		std::cout << m_filename[i] << " has been read successfully" << endl;

		cv::Mat img_left = img(cv::Rect2d(0, 0, img.cols / 2.f, img.rows));
		imgSrc = img_left.clone();
		//while (true)
		//{
		//	imshow(m_filename[i], img);
		//	if (cv::waitKey(30) == 27)
		//	{
		//		break;
		//	}
		//}

		bool found = cv::findChessboardCorners(img_left, m_boardSize, srcVecCorners_left);
		if (found == false)
		{
			cout << "<left>Cannot find the chessboard" << i << endl;
			exit(1);
		}

		cv::Mat img_right = img(cv::Rect2d(img.cols/2.0f, 0, img.cols / 2.f, img.rows));

		found = cv::findChessboardCorners(img_right, m_boardSize, srcVecCorners_right);
		if (found == false)
		{
			cout << "<right>Cannot find the chessboard" << i << endl;
			exit(1);
		}

		vector<cv::Point3f> dstVecCorners;
		for (int i = 0; i<m_boardSize.height; i++) {
			for (int j = 0; j<m_boardSize.width; j++) {
				dstVecCorners.push_back(cv::Point3f(i, j, 0.0f));
			}
		}

		cv::TermCriteria param(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 0.1);
		cv::cornerSubPix(img, srcVecCorners_left, cv::Size(5, 5), cv::Size(-1, -1), param);
		if ((srcVecCorners_left.size() == m_boardSize.area())&& (srcVecCorners_right.size() == m_boardSize.area()))
		{
			vecCorners_left.push_back(srcVecCorners_left);
			vecCorners_right.push_back(srcVecCorners_right);
			m_dstPoints.push_back(dstVecCorners);
		}
		else
		{
			std::cout << "srcVecorners.size() != m_boardSize.area()." << endl;
			exit(1);
		}
	}

	readInnerParam(filename_left);
	m_c1 = getCameraMatrix();
	m_d1 = getDistCoeffs();

	readInnerParam(filename_right);
	m_c2 = getCameraMatrix();
	m_d2 = getDistCoeffs();

	cv::stereoCalibrate(m_dstPoints, vecCorners_left, vecCorners_right, m_c1, m_d1, m_c2, m_d2, imgSrc.size(), m_R, m_T, m_E, m_F);
	std::cout << "m_R = " << m_R << endl;
	std::cout << "m_T = " << m_T << endl;
	std::cout << "m_E = " << m_E << endl;	
	std::cout << "m_F = " << m_F << endl;

	saveStereoParam("matrix_R.txt", m_R);
	saveStereoParam("matrix_T.txt", m_T);

}

void CameraCalibrator::stereoCalibration(const char* filename_pic, int n, const char* filename_left, const char* filename_right)
{
	setFilenames(filename_pic, n);
	findChessboardPoints_Stereo(filename_left, filename_right);
}

void CameraCalibrator::setCountOfImage(const int &CountOfImage)
{
	if (CountOfImage <= 0)
	{
		std::cout << "The count of input pictures is zero or negative." << endl;
		exit(1);
	}

	m_count = CountOfImage;
}

void CameraCalibrator::setBoardSize(const cv::Size& boardSize)
{
	m_boardSize = boardSize;
}

void CameraCalibrator::calibrate(const cv::Mat& imgSrc)
{

	cv::Size imageSize = imgSrc.size();
	cv::calibrateCamera(m_dstPoints, m_srcPoints, imageSize, m_cameraMatrix, m_distCoeffs, m_rvecs, m_tvecs);
	m_isCalibrated = true;
	std::cout << "camrea calibrated." << endl;
	std::cout << "Coeffients of the distortion" << endl << " " << m_distCoeffs << endl;
	std::cout << "Camera Matrix" << endl << " " << m_cameraMatrix << endl;
	//cout << "Rotation vector = " << endl << " " << m_rvecs[0] << endl;
	//cout << "Translation vector = " << endl << " " << m_tvecs[0] << endl;
}

void CameraCalibrator::setFilenames(const char* filename, int n)
{
	cv::String fileName;
	m_filename.clear();
	for (int i = 0; i <= n; i++)
	{
		stringstream ss;
		ss << i;
		fileName.clear();
		//fileName += "left";
		fileName += filename;
		fileName += ss.str();
		fileName += ".jpg";
		m_filename.push_back(fileName);
	}
}

cv::Mat CameraCalibrator::getCameraMatrix()
{
	return m_cameraMatrix;
}

cv::Mat CameraCalibrator::getDistCoeffs()
{
	return m_distCoeffs;
}

cv::Mat CameraCalibrator::getCameraMatrix_left()
{
	return m_c1;
}

cv::Mat CameraCalibrator::getCameraMatrix_right()
{
	return m_c2;
}

cv::Mat CameraCalibrator::getDistCoeffs_left()
{
	return m_d1;
}

cv::Mat CameraCalibrator::getDistCoeffs_right()
{
	return m_d2;
}

cv::Mat CameraCalibrator::getR()
{
	return m_R;
}

cv::Mat CameraCalibrator::getT()
{
	return m_T;
}

cv::Mat CameraCalibrator::getE()
{
	return m_E;
}

cv::Mat CameraCalibrator::getF()
{
	return m_F;
}

vector<cv::Mat> CameraCalibrator::getRvecs()
{
	return m_rvecs;
}

vector<cv::Mat> CameraCalibrator::getTvecs()
{
	return m_tvecs;
}

void CameraCalibrator::rectifyMap(const cv::Mat &imgSrc, cv::Mat &imgDst)
{
	cv::Size imageSize = imgSrc.size();
	cv::Mat map1, map2;
	cv::initUndistortRectifyMap(m_cameraMatrix, m_distCoeffs, cv::Mat(), cv::Mat(), imageSize, CV_32F, map1, map2);
	cv::remap(imgSrc, imgDst, map1, map2, cv::INTER_LINEAR);
	std::cout << "Map rectified." << endl;
}

bool CameraCalibrator::isCalibrated()
{
	return m_isCalibrated;
}

bool CameraCalibrator::isPhotocatched()
{
	return m_isPhotocatched;
}

bool CameraCalibrator::saveInnerParam(const char* filename)
{
	bool isSaved = false;
	
	ofstream fout(filename);
	if (!fout.is_open())
	{
		std::cout << "Cannot create the file " << filename << "." << endl;
		
	}
	else
	{
		fout.clear();
		std::cout << "saving camera matrix..." << endl;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				long double* tmp = m_cameraMatrix.ptr<long double>(i);
				std::cout << "tmp[" << j << "] = " << tmp[j] << endl;
				fout << tmp[j] <<" ";
			}
			fout << endl;
		}
		std::cout << "camera matrix saved" << endl;
		std::cout << "saving distortion coeffients..." << endl;
		for (int i = 0; i < 5; i++)
		{
			long double* tmp = m_distCoeffs.ptr<long double>(0);
			fout << tmp[i] << " ";
		}
		std::cout << "distortion coeffients saved..." << endl;
		std::cout << "The parameters of camera have been saved at " << filename << endl;
		fout.close();
		isSaved = true;
	}
	return isSaved;
}

bool CameraCalibrator::readInnerParam(const char* filename)
{
	bool isRead = false;
	ifstream fin(filename);
	if (!fin.is_open())
	{
		std::cout << "Cannot open the file " << filename << "." << endl;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (fin.good())
				{
					long double* tmp = m_cameraMatrix.ptr<long double>(i);
					fin >> tmp[j];
				}
				else
				{
					break;
				}
			}
		}
		for (int i = 0; i < 5; i++)
		{
			if (fin.good())
			{
				long double* tmp = m_distCoeffs.ptr<long double>(0);
				fin >> tmp[i];
			}
			else
			{
				break;
			}
		}
		std::cout << "Read cameraMatrix: " << endl << " " << m_cameraMatrix << endl;
		std::cout << "Read distortion coeffients: " << endl << " " << m_distCoeffs << endl;
		isRead = true;
	}
	return isRead;
}

bool CameraCalibrator::saveStereoParam(const char * filename, cv::Mat matrix)
{
	ofstream fout(filename);
	if (!fout.is_open())
	{
		std::cout << "Cannot create the file " << filename << "." << endl;
		return false;
	}
	else
	{
		fout.clear();
		std::cout << "saving matrix..." << endl;
		for (int i = 0; i < matrix.rows; i++)
		{
			for (int j = 0; j < matrix.cols; j++)
			{
				long double* tmp = matrix.ptr<long double>(i);
				std::cout << "tmp[" << j << "] = " << tmp[j] << endl;
				fout << tmp[j] << " ";
			}
			fout << endl;
		}
		fout.close();
	}
	return true;
}

CameraCalibrator::CameraCalibrator()
{
	m_cameraMatrix = cv::Mat(3, 3, CV_64FC1);
	m_distCoeffs = cv::Mat(1, 5, CV_64FC1);
}

CameraCalibrator::~CameraCalibrator()
{
}


//
//void CameraCalibrator::setFilename() {
//	m_filenames.clear();
//	m_filenames.push_back("chessboard1.jpg");
//	m_filenames.push_back("chessboard2.jpg");
//	m_filenames.push_back("chessboard3.jpg");
//	m_filenames.push_back("chessboard4.jpg");
//	m_filenames.push_back("chessboard5.jpg");
//	m_filenames.push_back("chessboard6.jpg");
//	m_filenames.push_back("chessboard7.jpg");
//	m_filenames.push_back("chessboard8.jpg");
//	m_filenames.push_back("chessboard9.jpg");
//	m_filenames.push_back("chessboard10.jpg");
//	m_filenames.push_back("chessboard11.jpg");
//	m_filenames.push_back("chessboard12.jpg");
//	m_filenames.push_back("chessboard13.jpg");
//	m_filenames.push_back("chessboard14.jpg");
//}
//
//void CameraCalibrator::setBorderSize(const Size &borderSize) {
//	m_borderSize = borderSize;
//}
//
//void CameraCalibrator::addChessboardPoints() {
//	vector<Point2f> srcCandidateCorners;
//	vector<Point3f> dstCandidateCorners;
//	for (int i = 0; i<m_borderSize.height; i++) {
//		for (int j = 0; j<m_borderSize.width; j++) {
//			dstCandidateCorners.push_back(Point3f(i, j, 0.0f));
//		}
//	}
//
//	for (int i = 0; i<m_filenames.size(); i++) {
//		Mat image = imread(m_filenames[i], CV_LOAD_IMAGE_GRAYSCALE);
//		findChessboardCorners(image, m_borderSize, srcCandidateCorners);
//		TermCriteria param(TermCriteria::MAX_ITER + TermCriteria::EPS, 30, 0.1);
//		cornerSubPix(image, srcCandidateCorners, Size(5, 5), Size(-1, -1), param);
//		if (srcCandidateCorners.size() == m_borderSize.area()) {
//			addPoints(srcCandidateCorners, dstCandidateCorners);
//		}
//	}
//}
//
//void CameraCalibrator::addPoints(const vector<Point2f> &srcCorners, const vector<Point3f> &dstCorners) {
//	m_srcPoints.push_back(srcCorners);
//	m_dstPoints.push_back(dstCorners);
//}
//
//void CameraCalibrator::calibrate(const Mat &src, Mat &dst) {
//	Size imageSize = src.size();
//	Mat cameraMatrix, distCoeffs, map1, map2;
//	vector<Mat> rvecs, tvecs;
//	calibrateCamera(m_dstPoints, m_srcPoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);
//	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), Mat(), imageSize, CV_32F, map1, map2);
//	remap(src, dst, map1, map2, INTER_LINEAR);
//}