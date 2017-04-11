#include </usr/include/opencv2/core/core.hpp>
#include </usr/include/opencv2/highgui/highgui.hpp>
#include </usr/include/opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <cstring>
#include <sys/time.h>
#include <vector>
// Pour OpenCV 3
//#include </usr/include/opencv2/imgcodecs.hpp>

#include "FlyCapture2.h"


//#include "acquisition.hpp"

#define RESULT_BENCH "result.bench"
#define LATENCE_BENCH "latence.bench"

#define CAMERA_WIDTH 1288
#define CAMERA_HEIGHT 964
#define NB_FPS 30

#define BENCH TRUE

using namespace FlyCapture2;
using namespace std;

int frameCount = 0;
int frameCount1 = 0;

//cv::Mat image, image1;

/**
   Affiche les informations de version de FlyCapture
 */
void PrintBuildInfo(){
  FC2Version fc2Version;
  Utilities::GetLibraryVersion(&fc2Version);
  ostringstream version;
  version << "FlyCapture2 library version : " << fc2Version.major << "."
	  << fc2Version.minor << "." << fc2Version.type << "." << fc2Version.build;
  cout << version.str() << endl;
  ostringstream timeStamp;
  timeStamp << "Application build date : " << __DATE__ << " " << __TIME__;
  cout << timeStamp.str() << endl << endl;
}

/**
   Affiche les informations de la caméra
 */
void PrintCameraInfo(CameraInfo* pCamInfo){
  cout << "*** CAMERA INFORMATION ***" << endl;
  cout << "Serial number - " << pCamInfo->serialNumber << endl;
  cout << "Camera model - " << pCamInfo->modelName << endl;
  cout << "Camera vendor - " << pCamInfo->vendorName << endl;
  cout << "Sensor - " << pCamInfo->sensorInfo << endl;
  cout << "Resolution - " << pCamInfo->sensorResolution << endl;
  cout << "Firmware version - " << pCamInfo->firmwareVersion << endl;
  cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << endl << endl;
}

/**
   Transforme un int en string
 */
string int2str(int i){
  ostringstream out;
  out << i;
  return out.str();
}

/**
   Remplace les espaces par des underscores dans un string donnée (date ?)
 */
string replaceSpace(string str){
  int n = str.length();

  if (str.empty() || n < 0){
    cout << "date empty" << endl;
    return 0;
  }

  for (int i = 0; i < n; i++){
    if (str[i] == ' '){
      str[i] = '_';
    }
  }
  return str;
}

/**
   Affiche une erreur
 */
void PrintError(Error error){
  error.PrintErrorTrace();
}

/**
   Si il y a un clic gauche de la souris dans la fenêtre, affiche les coordonées du pointeur
 */
void on_mouse(int event, int x, int y, int flags, void* ustc){
  if ((event = CV_EVENT_LBUTTONDBLCLK)){
    cv::Point pt = cv::Point(x, y);
    if (pt.x <= 1288){
      cout << "(x,y) = (" << pt.x << ", " << pt.y << ")" << 
	endl;
    }
    else
      cout << "(x,y) = (" << pt.x - 1288 << ", " << pt.y << ")"
	   << endl;
  }
}

/**
   Affiche des paramètres de réglage de la caméra
 */
bool ReadInnerParam(const char* filename, long double cm[3][3], long double dc[5]){
  bool isRead = false;
  ifstream fin(filename, ios_base::in);
  if (!fin.is_open()) {
    cout << "Cannot open the file " << filename << "." << endl;
    return false;
  } else {
    long double tmp;
    for (int i = 0; i < 3; i++){
      for (int j = 0; j < 3; j++){
	//long double* tmp =cameraMatrix.ptr<long double>(i);
	fin >> tmp;
	cm[i][j] = tmp;
      }
    }
    for (int i = 0; i < 5; i++){
      //long double* tmp = distCoeffs.ptr<long double>(0);
      fin >> tmp;
      dc[i] = tmp;
      //distCoeffs.at<long>(i) = tmp;
    }
    //cout << "Read cameraMatrix: " << endl << " " << cm << endl;
    //cout << "Read distortion coeffients: " << endl << " " << dc << endl;
    isRead = true;
  }
  return isRead;
}




int main(int argc, char* argv[]){
  	cout << "=========================== Welcome to GraphoScan ===========================" << endl;
  
  	cv::Size imageSize(CAMERA_WIDTH, CAMERA_HEIGHT);
  	
  	BusManager busMgr;
  	unsigned int numCameras;
  	Error error, error1;
  	CameraInfo camInfo, camInfo1;
  	Camera camera, camera1;

  	//========================Print Build information======================
  	
  	PrintBuildInfo();


  	//========================get camera numbers======================

  	error = busMgr.GetNumOfCameras(&numCameras);
  	if (error != PGRERROR_OK){
		PrintError(error);
    	return -1;
  	}else{
    	cout << "Number of cameras detected : " << numCameras << endl;
  	}


  	//========================get two cameras' id======================
  	
  	//Error* errorArray = new Error[numCameras];
  	//PGRGuid* guidArray = new PGRGuid[numCameras];
  	//vector<Error> errorArray(numCameras);
  	//vector<PGRGuid> guidArray(numCameras);
  	Error errorArray[numCameras];
  	PGRGuid guidArray[numCameras];
  	
  	for(unsigned int i = 0; i < numCameras; i++){
  		errorArray[i] = busMgr.GetCameraFromIndex(i, &guidArray[i]);
  		if(PGRERROR_OK != errorArray[i].GetType()){
  			PrintError(errorArray[i]);
  			return -1;
  		}
  	}
  	
  	
  	//========================Connect the cameras====================== 

	//Camera* cameraArray = new Camera[numCameras];
	//vector<Camera> cameraArray(numCameras);
	Camera cameraArray[numCameras];
	
	for(unsigned int i = 0; i < numCameras; i++){
		errorArray[i] = cameraArray[i].Connect(&guidArray[i]);
		if(PGRERROR_OK != errorArray[i].GetType()){
			cout << "Failed to connect to camera n°" << i << endl;
			return -1;
		}else{
			cout << "Successfully connected to camera n°" << i << endl;
		}
	}


  	/*
  	  PropertyType mtype = FRAME_RATE;
  	  Property mproperty(mtype);
  	  //mproperty.present = true;
  	  mproperty.absControl = true;
  	  mproperty.onOff = true;
  	  mproperty.onePush = false;
  	  mproperty.autoManualMode = false;
  	  mproperty.absValue = 30;
  	  error = camera.SetProperty(&mproperty, false);
  	  error1 = camera1.SetProperty(&mproperty, false);
  	  PropertyInfo minfo(mtype);
  	  minfo.onOffSupported = true;
  	  minfo.readOutSupported = true;
  	  error = camera.GetPropertyInfo(&minfo);
  	  cout << minfo.absMax <<" " <<minfo.absMin << endl;*/
  	/*
  	  FC2Config mconfig;
  	  mconfig.grabMode = BUFFER_FRAMES;
  	  mconfig.numBuffers = 30;
  	  error = camera.SetConfiguration(&mconfig);
  	  error1 = camera1.SetConfiguration(&mconfig);
  	  FC2Config readconfig;
  	  error = camera.GetConfiguration(&readconfig);
  	  cout << "setconfig " << readconfig.grabMode << " " << 
  	  readconfig.numBuffers << endl;*/
  	//videomode framerate
  	/*VideoMode mvideomode = VIDEOMODE_800x600RGB;
  	  FrameRate mframerate = FRAMERATE_30;
  	  error = camera.SetVideoModeAndFrameRate(mvideomode, mframerate);
  	  error1 = camera1.SetVideoModeAndFrameRate(mvideomode, mframerate);*/
  	//cout << "videomode = " << mvideomode << ", framerate = " << 
  	//mframerate << endl;
  	//camera.GetProperty(&mproperty);
  	//camera.SetConfiguration();
  
  
  	// ========================Get the camera info and print it out====================== 

	CameraInfo camInfoArray[numCameras];

	for(unsigned int i = 0; i < numCameras; i++){
		errorArray[i] = cameraArray[i].GetCameraInfo(&camInfoArray[i]);
		if(PGRERROR_OK != errorArray[i].GetType()){
			cout << "Failed to get informations from camera n°" << i << endl;
			return -1;
		}else{
			cout << "==================== Camera n°" << i << " ====================" << endl;
			PrintCameraInfo(&camInfoArray[i]);
		}
	}

  	/*
    	std::cout << "The camera0 you use is " << camInfo.vendorName << " "
    	<< camInfo.modelName << " "
    	<< camInfo.serialNumber << std::endl;
    */

  	// ========================StartCapture====================== 
  	
  	for(unsigned int i = 0; i < numCameras; i++){
  		errorArray[i] = cameraArray[i].StartCapture();
  		if(PGRERROR_ISOCH_BANDWIDTH_EXCEEDED == errorArray[i].GetType()){
  			cout << "Bandwidth exceeded (camera n°" << i << ")" << endl;
  			return -1;
  		}else{
  			if(PGRERROR_OK != errorArray[i].GetType()){
  				cout << "Failed to start image capture (camera n°" << i << ")" << endl;
  				return -1;
  			}
  		}
  	}

  	// ===============================Take video / picture================================== 
  	// =====================take photos=============================
  	//two modes: take pictures for calibration, or just take common pictures

	
	string date0 = __DATE__;
	string date = replaceSpace(date0);
	
	// create folder
	char* folder = const_cast<char*>(date.c_str());
	char const *make = "mkdir "; // md pour windows
	char* foldermake = new char[strlen(folder) + strlen(make) + 1];
	strcpy(foldermake, make);
	strcat(foldermake, folder);
	system(foldermake);
	
	cout << "==================== Picture & Videos ====================" << endl;
	cout << "Do you want to take pictures for calibration (press SPACE to take pictures) ? (y/n)" << endl;
	
	char chP;
	//cin >> chP;
	chP = 'N';
  	
  	int numCalib = 0;
  	int numPic = 0;
  	const string right = "_right";
  	const string left = "_left";
  	const string stereo = "_Stereo";
  	const string pic = "_Pic";
  	const string format = ".jpg";
  	string PicNameL;
  	string PicNameR;
	
  	//create image list
  	
  	if('y' == chP || 'Y' == chP){
  		const string list = "_List";
  		const string format_list = ".yaml";
  		cv::FileStorage fsC, fsS; // fsCamera, fsStereo
  		for(unsigned int i = 0; i < numCameras; i++){
			string output_list = date + "/" + date + "_camera_" + i + list + format_list;
			fsC.open(output_list, cv::FileStorage::WRITE);
			fsC << "images[" << endl;
			output_list = date + "/" + date + "_stereo" + list + format_list;
			fsS.open(output_list, cv::FileStorage::WRITE);
			fsS << "images[" << endl;
  		}
	
  	}
  	
	
  	//============================create a video==============================
	
  	cout << "Do you want to take videos (video begins immediately) ? (y/n) ";
  	char chV;
  	//cin >> chV;
  	chV='Y';
  	
  	cv::VideoWriter outputVideoArray[numCameras];
  	if('y' == chV || 'Y' == chV){
  		for(unsigned int i = 0; i < numCameras; i++){
  			string name = date + "/" + date + "_camera_" + i + ".avi";
  			outputVideoArray[i].open(name, CV_FOURCC('X', 'V', 'I', 'D'), NB_FPS, cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), true);
  			if(!outputVideoArray[i].isOpened()){
  				cout << "Could not open the output video n°" << i << " for write" << endl;
  			}
  		}
  	}
	
	
  	//====================================Main loop==================================== 
	
	string windowName = "Cameras : ";
	for(unsigned int = 0; i < numCameras; i++){
		windowName += i + " - ";
	}
  	cv::namedWindow(windowName, CV_WINDOW_NORMAL);
  	
  	char key = 0;
	
  	time_t start, end;
  	double fps;
  	int counter = 0;
  	double sec;
  	
  	ofstream myfile2(LATENCE_BENCH,ios::app);
  	// pour afficher une frame sur deux
  	bool display = false;
  	
  	(BENCH)?time(&start); // lancement du timer pour les benchmarks
  	
  	Image rawImageArray[numCameras];
  	Image rgbImageArray[numCameras];
  	
  	while (/*key != 27*/counter<500){
  		++counter;
  		
    	// Get the image
    	for(unsigned int i = 0; i < numCameras; i++){
    		errorArray[i] = cameraArray[i].RetrieveBuffer(&rawImageArray[i]);
    		if(PGRERROR_OK != errorArray[i].GetType()){
    			cout << "capture error (camera n°" << i << ")" << endl;
    			return -1;
    		}
    	}
    	
    	for(unsigned int i = 0; i < numCameras; i++){
    		// convert to RGB
    		rawImageArray[i].Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImageArray[i]);
    		unsigned int rowBytes = (double)rgbImageArray[i].GetReceivedDataSize() / (double)rgbImageArray[i].GetRows();
    		// convert to OpenCV Mat
    		cv::Mat image = cv::Mat(rgbImageArray[i].GetRows(), rgbImageArray[i].GetCols(), CV_8UC3, rgbImageArray[i].GetData(), rowBytes);
    		// a bien définir
    		// concerne l'affichage dans la fenetre	
    	}

    	//cv::imshow("leftCamera", image1);
    	//two images in a same window
    	cv::Size sizeR = image.size();
    	cv::Size sizeL = image1.size();
    	cv::Mat imageROI(sizeL.height, sizeL.width + sizeR.width, CV_8UC3);
    	cv::Mat wright(imageROI, cv::Rect(sizeL.width, 0, sizeR.width, sizeR.height));
    	image.copyTo(wright);
    	cv::Mat wleft(imageROI, cv::Rect(0, 0, sizeL.width, sizeL.height));
    	image1.copyTo(wleft);
    	imshow("Cameras: left - right", imageROI);
	
    	//cv::setMouseCallback("Cameras: left - right", on_mouse, 0);
    	//==============================take pictures===========================
    	//compression-JPG
	
    	vector<int> compression_params;
    	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    	compression_params.push_back(100);
	
    
    	//calibration pic
    	if (key  == ' ' && (chP == 'y' || chP == 'Y')){
      	//attention: no space in names
      		PicNameR = date + "\\" + date + right + int2str(numCalib) + format;
      		PicNameL = date + "\\" + date + left + int2str(numCalib) + format;
      		cout << "save calibration image " << numCalib << endl;

      		//write to JPG
      		cv::imwrite(PicNameR.data(), image, compression_params);
      		cv::imwrite(PicNameL.data(), image1, compression_params);

      		//write to image list
      		fsL << PicNameL;
      		fsR << PicNameR;
      		fsS << PicNameL;
      		fsS << PicNameR;
      		numCalib++;
    	}

    	//common pic
    	string time = __TIME__;
    	if (key == ' ' && (chP == 'n' || chP == 'N')){
      		//attention: no space in names
      		PicNameR = date + "\\" + time + right + pic + int2str(numPic) + format;
      		PicNameL = date + "\\" + time + left + pic + int2str(numPic) + format;
      		cout << "save common image " << numPic << endl;

      		//write to JPG
      		cv::imwrite(PicNameR.data(), image, compression_params);
      		cv::imwrite(PicNameL.data(), image1, compression_params);
      		numPic++;
    	}

    	//save Mat to avi
    	if (chV == 'y' || chV == 'Y'){
      		outputVideo.write(image);
      		//cout << "frameCount: " << frameCount++ << endl;
      		outputVideo1.write(image1);
      		//cout << "frameCount1: " << frameCount1++ << endl;
    	}
    	if(display){
      		key = cv::waitKey(1); // à diminuer pour les tests
    	}
      	display = !display;
  	}
  
  
  	myfile2.close();

























  
  
  time(&end);
  sec = difftime (end, start);      
  fps = counter / sec;
  cout << fps << endl;
  
  ofstream myfile(RESULT_BENCH,ios::app);
  myfile << fps << endl;
  myfile.close();
  
  //calibration pic list
  if (chP == 'y' || chP == 'Y'){
    fsL << "]";
    fsR << "]";
    fsS << "]";
  }

  cout << "countframe = " << countfram << endl;
  outputVideo.release();
  outputVideo1.release();
  error = camera.StopCapture();
  error1 = camera1.StopCapture();
  camera.Disconnect();
  camera1.Disconnect();

  /*=============================undistortion=============================*/

  long double cmL[3][3], cmR[3][3], dcL[5], dcR[5];
  cout << endl << "============Read calibration parameters from files===============" << endl;
  ReadInnerParam("CalibLeft_Matlab_1207_03.txt", cmL, dcL);
  ReadInnerParam("CalibRight_Matlab_1207_03.txt", cmR, dcR);
  cv::Mat cameraMatrix_L = cv::Mat(3, 3, CV_64FC1, cmL);
  cv::Mat distCoeffs_L = cv::Mat(1, 5, CV_64FC1, dcL);
  cv::Mat cameraMatrix_R = cv::Mat(3, 3, CV_64FC1, cmR);
  cv::Mat distCoeffs_R = cv::Mat(1, 5, CV_64FC1, dcR);
  cout << "=====>cameraMatrix_L is: " << endl << cameraMatrix_L << endl;
  cout << "=====>disCoeffs_L is:" << endl << distCoeffs_L << endl << endl;
  cout << "=====>cameraMatrix_R is: " << endl << cameraMatrix_R << endl;
  cout << "=====>disCoeffs_L is:" << endl << distCoeffs_R << endl << endl;
  cv::Mat map1_L, map1_R, map2_L, map2_R;
  cv::initUndistortRectifyMap(cameraMatrix_L, distCoeffs_L, cv::Mat(), cv::Mat(), imageSize, CV_32F, map1_L, map2_L);
  cv::initUndistortRectifyMap(cameraMatrix_R, distCoeffs_R, cv::Mat(), cv::Mat(), imageSize, CV_32F, map1_R, map2_R);

  //================================load original videos======================================

  string sourceL = "Jul_20_2016_video_left.avi";
  string sourceR = "Jul_20_2016_video_right.avi";
  cv::VideoCapture cam_L, cam_R;
  cam_L.open(sourceL);
  if (!cam_L.isOpened()){
    cout << "could not open " << sourceL << endl;
    return -1;
  }

  cam_R.open(sourceR);
  if (!cam_R.isOpened()){
    cout << "could not open " << sourceR << endl;
    return -1;
  }

  cout << "frame: " << cam_L.get(CV_CAP_PROP_FRAME_COUNT) << endl;
  bool stop(false);

  //cv::namedWindow("before-after-left", CV_WINDOW_NORMAL);
  //cv::namedWindow("before-after-right", CV_WINDOW_NORMAL);

  cv::namedWindow("left-right-before-after", CV_WINDOW_NORMAL);

  //============================save undistortion video=====================================

  cv::VideoWriter undist_L, undist_R;
  cv::VideoWriter Four;
  undist_L.open("undist_XIAODI_L.avi", CV_FOURCC('X', 'V', 'I', 'D'), 10, cv::Size(1288, 964), true);
  if (!undist_L.isOpened()){
    cout << "could not open the output video for write" << endl;
    return -1;
  }

  undist_R.open("undist_XIAODI_R.avi", CV_FOURCC('X', 'V', 'I', 'D'), 10, cv::Size(1288, 964), true);

  if (!undist_R.isOpened()){
    cout << "could not open the output video1 for write" << endl;
    return -1;
  }

  Four.open("total_image_four_in_one_XIAODI.avi", CV_FOURCC('X', 'V', 'I', 'D'), 10, cv::Size(1288 * 2, 964 * 2), true);

  if (!Four.isOpened()){
    cout << "could not open the output video Four for write" << endl;
    return -1;
  }

  //============================================MAIN LOOP=============================================

  int i = 0;
  while (!stop){
    cv::Mat src_L, src_R, dst_L, dst_R;
    if (!cam_L.read(src_L))
      break;
    if (!cam_R.read(src_R))
      break;

    //CalibLeft.rectifyMap(src_L,dst_L);
    //CalibRight.rectifyMap(src_R, dst_R);

    cv::remap(src_L, dst_L, map1_L, map2_L, CV_INTER_LINEAR);
    cv::remap(src_R, dst_R, map1_R, map2_R, CV_INTER_LINEAR);
    cv::Mat ROI4(964 * 2, 1288 * 2, CV_8UC3);
    cv::Mat LeftTop(ROI4, cv::Rect(0, 0, 1288, 964));
    src_L.copyTo(LeftTop);
    cv::Mat RightTop(ROI4, cv::Rect(1288, 0, 1288, 964));
    dst_L.copyTo(RightTop);
    cv::Mat LeftDown(ROI4, cv::Rect(0, 964, 1288, 964));
    src_R.copyTo(LeftDown);
    cv::Mat RightDown(ROI4, cv::Rect(1288, 964, 1288, 964));
    dst_R.copyTo(RightDown);
    imshow("left-right-before-after", ROI4);
    undist_L.write(dst_L);
    undist_R.write(dst_R);
    Four.write(ROI4);
    i++;
    //cout << "frame " << i << endl;
    if (cv::waitKey(30) == 27 || i > cam_L.get(CV_CAP_PROP_FRAME_COUNT)){
      stop = true;
    }
  }
  cout << "frame = " << i << endl;
  undist_L.release();
  undist_R.release();
  Four.release();
  cam_L.release();
  cam_R.release();
  return 0;
}
