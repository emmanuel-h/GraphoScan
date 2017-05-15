//#include "Shader.h"
//#include "Texture2D.h"
//#include "Camera.h"
//#include "OpenGL.cpp"
#include <GL/glew.h>
//#include <GLFW/glfw3.h>

//#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>


#include "GraphoScan.cpp"
#include "Camera.cpp"
#include "Shader.cpp"
#include "OpenGL.cpp"


#define OPENCV
#define OPENGL
//#define STEREOVISION


vector<Point2f> pts;
bool isFrameSelected = false;
int framesOfVideo(const char* filename);

void mouseSelectPoint_Callback(int event, int x, int y, int flags, void* userData);

void videoClipper(const char* videoName);

int main(int argc, char ** argv)
{

#ifdef STEREOVISION
	//CameraCalibrator calibrator;
	//calibrator.setBoardSize(Size(7, 7));
	//calibrator.stereoCalibration("ch", 13 ,"innerParam_left.txt", "innerParam_right.txt");
	
	//calibrator.setFilenames("ch",13);
	//calibrator.findChessboardPoints_File();
	//calibrator.saveInnerParam("innerParam_right.txt");
	//calibrator.findChessboardPoints_Stereo("innerParam_left.txt", "innerParam_right.txt");
	
#endif // STEREOVISION


#ifdef OPENCV
  int frame=framesOfVideo("/home/emmanuelh/Videos/3e_prise_g.avi");
  cout << frame << endl;
  //framesOfVideo("Test_xvid_001.avi");
	//videoClipper("Test_xvid_001.avi");
	
	GraphoScan grapho_left, grapho_right;
	// inutile
	//grapho_left.mySelectBg("undist_2007_L.avi","pt_bg_undist_2007_L.txt");
	
	//lance le tracking 
	grapho_left.myTrackerKCF("/home/emmanuelh/Videos/3e_prise_g.avi");

	grapho_left.calcImgPtsAndImgTrack();

	//enregistre dans les .jpg le HOG puis la trajectoire noir/blanc
	grapho_left.saveImgPtsAndImgTraject("imgPtsObjet_1207_undist_POLYTECH_L_1.jpg", "imgTrajectoire_cor_1207_undist_POLYTECH_L_1.jpg");

	//enregistre les points du tracking dans un fichier
	grapho_left.saveTrajectoire("left_add.txt");
	
	//enregistre les données du fichier dans ptsObjet
	//grapho_left.readTracjectoire("1207_undist_POLYTECH_L_1.txt");
	
	//a tester
    cout << "LA" << endl;
	grapho_left.insertPoints(3);
    cout << "LA" << endl;
    
	grapho_left.saveTrajectoire("1207_undist_POLYTECH_L_add_1.txt"); 

	
	
	
	//grapho_right.mySelectBg("undist_2007_R.avi", "pt_bg_undist_2007_R.txt");
	grapho_right.myTrackerKCF("/home/emmanuelh/Videos/3e_prise_d.avi");

	grapho_right.calcImgPtsAndImgTrack();

	grapho_right.saveImgPtsAndImgTraject("imgPtsObjet_1207_undist_POLYTECH_R_1.jpg", "imgTrajectoire_cor_1207_undist_POLYTECH_R_1.jpg");

	//grapho_right.saveTrajectoire("1207_undist_POLYTECH_R_1.txt");
	//grapho_right.readTracjectoire("1207_undist_POLYTECH_R_1.txt");
	//grapho_right.insertPoints(3);
	grapho_right.saveTrajectoire("right_add.txt");


	//calculer des points en 3D
	VideoCapture cap("/home/emmanuelh/Videos/3e_prise_g.avi");
	cv::Mat imgSrc;
	cap.read(imgSrc);
	GraphoScan::calAndSavePointsOf3D(imgSrc.size(), "left_add.txt", "right_add.txt", "pt2_add.txt");

	//grapho_left.ptsObjet = pts;

	//reperer des points manuellement(left)
	//grapho_left.selectPointManuel("1207_undist_POLYTECH_L.avi");
	//grapho_left.saveTrajectoire("pt1.txt");


	//grapho_right.ptsObjet = pts;

	//reperer des points manuellement(right)
	//grapho_right.selectPointManuel("left.avi");
	//grapho_right.saveTrajectoire("pt2.txt");

#endif // OPENCV

#ifdef OPENGL
	RunGL("pt2_add.txt");
    myOpenGL myApp;
	myApp.InitWindow(argc,argv);
	myApp.InitVertex();
	
#endif // OPENGL

	return 0;
}


//compte le nombre de frame dans une video donnée
int framesOfVideo(const char* filename)
{

	VideoCapture cap(filename);
	if (!cap.isOpened())
	{
		cout << "Cannot open the video." << endl;
		exit(1);
	}

	int frameCount = 0;
	cv::Mat imgSrc;
	vector<cv::Mat> vectImages;
	while (true)
	{
		cap.read(imgSrc);
		if (imgSrc.empty())
		{
			cout << "imgSrc is empty." << endl;
			break;
		}

		vectImages.push_back(imgSrc.clone());
		frameCount++;
	}
	cout << "frameCount = " << endl;

	//afficher la video
	int count = 0;
	cv::namedWindow(filename);
	createTrackbar("frameCount", filename, &count, frameCount -1 );
	
	//setMouseCallback(filename, mouseSelectPoint_Callback);
	//pts.clear();
	
	while (true)
	{
		cv::imshow(filename, vectImages[count]);
		if (waitKey(30) == 27)
			break;
	}
	return frameCount;
}

void mouseSelectPoint_Callback(int event, int x, int y, int flags, void * userData)
{
	cv::Point2f pt = cv::Point2f(x, y);
	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		pts.push_back(pt);
		std::cout << "pt.x = " << pt.x << "; pt.y" << pt.y << endl;
		break;
	default:
		break;
	}
}

void videoClipper(const char* videoName) 
{
	cv::Mat imgSrc;
	VideoCapture cap(videoName);
	if (!cap.isOpened())
	{
		std::cout << "Cannot open the video." << endl;
		exit(1);
	}
	cap.read(imgSrc);

	//la taille de la video
	Size frameSize = Size(imgSrc.cols / 2.0f, imgSrc.rows);
	std::cout << "frameSize = " << frameSize << endl;

	//initialisation des videowriters
	cv::VideoWriter outputVideo1("left.avi", CV_FOURCC('X', 'V', 'I', 'D'), 10, frameSize, true);
	if (!outputVideo1.isOpened()) {
		std::cout << "could not open the output video1 for write" << endl;
		return;
	}

	cv::VideoWriter outputVideo2("right.avi", CV_FOURCC('X', 'V', 'I', 'D'), 10, frameSize, true);
	if (!outputVideo2.isOpened()) {
		std::cout << "could not open the output video2 for write" << endl;
		return;
	}
	cv::Mat imgLeft = imgSrc(cv::Rect2d(0, 0, frameSize.width, frameSize.height)).clone();
	cv::Mat imgRight = imgSrc(cv::Rect2d(frameSize.width, 0, frameSize.width, frameSize.height)).clone();
	
	outputVideo1.write(imgLeft);
	outputVideo2.write(imgRight);

	int nFrameCount = 0;
	// possiblement à changer par frameCount plus tard
	while (nFrameCount < 270)
	{
		cap.read(imgSrc);
		imgLeft = imgSrc(cv::Rect2d(0, 0, frameSize.width, frameSize.height)).clone();
		imgRight = imgSrc(cv::Rect2d(frameSize.width, 0, frameSize.width, frameSize.height)).clone();

		outputVideo1.write(imgLeft);
		outputVideo2.write(imgRight);

		nFrameCount++;
	}
}
