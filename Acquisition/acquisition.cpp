
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

#define CAMERA_WIDTH 1288
#define CAMERA_HEIGHT 964
#define NB_FPS 30

#define SN0 15508330 // serial number of the left camera
#define SN1 15508311 // serial number of the right camera
// add the serial number of other cameras to keep camera 0 as left, camera 1 as right, and camera X as whatever


#define WINDOWS false // set to true if user is under Windows OS, false otherwise


using namespace FlyCapture2;
using namespace std;


/*!
 * \brief {Serial numbers of different cameras
 * [0] is the serial number of camera 0, so camera 0 is camera left
 * [1] is the serial number of camera 1, so camera 1 is camera right
 * add serial numbers here whenever you connect another camera}
 */
static unsigned int serialNumber[] = {15508330, 15508311};

/*!
 * \brief default mode for performance measurement is set to false
 */
static bool bench = false;

/*!
 * \brief default mode for display a window while capturing is set to false
 */
static bool display = false;

/*!
 * \brief Number of frames to compute when not displaying the window
 */
static int nbFrame = -1;


/*!
 * \fn void PrintBuildInfo()
 * \brief Display FlyCapture's version informations (Polytech)
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

/*!
 * \fn void PrintCameraInfo (CameraInfo * pCamInfo)
 * \brief Display camera's informations (Polytech)
 *
 * \param pCamInfo Structure containing camera's informations
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

/*!
 * \fn string int2str (int i)
 * \brief Converts an int into a string (Polytech)
 * 
 * \parem i The int to convert
 * \return The int converted into a string
 */
string int2str(int i){
    ostringstream out;
    out << i;
    return out.str();
}

/*!
 * \fn string replaceSpace (string str)
 * \brief Converts spaces with underscores in a given string (mainly used for dates) (Polytech)
 *
 * \param str The string to be converted
 * \return The string where spaces are replaced with underscores
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

/*!
 * \fn void PrintError (Error error)
 * \brief Display an error (Polytech)
 *
 * \param error The error to be displayed
 */
void PrintError(Error error){
    error.PrintErrorTrace();
}

/*!
 * \fn void on_mouse (int event, int x, int y, int flags, void * ustc)
 * \brief Displays pointer coordinates when a click is made in the window (Polytech)
 *
 * \param event
 * \param x The x coordinate of the pointer
 * \param y The y coordinate of the pointer
 * \param flags
 * \param ustc
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

/*!
 * \fn bool ReadInnerParam (const char * filename, long double cm [3][3], long double dc [5])
 * \brief Read settings parameters of the camera based on a .txt file filled with MatLab 
 *
 * \param filename The filename to be read (must respect a certain format, see documentation for more details)
 * \param cm The IntrinsicMatrix of the camera given by MatLab
 * \param dc The RadialDisortion and TangentialDistortion of the camera given by MatLab
 * \return True if the read was successful
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
                //cout << "tmp : " << tmp << endl;
                cm[i][j] = tmp;
            }
        }
        for (int i = 0; i < 5; i++){
            //long double* tmp = distCoeffs.ptr<long double>(0);
            fin >> tmp;
            //cout << "tmp 2 : " << tmp << endl;
            dc[i] = tmp;
            //distCoeffs.at<long>(i) = tmp;
        }
        //cout << "Read cameraMatrix: " << endl << " " << cm << endl;
        //cout << "Read distortion coeffients: " << endl << " " << dc << endl;
        isRead = true;
    }
    return isRead;
}



/*!
 * \fn int undistortion (int numCamera, string id, cv::Size imageSize, string date)
 * \brief Used to perform undistortion on recently captured videos
 * 
 * \param numCamera The number of the camera which we want to undistort its capture
 * \param id The randomly generated ID affected to the video
 * \param imageSize The cv::Size of the capture (resolution of the camera)
 * \param date The date of the day, which is contained in the name of the video
 * \return -1 if there was a problem, 0 otherwise
 */
int undistortion(int numCamera, string id, cv::Size imageSize, string date){
    
    long double cm[3][3], dc[5];
    cout << endl << "========== Read calibration parameters from file ==========" << endl;

    // Well configured file with MatLab
    // No clue on how to do that, ask previous Polytech students
    string name = "Calib_camera_" + int2str(numCamera) + "_Matlab.txt";
    ReadInnerParam(name.c_str(), cm, dc);

    cv::Mat cameraMatrix = cv::Mat(3, 3, CV_64FC1, cm);
    cv::Mat distCoeffs = cv::Mat(1, 5, CV_64FC1, dc);

    cv::Mat map1, map2;

    // cv::initUndistort + cv::remap OR cv::undistort only
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cv::Mat(), imageSize, CV_32FC1, map1, map2);

    string source = date + "/" + date + "_camera_" + int2str(numCamera) + "_" + id + ".avi";
    cv::VideoCapture capture;
    capture.open(source);
    if(!capture.isOpened()){
        cout << "Could not open " << source << endl;
        return -1;
    }

    int frame = capture.get(CV_CAP_PROP_FRAME_COUNT); // we get the number of frame in the video

    cv::VideoWriter undist;

    string nameUndist = date + "/" + date + "_camera_" + int2str(numCamera) + "_" + id + "_undist.avi";

    undist.open(nameUndist, CV_FOURCC('X', 'V', 'I', 'D'), NB_FPS, imageSize, true);
    if(!undist.isOpened()){
        cout << "Could not open the output video to write " << nameUndist << endl;
        return -1;
    }

    cv::Mat srcMat, destMat;

    for(int i = 0; i < frame; i++){
        if(!capture.read(srcMat)){
            cout << "Error while reading " << source << endl;
            break;
        }

        // other way to undistort
        //cv::undistort(srcMat, destMat, cameraMatrix, distCoeffs);
        
        cv::remap(srcMat, destMat, map1, map2, CV_INTER_LINEAR);

        undist.write(destMat);
    }
    

    capture.release();
    undist.release();

    cout << "Wrote undist video into " << nameUndist << endl;

    return 0;
}


/*!
 * \fn string genRandomId (int len)
 * \brief Used to affect a random ID to a video
 *  This way, we can keep more than one video from the same day since the ID
 *  is very likely to be unique for a single day
 *
 * \param len The length of the ID. Increase to have more chance of having a very unique ID
 * \return The string corresponding to the ID given
 */
string genRandomId(int len){

    const char alpha[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int sizeAlpha = 62;

    string s = "";

    for(int i = 0; i < len; i++){
        s += alpha[rand() % (sizeAlpha - 1)];
    }

    return s;
}






int main(int argc, char* argv[]){
    
	cout << "==================== Welcome to GraphoScan ====================" << endl;

    //=============== Ask parameters ===============
    cout << "Do you want to enter performance measurement mode ? [y/n]" << endl;
    char chB = ' ';
    bool cond = false;
    while(!cond){
        cin >> chB;
        if('y' == chB || 'Y' == chB || 'n' == chB || 'N' == chB){
            cond = true;
        }else{
            cout << "Error : check your answer (enter 'y' for yes, 'n' for no)" << endl;
        }
    }
    if('y' == chB ||'Y' == chB){
        bench = true;
        cout << "### A measure of performances will be performed while capturing ###" << endl;
    }else{
        cout << "### No performance measures will be performed during the capture ###" << endl;
    }

    cout << endl <<  "Do you want to display the acquisition being captured by cameras ? [y/n]" << endl;
    char chD = ' ';
    cond = false;
    while(!cond){
        cin >> chD;
        if('y' == chD || 'Y' == chD || 'n' == chD || 'N' == chD){
            cond = true;
        }else{
            cout << "Error : check your answer (enter 'y' for yes, 'n' for no)" << endl;
        }
    }
    if('y' == chD || 'Y' == chD){
        display = true;
        cout << "### The videos will be displayed while being captured ###" << endl;
    }else{
        cond = false;
        cout << "### The videos won't be displayed while being captured ###" << endl;
        cout << "How many frames do you want to capture ?" << endl;
        while(!cond){
            cin >> nbFrame;
            if(0 < nbFrame){
                cond = true;
            }else{
                cout << "Error : please enter a valid number" << endl;
            }
        }
        cout << "Capturing " << nbFrame << " frames for this session" << endl << endlx;
    }
	
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
	
	
	//=============== Beginning parallel region ==============
	#pragma omp parallel num_threads(numCameras)
	{
		int id = omp_get_thread_num();
		
		//========== Get cameras' id ==========
		errorArray[id] = busMgr.GetCameraFromIndex(id, &guidArray[id]);
		if(PGRERROR_OK != errorArray[id].GetType()){
			PrintError(errorArray[id]);
            noProb = false;
		}
		
		//========== Connect the cameras ==========
        //
        // We match the cameras with their serial number so
        // camera 0 is the left camera
        // camera 1 is the right camera
        //
        bool cameraMatch = false;
        Error error1, error2;
        unsigned int i = 0;
        int nbTry = 0;
        do{
            error1 = cameraArray[id].Connect(&guidArray[i]);
            error2 = cameraArray[id].GetCameraInfo(&camInfoArray[id]);
            if(PGRERROR_OK != error1.GetType()){
                cout << "Failed to connect to camera n° " << i << endl;
                noProb = false;
                break;
            }
            if(PGRERROR_OK != error2.GetType()){
                cout << "Failed to retrieve informations from camera n° " << i << endl;
                noProb = false;
                break;
            }
            if(camInfoArray[id].serialNumber == serialNumber[id]){
                cameraMatch = true;
                break;
            }
            if(numCameras - 1 == i){
                i = 0;
            }else{
                i++;
            }
            nbTry++;
        }while(!cameraMatch && nbTry < 5);

        if(!cameraMatch){
            cout << "Error : unable to match cameras with their serial number" << endl;
            cout << "Please check serial number of your cameras, and serial number from serialNumber array" << endl;
            noProb = false;
        }else{
            cout << "Successfully connected to camera n° " << i << endl;
            cout << "=============== Camera n° " << id << " ==============" << endl;
			PrintCameraInfo(&camInfoArray[id]);
        }

	}

    if(!noProb){
        return -1;
    }
	
	
	string dateTemp = __DATE__; // get the date for videos name
	string date = replaceSpace(dateTemp); // replace spaces in date with underscores
	
	// create folder
	char* folder = const_cast<char*>(date.c_str());
    string make;
    if(WINDOWS){
        make = "md "; // Under Windows OS, we create a new folder with 'md' command
    }else{
        make = "mkdir "; // Under UNIX OS, we create a new folder with 'mkdir' command
    }
	char* foldermake = new char[strlen(folder) + strlen(make.c_str()) + 1];
	strcpy(foldermake, make.c_str());
	strcat(foldermake, folder);
	system(foldermake);

    if(bench){
        char* folderBench = const_cast<char*>("Bench");
        char* foldermakeBench = new char[strlen(folderBench) + strlen(make.c_str()) + 1];
        strcpy(foldermakeBench, make.c_str());
        strcat(foldermakeBench, folderBench);
        system(foldermakeBench);
    }

    string fileSeparator;
    if(WINDOWS){
        /* separators in Windows' directory is  \ */
        fileSeparator = "\\";
    }else{
        /* separator in UNIX's directory is / */
        fileSeparator = "/";
    }
	
	cout << "=============== Picture & Videos ==============" << endl;

    char chP;
    if(!bench){ // in BENCH mode, we do not take calibration pictures
        cout << "Do you want to take pictures for calibration (press SPACE to take pictures) ? (y/n)" << endl;
        cin >> chP;
    }else{
        chP = 'N';
    }

    char chV;
    if(!bench){ // in BENCH mode, we save up captured videos
        cout << "Do you want to take videos (video begins immediately) ? (y/n)" << endl;
        cin >>chV;
    }else{
        chV = 'Y';
    }

    string idArray[numCameras];
    for(unsigned int i = 0; i < numCameras; i++){
        idArray[i] = genRandomId(4);
    }

    // format of saved up pictures (calibration and standard pictures)
	const string format = ".jpg";
	
	//create image list
	cv::FileStorage fileStorageArray[numCameras];
	cv::FileStorage fileStorageStereo;
	
	cv::VideoWriter outputVideoArray[numCameras];

    string windowName = "Cameras : ";
    char key = 0;

    int counterArray[numCameras];

    Image rawImageArray[numCameras];
    Image rgbImageArray[numCameras];

    cv::Mat imageArray[numCameras];
    cv::Mat imageROI;

    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    
    timespec timespecArray[numCameras];
    
    string latenceFileName = "Bench" + fileSeparator + "latence_" + idArray[0] + ".bench";
    string fpsFileName = "Bench" + fileSeparator + "fps_" + idArray[0] + ".bench";
    
    ofstream latenceFile(latenceFileName.c_str(), ios::app);
    ofstream fpsFile(fpsFileName.c_str(), ios::app);

    time_t start, end;

    if(bench){
        cout << "Saving benchmarks files in folder 'Bench', with unique ID : " + idArray[0] << endl;

        for(unsigned int i = 0; i < numCameras; i++){
            latenceFile << "\"" << i << "\"";
        }
        latenceFile << endl;
    }
    

    // Launching parallel region with as many thread as number of camera
	#pragma omp parallel num_threads(numCameras)
	{
		int id = omp_get_thread_num(); // we get the ID of the thread
		
		int numCalib = 0;
        int numPic = 0;
	
		if('y' == chP || 'Y' == chP){
			const string list = "_List";
			const string format_list = ".yaml";
			string output_list = date + fileSeparator + date + "_camera_" + int2str(id) + list + format_list;
			fileStorageArray[id].open(output_list, cv::FileStorage::WRITE);
			fileStorageArray[id] << "images[";			
		}
		
		if('y' == chV || 'Y' == chV){
            idArray[id] = genRandomId(4);
			string name = date + fileSeparator + date + "_camera_" + int2str(id) + "_" + idArray[id] +  ".avi";
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
            noProb = false;
        }else{
            if(PGRERROR_OK != errorArray[id].GetType()){
                cout << "Failed to start image capture (camera n° " << id << ")" << endl;
                noProb = false;
            }
        }
        

        // Use only if we want to display a window while capturing video
        #pragma omp master
        {
            if(display){ // if user wants a display, creates a window
                for(unsigned int i = 0; i < numCameras-1; i++){
                    windowName += "  " + int2str(i) + "  -";
                }
                windowName += "  " + int2str(numCameras - 1);
                cv::namedWindow(windowName, CV_WINDOW_NORMAL); // we create an OpenCV window with windowName as window's title
            }
            if(bench){
                time(&start); // we start the timer for FPS benchmark
            }
        }

        
        // all threads wait for the master thread
        //#pragma omp barrier

        counterArray[id] = 0;

        // replace 'key != 27' with 'counter < nbFrame' when there is no display or when you want to capture a certain number of frames,
        // even if the display option is ON
        while(key != 27 /*counter < nbFrame*/){

            if(bench){
                counterArray[id]++;
            }
          
            #pragma omp barrier

            errorArray[id] = cameraArray[id].RetrieveBuffer(&rawImageArray[id]);
            if(bench){
                // saving time at when thread retrieves frame
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timespecArray[id]);
            }
            if(PGRERROR_OK != errorArray[id].GetType()){
                cout << "Capture error (camera n° " << id << ")" << endl;
                noProb = false;
            }
            
            //#pragma omp barrier

            if(bench){
                // we wait for all threads to end their RetrieveBuffer()
                #pragma omp barrier

                #pragma omp single
                {
                    // writing the number of the frame (used for R statistics)
                    // a file will look like :
                    //            "t1"               "tN"
                    // "numFrame" sec.nanoSec [...]  sec.nanoSec
                    // with N = numCameras
                    latenceFile << "\"" << counterArray[0] << "\"";
                    for(unsigned int i = 0; i < numCameras; i++){
                        latenceFile << " " << timespecArray[i].tv_sec << "." << timespecArray[i].tv_nsec;
                    }
                    latenceFile << endl;
                }
            }

            // convert the raw image to a BGR image
            rawImageArray[id].Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImageArray[id]);
            unsigned int rowBytes = (double)rgbImageArray[id].GetReceivedDataSize() / (double)rgbImageArray[id].GetRows();
            // create a matrix with the new BGR image
            imageArray[id] = cv::Mat(rgbImageArray[id].GetRows(), rgbImageArray[id].GetCols(), CV_8UC3, rgbImageArray[id].GetData(), rowBytes);

            #pragma omp barrier

            // Used only if user wants a display of the acquisition
            #pragma omp single
            {
                if(display){
                    string s = "Cameras :  ";
                    int totalWidth = 0; // total width of the window
                    int maxHeight = 0; // max height of the window
                    int offset = 0; // offset to put the next image in the window
                    for(unsigned int i = 0; i < numCameras; i++){
                        totalWidth += imageArray[i].size().width; // total width equals to the cumsum of all image's width
                        int height = imageArray[i].size().height;
                        if(maxHeight < height){
                            maxHeight = height; // max height equals to the maximum height of all images
                        }
                        s += int2str(i) + "  ";
                    }

                    cv::Mat imageROI(maxHeight, totalWidth, CV_8UC3); // creating a matrix with adaptated size

                    for(unsigned int i = 0; i < numCameras; i++){
                        // positioning the image into the window
                        cv::Mat matrix(imageROI, cv::Rect(offset, 0, imageArray[i].size().width, imageArray[i].size().height));
                        imageArray[i].copyTo(matrix);
                        offset += imageArray[i].size().width; // increase the offset by the width of the recently placed image
                    }

                    imshow(windowName, imageROI); // display window
                }
                
            }
            
            

            
            // Calibration picture
            string time = __TIME__;
            if(' ' == key && ('y' == chP || 'Y' == chP)){
                string name = date + fileSeparator + date + "_calib_camera_" + int2str(id) + "_" + int2str(numCalib) + format;
                cout << "Save calibration image n° " <<time << endl;
                numCalib++;
                cv::imwrite(name, imageArray[id], compression_params); // write a calib picture
                
                fileStorageArray[id] << name;
            }

            // Common picture
            if(' ' == key && ('n' == chP || 'N' == chP)){
                string name = date + fileSeparator + date + "_camera_" + int2str(id) + "_pic_" + int2str(numPic) + format;
                cout << "Save common image n° " << numCalib << endl;
                numPic++;
                cv::imwrite(name, imageArray[id], compression_params); // write a common picture
            }

            if('y' == chV || 'Y' == chV){
                outputVideoArray[id].write(imageArray[id]); // write the image in the video file
            }

            #pragma omp single
            {
                if(display){
                    // decrease the value for the tests
                    // increase it for more "user-friendly" experience (the time gap to press ESC is longer)
                    // /!\ Increasing it too much leads to a heavy drop in FPS (and is quite pointless)
                    key = cv::waitKey(1);
                }
            }
            
        } // end of while loop
                
    } // end of parallel region

    
    if(!noProb){
        return -1;
    }

    double fps;
    double sec;

    if(bench){
        time(&end); // stoping timer for FPS benchmark
        cout << "Benchmarking FPS on " << counterArray[0] << " iterations" << endl;
        sec = difftime(end, start);
        fps = counterArray[0] / sec; // fps = number of frame / time
        cout << "Benchmark result : " << fps << " FPS" << endl;
        fpsFile << fps << endl;

        fpsFile.close();
        latenceFile.close();
        
    }

    // Calibration pic list
    if('y' == chP || 'Y' == chP){
        for(unsigned int i = 0; i < numCameras; i++){
            fileStorageArray[i] << "]";
        }
    }

    for(unsigned int i = 0; i < numCameras; i++){
        // releasing connected cameras / files
        outputVideoArray[i].release();
        errorArray[i] = cameraArray[i].StopCapture();
        cameraArray[i].Disconnect();
    }

    // Apply undistorsion on captured videos (based on the date and the id of the videos)
    for(unsigned int i = 0; i < numCameras; i++){
        undistortion(i, idArray[i], imageSize, date);
    }

    return 0;
}
