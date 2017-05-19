#include "GraphoScan.hpp"
#include "OpenGL.hpp"

#define OPENCV
#define OPENGL
//#define MANUAL_SELECTION


string algo[]={"KCF","BOOSTING","MIL","TLD"};
vector<cv::Point2f> pts;
bool isFrameSelected = false;

/*!
 * \fn int framesOfVideo(const char* filename)
 * \brief calculate number of frame in the video
 *
 * calculate the number of frames in the video named  
 */
int framesOfVideo(const char* filename);

int main(int argc, char ** argv) {

  if(argc == 2 && (strcmp(argv[1], "--help")==0 || strcmp(argv[1], "-h")==0)) {
    cout << "Usage: ./graphoScan VIDEO_FILENAME_LEFT VIDEO_FILENAME_RIGHT [ALGORITHM NAME]" << endl;
    cout << "" << endl;
    cout << "VIDEO_FILENAME_LEFT AND VIDEO_FILENAME_RIGHT must be .avi files" << endl;
    cout << "ALGORITHM NAME: default value KCF"<< endl;
    cout << "Other algorithms: TLD, MIL, BOOSTING"<<endl;;
    exit(1);
  }
  
  if(argc < 3 || argc > 4) {
    cout << "Bad Number of Arguments" << endl;
    cout << "Use ./graphoScan -h or ./graphoScan --help to open the help menu" << endl;
    exit(1);
  }

  string algo_name;
  if (argc == 4) {
    bool valid = false;
    int length = (sizeof(algo)/sizeof(*algo));
    for (int i=0; i < length ; i++) {     
      if (argv[3] == algo[i]) {
	valid = true;
      }
    }
    
    if (valid) {      
      algo_name=argv[3];
    } else {
      cout << "Bad Algorithm Argument" << endl;
      cout << "Good algorithms : KCF, TLD, BOOSTING, MIL" << endl;
      exit(1);
    }
  }else{
    algo_name="KCF";
  }

  char * filename_g = argv[1];
  char * filename_d = argv[2];
  VideoCapture cap;
  cap.open(filename_g);
  
  if (!cap.isOpened()) {
    cout << "Cannot open the video left." << endl;
    exit(1);
  }
  
  cap.open(filename_d);
  if (!cap.isOpened()) {
    cout << "Cannot open the video right." << endl;
    exit(1);
  }

#ifdef OPENCV
  
  int frame = framesOfVideo(filename_g);
  cout << "FrameCount = " << frame << endl;

  GraphoScan grapho_left, grapho_right;

  //VIDEO CAMERA LEFT
  grapho_left.myTracker(filename_g,algo_name);
  grapho_left.calcImgPtsAndImgTrack();

  // save trajectory of HOG and Tracking in .jpg pictures
  grapho_left.saveImgPtsAndImgTraject("imgPtsObjet_1207_undist_POLYTECH_L_1.jpg", "imgTrajectoire_cor_1207_undist_POLYTECH_L_1.jpg");

  // show in a full-screen window the result of each pictures
  grapho_left.showImgTrackAndHog("imgPtsObjet_1207_undist_POLYTECH_L_1.jpg", "imgTrajectoire_cor_1207_undist_POLYTECH_L_1.jpg");
	
  // insert points between trajectory points
  grapho_left.insertPoints(3);

  // save trajectory points in a file
  grapho_left.saveTrajectoire("left_add.txt"); 

  // VIDEO CAMERA RIGHT
  grapho_right.myTracker(filename_d,algo_name);
  grapho_right.calcImgPtsAndImgTrack();
	
  grapho_right.saveImgPtsAndImgTraject("imgPtsObjet_1207_undist_POLYTECH_R_1.jpg", "imgTrajectoire_cor_1207_undist_POLYTECH_R_1.jpg");

  grapho_right.showImgTrackAndHog("imgPtsObjet_1207_undist_POLYTECH_R_1.jpg", "imgTrajectoire_cor_1207_undist_POLYTECH_R_1.jpg");
        
  grapho_right.insertPoints(3);
  grapho_right.saveTrajectoire("right_add.txt");


  // calculate and save 3D points in a file
  cap.open(filename_g);
  cv::Mat imgSrc;
  cap.read(imgSrc);
  GraphoScan::calAndSavePointsOf3D(imgSrc.size(), "left_add.txt", "right_add.txt", "pt2_add.txt", "pt2_addZ.txt");

#ifdef MANUAL_SELECTION
  
  // manual selection of roi (not used because too much frames)
  grapho_left.selectPointManuel(filename_g);
  grapho_left.saveTrajectoire("pt1.txt");
  
  grapho_right.selectPointManuel(filename_d);
  grapho_right.saveTrajectoire("pt2.txt");
  
#endif

#endif

#ifdef OPENGL
  
  // 3D reconstruct with OpenGL
  MyOpenGL myApp;
  myApp.InitWindow();
  myApp.InitVertex();
  myApp.RunGL("pt2_add.txt");
  
#endif
  return 0;
}


//calculate number of frame in a video
int framesOfVideo(const char* filename) {

  VideoCapture cap(filename);
  if (!cap.isOpened()) {
      cout << "Cannot open the video." << endl;
      exit(1);
  }

  int frameCount = 0;
  cv::Mat imgSrc;
  vector<cv::Mat> vectImages;
  while (true) {
      cap.read(imgSrc);
      if (imgSrc.empty()) {
	  cout << "Source picture is empty." << endl;
	  break;
	}

      vectImages.push_back(imgSrc.clone());
      frameCount++;
    }

  //display video
  int count = 0;
  cv::namedWindow(filename);
  createTrackbar("frameCount", filename, &count, frameCount -1 );
	
  while (true) {
      cv::imshow(filename, vectImages[count]);
      if (waitKey(1) == 27) {
	  cv::destroyWindow(filename);
	  break;
	}
    }
	
  return frameCount;
}


