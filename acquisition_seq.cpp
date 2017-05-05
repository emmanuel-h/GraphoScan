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

#define BENCH true

// not possible to use a #define for Matlab configuration file
// because of the N cameras instead of left / right
#define MATLAB_CONFIG_FILE "CalibLeft_Matlab_1207_03.txt"


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
  	Camera camera, camera1;

  	//========================Print Build information======================
  	
  	PrintBuildInfo();


  	//========================get camera numbers======================

  	error = busMgr.GetNumOfCameras(&numCameras);
  	if (error != PGRERROR_OK){
		PrintError(error);
    	return -1;
  	}else{
  		if(0 == numCameras){
  			cout << "No camera detected, aborting" << endl;
  			return -1;
  		}else{
    		cout << "Number of cameras detected : " << numCameras << endl;
    	}
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
    cv::FileStorage fileStorageArray[numCameras];
    cv::FileStorage fileStorageStereo;
	
  	if('y' == chP || 'Y' == chP){
  		const string list = "_List";
  		const string format_list = ".yaml";
  		cv::FileStorage fsC, fsS; // fsCamera, fsStereo
  		for(unsigned int i = 0; i < numCameras; i++){
            string output_list = date + "/" + date + "_camera_" + int2str(i) + list + format_list;
            fileStorageArray[i].open(output_list, cv::FileStorage::WRITE);
            fileStorageArray[i] << "images[";
  		}
        string output_list = date + "/" + date + "_stereo" + list + format_list;
        fileStorageStereo.open(output_list, cv::FileStorage::WRITE);
        fileStorageStereo << "images[";
	
  	}
  	
	
  	//============================create a video==============================
	
  	cout << "Do you want to take videos (video begins immediately) ? (y/n) " << endl;
  	char chV;
  	//cin >> chV;
  	chV='Y';
  	cv::VideoWriter outputVideoArray[numCameras + 1];
  	if('y' == chV || 'Y' == chV){
  		for(unsigned int i = 0; i < numCameras; i++){
            string name = date + "/" + date + "_camera_" + int2str(i) + ".avi";
  			outputVideoArray[i].open(name, CV_FOURCC('X', 'V', 'I', 'D'), NB_FPS, cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), true);
  			if(!outputVideoArray[i].isOpened()){
  				cout << "Could not open the output video n°" << i << " for write" << endl;
  			}
  		}
  		string nameStereo = date + "/" + date + "_stereo_" + ".avi";
  		outputVideoArray[numCameras].open(nameStereo, CV_FOURCC('X', 'V', 'I', 'D'), NB_FPS, cv::Size(CAMERA_WIDTH * 2, CAMERA_HEIGHT), true);
  		if(!outputVideoArray[numCameras].isOpened()){
  			cout << "Could not open the output video stereo for write" << endl;
  		}
  	}
	
	
  	//====================================Main loop==================================== 
	
	string windowName = "Cameras : ";
	for(unsigned int i = 0; i < numCameras; i++){
		windowName += int2str(i) + " - ";
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

    if(BENCH){
      time(&start); // lancement du timer pour les benchmarks
    }
  	// (BENCH)?time(&start); // lancement du timer pour les benchmarks
  	
  	Image rawImageArray[numCameras];
  	Image rgbImageArray[numCameras];
  	
  	
  	
  	// =============== Main loop ===============
  	
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
    	//cv::Size sizeArray[numCameras];
    	cv::Mat imageArray[numCameras];
    	int totalWidth = 0;
    	int maxHeight = 0;
    	for(unsigned int i = 0; i < numCameras; i++){
    		// convert to RGB
    		rawImageArray[i].Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImageArray[i]);
    		unsigned int rowBytes = (double)rgbImageArray[i].GetReceivedDataSize() / (double)rgbImageArray[i].GetRows();
    		// convert to OpenCV Mat
    		imageArray[i] = cv::Mat(rgbImageArray[i].GetRows(), rgbImageArray[i].GetCols(), CV_8UC3, rgbImageArray[i].GetData(), rowBytes);
    		// a bien définir
    		// concerne l'affichage dans la fenetre	
    		//sizeArray[i] = imageArray[numCameras].size();
    		//cout << "width : " << imageArray[i].size().width << endl;
    		//cout << "height : " << imageArray[i].size().height << endl;
    		totalWidth += imageArray[i].size().width;
    		if(maxHeight < imageArray[i].size().height){
    			maxHeight = imageArray[i].size().height;
    		}
    	}
    	
    	//cout << "1" << endl;
    	//cout << "maxHeight : " << maxHeight << endl;
    	//cout << "totalWidth : " << totalWidth << endl;
    	cv::Mat imageROI(maxHeight, totalWidth, CV_8UC3);
    	//cout << "2" << endl;
    	int offset = 0;
    	string s = "Cameras : ";
    	for(unsigned int i = 0; i < numCameras; i++){
            cv::Mat matrix(imageROI, cv::Rect(offset, 0, imageArray[i].size().width, imageArray[i].size().height));
    		imageArray[i].copyTo(matrix);
    		offset += imageArray[i].size().width;
    		s += int2str(i) + "  ";
    	}
    	imshow(windowName, imageROI);
    	
	
    	//cv::setMouseCallback("Cameras: left - right", on_mouse, 0);
    	//==============================take pictures===========================
    	//compression-JPG
	
    	vector<int> compression_params;
    	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    	compression_params.push_back(100);
	
    
    	//calibration pic
    	if (key  == ' ' && (chP == 'y' || chP == 'Y')){
      	//attention: no space in names
      		for(unsigned int i = 0; i < numCameras; i++){
              string name = date + "/" + date + "camera-" + int2str(i) + "_" + int2str(numCalib) + format;
      			cout << "save calibration image n°" << numCalib << endl;
      			cv::imwrite(name, imageArray[i], compression_params);

                fileStorageArray[i] << name;
                fileStorageStereo << name;
      		}
      		numCalib++;
    	}

    	//common pic
    	string time = __TIME__;
    	if (key == ' ' && (chP == 'n' || chP == 'N')){
      		//attention: no space in names
      		for(unsigned int i = 0; i < numCameras; i++){
              string name = date + "/" + date + "camera-" + int2str(i) + "_pic" + int2str(numPic) + format;
      			cout << "save common image " << numPic << endl;
      			cv::imwrite(name, imageArray[i], compression_params);
      		}
      		numPic++;
    	}

    	//save Mat to avi
    	if (chV == 'y' || chV == 'Y'){
    		for(unsigned int i = 0; i < numCameras; i++){
    			outputVideoArray[i].write(imageArray[i]);
    		}
    		outputVideoArray[numCameras].write(imageROI);
    	}
    	
    	if(display){
      		key = cv::waitKey(1); // à diminuer pour les tests
    	}
      	display = !display;
  	}
  
  
  	myfile2.close();

    if(BENCH){
        time(&end);
    }
    sec = difftime (end, start);      
    fps = counter / sec;
    cout << fps << endl;
  
    ofstream myfile(RESULT_BENCH,ios::app);
    myfile << fps << endl;
    myfile.close();
  
    //calibration pic list
    if (chP == 'y' || chP == 'Y'){
        for(unsigned int i = 0; i < numCameras; i++){
            fileStorageArray[i] << "]";
        }
        fileStorageStereo << "]";
    }

    //cout << "countframe = " << countfram << endl;
    for(unsigned int i = 0; i < numCameras; i++){
        outputVideoArray[i].release();
        errorArray[i] = cameraArray[i].StopCapture();
        cameraArray[i].Disconnect();
    }
    





    
    //=============================undistortion=============================
    
	long double cm[numCameras][3][3], dc[numCameras][5];
	cout << endl << "============ Read calibration parameters from file ===============" << endl;
	
	for(unsigned int i = 0; i < numCameras; i++){
		string name = "Calib_camera_" + int2str(i) + "_Matlab.txt";
		ReadInnerParam(name.c_str(), cm[i], dc[i]);
	}
	
	cv::Mat cameraMatrixArray[numCameras];
	cv::Mat distCoeffsArray[numCameras];
	
	cv::Mat map1Array[numCameras];
	cv::Mat map2Array[numCameras];
	
	for(unsigned int i = 0; i < numCameras; i++){
		cameraMatrixArray[i] = cv::Mat(3, 3, CV_64FC1, cm[i]);
		distCoeffsArray[i] = cv::Mat(1, 5, CV_64FC1, dc[i]);
		
		cout << "=====> cameraMatrix n° " << i << " is : " << endl << cameraMatrixArray[i] << endl;
		cout << "=====> distCoeffs n° " << i << " is : " << endl << distCoeffsArray[i] << endl;
		
		cv::initUndistortRectifyMap(cameraMatrixArray[i], distCoeffsArray[i], cv::Mat(), cv::Mat(), imageSize, CV_32F, map1Array[i], map2Array[i]);
	}
	
	//=============================load original videos=============================
	cv::VideoCapture videoCapArray[numCameras];
	for(unsigned int i = 0; i < numCameras; i++){
		string source = date + "/" + date + "_camera_" + int2str(i) + ".avi";
		videoCapArray[i].open(source);
		if(!videoCapArray[i].isOpened()){
			cout << "Could not open file " << source << endl;
			return -1;
		}
	}
	
	int nbFrame = videoCapArray[0].get(CV_CAP_PROP_FRAME_COUNT);
	cout << "Number of frame : " << nbFrame << endl;
	
	
	//============================save undistortion video=====================================
	cv::VideoWriter undistVideoArray[numCameras];
	for(unsigned int i = 0; i < numCameras; i++){
		string name = date + "/" + date + "_undist_camera_" + int2str(i) + ".avi";
		undistVideoArray[i].open(name, CV_FOURCC('X', 'V', 'I', 'D'), NB_FPS, cv::Size(CAMERA_WIDTH,  CAMERA_HEIGHT), true);
		if(!undistVideoArray[i].isOpened()){
			cout << "Could not open the output video from file " << name << endl;
		}
	}
	
	

    //============================================MAIN LOOP=============================================
    cv::Mat srcMatArray[numCameras];
    cv::Mat distMatArray[numCameras];
    
    for(int n = 0; n < nbFrame; n++){
    
    	for(unsigned int i = 0; i < numCameras; i++){
    	
    		/*
    		if(!videoCapArray[i].read(srcMatArray[i])){
    			break;
    		}
    		*/
    		
    		cv::remap(srcMatArray[i], distMatArray[i], map1Array[i], map2Array[i], CV_INTER_LINEAR);
    		undistVideoArray[i].write(distMatArray[i]);
    	}
    }
    
    for(unsigned int i = 0; i < numCameras; i++){
    	undistVideoArray[i].release();
    	videoCapArray[i].release();
    }
    
    return 0;
    
    
}
