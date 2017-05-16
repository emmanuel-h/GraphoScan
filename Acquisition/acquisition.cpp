
// Pour OpenCV 2
//#include </usr/include/opencv2/core/core.hpp>
//#include </usr/include/opencv2/highgui/highgui.hpp>
//#include </usr/include/opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <cstring>
#include <sys/time.h>
#include <vector>
#include <omp.h>

// Pour OpenCV 3
#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FlyCapture2.h"

#define RESULT_BENCH "result.bench"
#define LATENCE_BENCH "latence.bench"

#define CAMERA_WIDTH 1288
#define CAMERA_HEIGHT 964
#define NB_FPS 30

#define BENCH true

// not possible to use a #define for Matlab configuration file
// because of the N cameras instead of just left / right
#define MATLAB_CONFIG_FILE "CalibLeft_Matlab_1207_03.txt"


using namespace FlyCapture2;
using namespace std;


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
	cout << "==================== Welcome to GraphoScan ====================" << endl;
	
	cv::Size imageSize(CAMERA_WIDTH, CAMERA_HEIGHT);
	
	BusManager busMgr;
	unsigned int numCameras;
	Error error;
	
	
	//=============== Print build information ===============
	PrintBuildInfo();
	
	
	//============== Get camera number ===============
	error = busMgr.GetNumOfCameras(&numCameras);
	if(PGRERROR_OK != error.GetType()){
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
	
	
	//============== Arrays for parallel computing ===============
	Error errorArray[numCameras];
	PGRGuid guidArray[numCameras];
	Camera cameraArray[numCameras];
	CameraInfo camInfoArray[numCameras];

    bool noProb = true;
    //OMP_CANCELLATION=true;
	
	
	//=============== Beginning parallel region ==============
	#pragma omp parallel num_threads(numCameras)
	{
		int id = omp_get_thread_num();
		
		//========== Get cameras' id ==========
		errorArray[id] = busMgr.GetCameraFromIndex(id, &guidArray[id]);
		if(PGRERROR_OK != errorArray[id].GetType()){
			PrintError(errorArray[id]);
			//return -1;
            noProb = false;
            #pragma omp cancel parallel
		}
		
		//========== Connect the cameras ==========
		errorArray[id] = cameraArray[id].Connect(&guidArray[id]);
		if(PGRERROR_OK != errorArray[id].GetType()){
			cout << "Failed to connect to camera n° " << id << endl;
			//return -1;
            noProb = false;
            #pragma omp cancel parallel
		}else{
			cout << "Successfully connected to camera n° " << id << endl;
		}
		
		//========== Get cameras' info and print it ==========
		errorArray[id] = cameraArray[id].GetCameraInfo(&camInfoArray[id]);
		if(PGRERROR_OK != errorArray[id].GetType()){
			cout << "Failed to get informations from camera n° " << id << endl;
			//return -1;
            noProb = false;
            #pragma omp cancel parallel
		}else{
			cout << "=============== Camera n° " << id << " ==============" << endl;
			PrintCameraInfo(&camInfoArray[id]);
		}

        #pragma omp cancellation point parallel
	}

    if(!noProb){
        return -1;
    }
	
	
	string dateTemp = __DATE__;
	string date = replaceSpace(dateTemp);
	
	
	// create folder
	char* folder = const_cast<char*>(date.c_str());
	char const *make = "mkdir "; // md pour windows
	char* foldermake = new char[strlen(folder) + strlen(make) + 1];
	strcpy(foldermake, make);
	strcat(foldermake, folder);
	system(foldermake);
	
	cout << "=============== Picture & Videos ==============" << endl;
	
	cout << "Do you want to take pictures for calibration (press SPACE to take pictures) ? (y/n)" << endl;
	char chP;
	//cin >> chP;
	chP = 'N';
	
	cout << "Do you want to take videos (video begins immediately) ? (y/n)" << endl;
	char chV;
	//cin >>chV;
	chV = 'Y';
	
	//int numCalib = 0;
	int numPic = 0;
	const string format = ".jpg";
	
	//create image list
	cv::FileStorage fileStorageArray[numCameras];
	cv::FileStorage fileStorageStereo;
	
	cv::VideoWriter outputVideoArray[numCameras];
	//cv::VideoWriter outputVideoStereo;

    string windowName = "Cameras : ";
    char key = 0;

    time_t start, end;
    double fps;
    //int counter = 0;
    double sec;

    bool display = true;

    ofstream benchFile(LATENCE_BENCH, ios::app);

    Image rawImageArray[numCameras];
    Image rgbImageArray[numCameras];

    cv::Mat imageArray[numCameras];
    cv::Mat imageROI;

    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    
    
    timeval timevalArray[numCameras];
    ofstream latenceFile("latence_omp.bench", ios::app);
    
	
	#pragma omp parallel num_threads(numCameras)
	{
		int id = omp_get_thread_num();
		
		int numCalib = 0;
	
		if('y' == chP || 'Y' == chP){
			const string list = "_List";
			const string format_list = ".yaml";
			string output_list = date + "/" + date + "_camera_" + int2str(id) + list + format_list;
			fileStorageArray[id].open(output_list, cv::FileStorage::WRITE);
			fileStorageArray[id] << "images[";			
		}
		
		if('y' == chV || 'Y' == chV){
			string name = date + "/" + date + "_camera_" + int2str(id) + ".avi";
			outputVideoArray[id].open(name, CV_FOURCC('X','V', 'I', 'D'), NB_FPS, cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), true);
			if(!outputVideoArray[id].isOpened()){
				cout << "Could not open the output video n° " << id << " for write" << endl;
                noProb = false;
                #pragma omp cancel parallel
			}
		}
		
		// we wait for all threads to synchronize
		#pragma omp barrier


        //========== Start capture ==========
        errorArray[id] = cameraArray[id].StartCapture();
        if(PGRERROR_ISOCH_BANDWIDTH_EXCEEDED == errorArray[id].GetType()){
            cout << "Bandwidth exceeded (camera n° " << id << ")" << endl;
            //return -1;
            noProb = false;
            #pragma omp cancel parallel
        }else{
            if(PGRERROR_OK != errorArray[id].GetType()){
                cout << "Failed to start image capture (camera n° " << id << ")" << endl;
                //return -1;
                noProb = false;
                #pragma omp cancel parallel
            }
        }
        

        // Use only if we want to display a window while capturing video
        /*
        #pragma omp master
        {
            for(unsigned int i = 0; i < numCameras-1; i++){
                windowName += "  " + int2str(i) + "  -";
            }
            windowName += "  " + int2str(numCameras - 1);
            cv::namedWindow(windowName, CV_WINDOW_NORMAL);

            if(BENCH){
                time(&start);
            }
        }
        */
        
        
        #pragma omp master
        {
            if(BENCH){
                time(&start);
            }
        }

        
        // all threads wait for the master thread
        #pragma omp barrier
        
        int counter = 0;


        while(/*key != 27 */counter < 1000){

          //#pragma omp single
          //{
                counter++;
          //}
          
          //cout << counter << endl;
          
          //#pragma omp barrier

            errorArray[id] = cameraArray[id].RetrieveBuffer(&rawImageArray[id]);
            gettimeofday(&timevalArray[id], 0);
            if(PGRERROR_OK != errorArray[id].GetType()){
                cout << "Capture error (camera n° " << id << ")" << endl;
                //return -1;
                noProb = false;
                #pragma omp cancel parallel
            }
            
            #pragma omp barrier
            
            #pragma omp single
            {
            	//cout << "sec : " << timevalArray[0].tv_sec << endl;
            	//cout << "usec : " << timevalArray[0].tv_usec << endl;
            	latenceFile << "\"" << counter << "\" " << timevalArray[0].tv_sec - 1494859000 << "." << timevalArray[0].tv_usec << " " << timevalArray[1].tv_sec - 1494859000 << "." << timevalArray[1].tv_usec << endl;
            }

            rawImageArray[id].Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImageArray[id]);
            unsigned int rowBytes = (double)rgbImageArray[id].GetReceivedDataSize() / (double)rgbImageArray[id].GetRows();
            imageArray[id] = cv::Mat(rgbImageArray[id].GetRows(), rgbImageArray[id].GetCols(), CV_8UC3, rgbImageArray[id].GetData(), rowBytes);

            #pragma omp barrier

            // Use only if we want to display a window while capturing video
            /*
            #pragma omp single
            {
                string s = "Cameras :  ";
                int totalWidth = 0;
                int maxHeight = 0;
                int offset = 0;
                for(unsigned int i = 0; i < numCameras; i++){
                    totalWidth += imageArray[i].size().width;
                    int height = imageArray[i].size().height;
                    if(maxHeight < height){
                        maxHeight = height;
                    }
                    s += int2str(i) + "  ";
                }

                cv::Mat imageROI(maxHeight, totalWidth, CV_8UC3);
                //cout << "Max height : " << maxHeight << endl;
                //cout << "Total width : " << totalWidth << endl;

                for(unsigned int i = 0; i < numCameras; i++){
                    cv::Mat matrix(imageROI, cv::Rect(offset, 0, imageArray[i].size().width, imageArray[i].size().height));
                    imageArray[i].copyTo(matrix);
                    offset += imageArray[i].size().width;
                }

                imshow(windowName, imageROI);
                
                //outputVideoStereo.write(imageROI);
            }
            */
            

            
            // Calibration picture
            string time = __TIME__;
            if(' ' == key && ('y' == chP || 'Y' == chP)){
                string name = date + "/" + date + "_camera_" + int2str(id) + "_" + time + format;
                cout << "Save calibration image n° " <<time << endl;
                cv::imwrite(name, imageArray[id], compression_params);
                
                fileStorageArray[id] << name;
            }

            // Common picture
            if(' ' == key && ('n' == chP || 'N' == chP)){
                string name = date + "/" + date + "_camera_" + int2str(id) + "_pic_" + int2str(numCalib) + format;
                cout << "Save common image n° " << numCalib << endl;
                numCalib++;
                cv::imwrite(name, imageArray[id], compression_params);
            }

            if('y' == chV || 'Y' == chV){
                outputVideoArray[id].write(imageArray[id]);
            }

            #pragma omp single
            {
                if(display){
                    // decrease the value for the tests
                    // increase it for more "user-friendly" experience (the time gap to press ESC is longer)
                    // /!\ Increasing it too much leads to a heavy drop in FPS
                    //key = cv::waitKey(1);
                }
                display = !display;
            }
            
        } // end of while loop
        
        //#pragma omp barrier

        //#pragma omp cancellation point parallel
        
    } // end of parallel region
    
    if(!noProb){
        return -1;
    }

    benchFile.close();

    if(BENCH){
        time(&end);
    }

    //cout << counter << endl;
    sec = difftime(end, start);
    fps = 1000 / sec;
    //fps = counter / sec;
    cout << fps << endl;

    ofstream fpsFile("fps.bench", ios::app);
    fpsFile << fps << endl;
    fpsFile.close();
    latenceFile.close();

    // Calibration pic list
    if('y' == chP || 'Y' == chP){
        for(unsigned int i = 0; i < numCameras; i++){
            fileStorageArray[i] << "]";
        }
    }

    for(unsigned int i = 0; i < numCameras; i++){
        outputVideoArray[i].release();
        errorArray[i] = cameraArray[i].StopCapture();
        cameraArray[i].Disconnect();
    }


    


	/*
    //==================== Undistorsion ====================
    long double cm[numCameras][3][3], dc[numCameras][5];
    cout << endl << "========== Read calibration parameters from file ==========" << endl;

    cv::Mat cameraMatrixArray[numCameras];
    cv::Mat distCoeffArray[numCameras];
    cv::Mat map1Array[numCameras];
    cv::Mat map2Array[numCameras];

    cv::VideoCapture videoCapArray[numCameras];
    cv::VideoWriter undistVideoArray[numCameras];

    cv::Mat srcMatArray[numCameras];
    cv::Mat distMatArray[numCameras];
    
    for(unsigned int i = 0; i < numCameras; i++){
        string name = "Calib_camera_" + int2str(i) + "_Matlab.txt";
        ReadInnerParam(name.c_str(), cm[i], dc[i]);
    }
    
#pragma omp parallel num_threads(numCameras)
    {
        int id = omp_get_thread_num();

        cameraMatrixArray[id] = cv::Mat(3, 3, CV_64FC1, cm[id]);
        distCoeffArray[id] = cv::Mat(1, 5, CV_64FC1, dc[id]);

        cout << "=====> cameraMatrix n° " << id << "is : " << endl << cameraMatrixArray[id] << endl;
        cout << "=====> distCoeff n° " << id << "is : " << endl << distCoeffArray[id] << endl;

        cv::initUndistortRectifyMap(cameraMatrixArray[id], distCoeffArray[id], cv::Mat(), cv::Mat(), imageSize, CV_32F, map1Array[id], map2Array[id]);

        //=============== Load original videos ===============
        string source = date + "/" + date + "_camera_" + int2str(id) + ".avi";
        videoCapArray[id].open(source);
        if(!videoCapArray[id].isOpened()){
            cout << "Could not open file " << source << endl;
            //return -1;
            noProb = false;
            #pragma omp cancel parallel
        }

        int nbFrame = videoCapArray[id].get(CV_CAP_PROP_FRAME_COUNT);

        string name = date + "/" + date + "undist_camera_" + int2str(id) + ".avi";
        undistVideoArray[id].open(name,CV_FOURCC('X', 'V', 'I', 'D'), NB_FPS, cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), true);
        if(!undistVideoArray[id].isOpened()){
            cout << "Could not open the output file video from file " << name << endl;
        }

        for(int i = 0; i < nbFrame; i++){
            cv::remap(srcMatArray[id], distMatArray[id], map1Array[id], map2Array[id], CV_INTER_LINEAR);
            undistVideoArray[id].write(distMatArray[id]);
        }

        undistVideoArray[id].release();
        videoCapArray[id].release();

        #pragma omp cancellation point parallel
        
    } // end of undistortion parallel region

    if(!noProb){
        return -1;
    }
    */

    return 0;




    
}




































