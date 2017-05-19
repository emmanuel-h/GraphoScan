#include <GL/glew.h>
//#include <GLFW/glfw3.h>

//#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GraphoScan.hpp"
#include "OpenGL.hpp"


#define OPENCV
#define OPENGL
//#define STEREOVISION


string algo[]={"KCF","BOOSTING","MIL","TLD"};
vector<Point2f> pts;
bool isFrameSelected = false;
int framesOfVideo(const char* filename);

void mouseSelectPoint_Callback(int event, int x, int y, int flags, void* userData);

void videoClipper(const char* videoName);

int main(int argc, char ** argv)
{

  if(argc == 2 && (strcmp(argv[1], "--help")==0 || strcmp(argv[1], "-h")==0)){
    cout << "Usage: ./graphoScan VIDEO_FILENAME_LEFT VIDEO_FILENAME_RIGHT [ALGORITHM NAME]" << endl;
    cout << "" << endl;
    cout << "VIDEO_FILENAME_LEFT AND VIDEO_FILENAME_RIGHT must be .avi files" << endl;
    cout << "ALGORITHM NAME: default value KCF"<< endl;
    cout << "Other algorithms: TLD, MIL, BOOSTING"<<endl;;
    exit(1);
  }
  
  if(argc < 3 || argc > 4){
    cout << "Bad Number of Arguments" << endl;
    cout << "Use ./graphoScan -h or ./graphoScan --help to open the help menu" << endl;
    exit(1);
  }

  string algo_name;
  if(argc == 4){
    bool valid = false;
    int length = (sizeof(algo)/sizeof(*algo));
    for(int i=0; i < length ; i++){     
      if(argv[3]==algo[i]){
	valid = true;
      }
    }
    if(valid){      
      algo_name=argv[3];
    }else{
       cout << "Bad Algorithm Argument" << endl;
       cout << "Good algorithms : KCF, TLD, BOOSTING, MIL" << endl;
       exit(1);
    }
  }

  char * filename_g = argv[1];
  char * filename_d = argv[2];
  VideoCapture cap;
  cap.open(filename_g);
  if (!cap.isOpened())
    {
      cout << "Cannot open the video left." << endl;
      exit(1);
    }
  cap.open(filename_d);
  if (!cap.isOpened())
    {
      cout << "Cannot open the video right." << endl;
      exit(1);
    }
  
  
  

#ifdef STEREOVISION
	//CameraCalibrator calibrator;
	//calibrator.setBoardSize(Size(7, 7));
	//calibrator.stereoCalibration("ch", 13 ,"innerParam_left.txt", "innerParam_right.txt");
	
	//calibrator.setFilenames("ch",13);
	//calibrator.findChessboardPoints_File();
	//calibrator.saveInnerParam("innerParam_right.txt");
	//calibrator.findChessboardPoints_Stereo("innerParam_left.txt", "innerParam_right.txt");
	
#endif // STEREOVISION


  //#ifdef OPENCV
  //int frame=framesOfVideo("/home/emmanuelh/Videos/3e_prise_g.avi");
  int frame=framesOfVideo(filename_g);
  cout << frame << endl;
#ifdef OPENCV

  //framesOfVideo("Test_xvid_001.avi");
	videoClipper("Test_xvid_001.avi");
	
	GraphoScan grapho_left, grapho_right;
	// inutile
	//grapho_left.mySelectBg("undist_2007_L.avi","pt_bg_undist_2007_L.txt");
	
	//lance le tracking 
	//grapho_left.myTrackerKCF("/home/emmanuelh/Videos/3e_prise_g.avi");
	grapho_left.myTracker(filename_g,algo_name);

	grapho_left.calcImgPtsAndImgTrack();

	//enregistre dans les .jpg le HOG puis la trajectoire noir/blanc
	grapho_left.saveImgPtsAndImgTraject("imgPtsObjet_1207_undist_POLYTECH_L_1.jpg", "imgTrajectoire_cor_1207_undist_POLYTECH_L_1.jpg");

	
	grapho_left.showImgTrackAndHog("imgPtsObjet_1207_undist_POLYTECH_L_1.jpg", "imgTrajectoire_cor_1207_undist_POLYTECH_L_1.jpg");
	
	//enregistre les données du fichier dans ptsObjet
	//grapho_left.readTracjectoire("1207_undist_POLYTECH_L_1.txt");
	
	//a tester
	grapho_left.insertPoints(3);
    
	grapho_left.saveTrajectoire("left_add.txt"); 

	
	
	
	//grapho_right.mySelectBg("undist_2007_R.avi", "pt_bg_undist_2007_R.txt");
	//grapho_right.myTrackerKCF("/home/emmanuelh/Videos/3e_prise_d.avi");
	grapho_right.myTracker(filename_d,algo_name);

	grapho_right.calcImgPtsAndImgTrack();

	
	grapho_right.saveImgPtsAndImgTraject("imgPtsObjet_1207_undist_POLYTECH_R_1.jpg", "imgTrajectoire_cor_1207_undist_POLYTECH_R_1.jpg");

	grapho_right.showImgTrackAndHog("imgPtsObjet_1207_undist_POLYTECH_R_1.jpg", "imgTrajectoire_cor_1207_undist_POLYTECH_R_1.jpg");
	
	//grapho_right.saveTrajectoire("1207_undist_POLYTECH_R_1.txt");
	//grapho_right.readTracjectoire("1207_undist_POLYTECH_R_1.txt");
	grapho_right.insertPoints(3);
	grapho_right.saveTrajectoire("right_add.txt");


	//calculer des points en 3D
	//VideoCapture cap("/home/emmanuelh/Videos/3e_prise_g.avi");
	cap.open(filename_g);
	cv::Mat imgSrc;
	cap.read(imgSrc);
	GraphoScan::calAndSavePointsOf3D(imgSrc.size(), "left_add.txt", "right_add.txt", "pt2_add.txt", "pt2_addZ.txt");

	//grapho_left.ptsObjet = pts;

	//reperer des points manuellement(left)
	//grapho_left.selectPointManuel(filename_g);
	//grapho_left.saveTrajectoire("pt1.txt");


	//grapho_right.ptsObjet = pts;

	//reperer des points manuellement(right)
	//grapho_right.selectPointManuel("left.avi");
	//grapho_right.saveTrajectoire("pt2.txt");

#endif // OPENCV

#ifdef OPENGL
	MyOpenGL myApp;
	myApp.InitWindow();
	cout << "INIT WINDOW FINISHED" << endl;
	myApp.InitVertex();
	cout << "INIT VERTEX FINISHED" << endl;
	myApp.RunGL("pt2_add.txt");
    //myApp.RunGL("pt2_addZ.txt");
	cout << "RUNGL FINISHED" << endl;
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
		if (waitKey(1) == 27)
		  {
		    cv::destroyWindow(filename);
		    break;
		  }
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
