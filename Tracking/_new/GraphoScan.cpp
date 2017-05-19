#include <iostream>
#include <fstream>

#include "HOG.hpp"
#include "GraphoScan.hpp"

using namespace std;

#define MIN_RECT_WIDTH 50
#define MIN_RECT_HEIGHT 50

#define MYHOG

vector<Point2f> roi;
bool isFinished = false;
int mNum = 4;
string name_window="Tracking";

void GraphoScan::mySelectBg(const char * fileName_video, const char * fileName_pt_bg) {
  cv::namedWindow(name_window);
  cv::setMouseCallback(name_window, mouseSelectPoint);

  VideoCapture cap(fileName_video);
  cap.read(imgSrc);
  if (!cap.isOpened()) {
    cout << "Cannot open the file: " << fileName_video << endl;
    exit(1);
  }

  // initialize with matrix fill by 0
  cv::Mat temp = cv::Mat::zeros(imgSrc.size(), imgSrc.type());
	
  vector<cv::Point2f> vect;
  vect.clear();
  while (true) {
    for (int i = 0; i < roi.size(); i++) {
      cv::circle(temp, roi[i], 2, Scalar(0, 255, 0), 2);
      if (i > 0)
	line(temp, roi[i - 1], roi[i], Scalar(0, 255, 255), 2);
    }
		
    // isFinished set true if the 4 points are set
    if (isFinished) {
      line(temp, roi[roi.size() - 1], roi[0], Scalar(0, 255, 255), 2);

      // Sort the points in anti-clockwise
      Point2f v1 = roi[1] - roi[0];
      Point2f v2 = roi[2] - roi[0];

      if (v1.cross(v2) > 0)
	swap(roi[1], roi[3]);
	  
      vect = roi;
      vect.push_back(roi[0]);
      break;
    }

    cv::imshow(name_window, imgSrc + temp);

    if (waitKey(1) == 27) {
      cv::destroyWindow(name_window);
      exit(1);
    }
  }
  // if the selected zone is not square, add points to obtain perfect square
  insertPoints(vect, 20);
  
  // save new bg
  saveTrajectoire(vect, fileName_pt_bg);
}

void mouseSelectPoint(int event, int x, int y, int flags, void* userData) {
  switch (event) {
  case CV_EVENT_LBUTTONDOWN:
    if (!isFinished) {
      roi.push_back(Point2f(x, y));
      if (roi.size() >= mNum)
	isFinished = true;
    }
  default: break;
  }
}


void GraphoScan::myTracker(const char* filename,const string algo_name, bool isTransPerspective) {
  
  // if you need to save the video
  cout << "Save video? [y/n]: ";
  
  cin >> saveVideo;
  if ((saveVideo == 'y') || (saveVideo == 'Y')) {
    init_VideoWritter();
  }

  VideoCapture cap;
  cap.open(filename);
  
  if (!cap.isOpened()) {
    cout << "Cannot open the webcam." << endl;
    exit(1);
  }
  
  // number of frame in the video
  int nFrame = cap.get(CAP_PROP_FRAME_COUNT);
  // if you need to skip frames to set start at the begin of writting
  char skipFrame;
  cout << "if skip frames? [y/n]: ";
  cin >> skipFrame;
  int nFrameStart = 0;
  int nFrameEnd = nFrame;
	
  // permit to begin and finish at the frames numbers set before
  if ((skipFrame == 'y')|| (skipFrame == 'Y')) {
    while (true) {
      cout << "start from:";
      string str;
      cin >> str;
      nFrameStart = stoi(str);
      if (nFrameStart < 0) {
	cout << "<error> the number of the frame should be positive." << endl;
	continue;
      } else {
	// set proprety of start frame to the videocapture properties
	cap.set(CV_CAP_PROP_POS_FRAMES, nFrameStart);
	cout << "strat from " << nFrameStart << " frame" << endl;
	break;
      }
    }
    while (true) {
      cout << "end at: ";
      string str;
      cin >> str;
      nFrameEnd = stoi(str);
      if ((nFrameEnd <= nFrameStart) || (nFrameEnd > nFrame)) {
	cout << "nFrame = " << nFrame << endl;
	cout << "<error> nFrameEnd <= mFrameStart or nFrameEnd > nFrame." << endl;
	continue;
      } else {
	cout << "end at " << nFrameEnd << endl;
	break;
      }
    }
  }
  
  cap.read(imgSrc);

  // verify if the picture is empty
  if (imgSrc.empty()) {
    cout << "The frame is empty." << endl;
    waitKey();
    exit(1);
  } 

  if (isTransPerspective == false) {
    imgRoi = imgSrc;
  } else {
    // transformee perspective
    imgRoi = transformPerspective(imgSrc);
  }
  
  Rect2d box = selectROI("selectRoi", imgRoi, true, true);
  cv::destroyWindow("selectRoi");
  
  // limit the roi size
  if (box.width < MIN_RECT_WIDTH) {
    box.x += box.width * 0.5;
    box.width = MIN_RECT_WIDTH;
    box.x -= MIN_RECT_WIDTH* 0.5;
  }
  if (box.height < MIN_RECT_HEIGHT) {
    box.y += box.height* 0.5;
    box.height = MIN_RECT_HEIGHT;
    box.y -= MIN_RECT_HEIGHT * 0.5;
  }

  // boundary protection
  box &= Rect2d(0, 0, imgRoi.cols, imgRoi.rows);

  // quit if ROI was not selected
  if (box.width == 0 || box.height == 0) {
    cout << "error: no box was seleted." << endl;
    exit(1);
  }

  // create a tracker with algo_name in parameter
  Ptr<Tracker> tracker = Tracker::create(algo_name);
  
  tracker->init(imgRoi, box);

  //initialize matrix to record tracker trajectory
  cv::Mat imgBin, imgDst;
  imgTrajectoire = cv::Mat::zeros(imgRoi.size(), imgRoi.type());
  imgTrajectoire_cor = imgTrajectoire.clone();

  cv::namedWindow(name_window,CV_WINDOW_NORMAL);
  cv::setWindowProperty(name_window,CV_WND_PROP_FULLSCREEN,CV_WINDOW_FULLSCREEN);
  
  int frameCount = 0;
  double tempsCalc = (double)getTickCount();
  
  while (nFrameStart + frameCount < nFrameEnd) {
    if(cv::waitKey(1)==27) {
      cv::setWindowProperty(name_window,CV_WND_PROP_FULLSCREEN,CV_WINDOW_NORMAL);
    }
      
    double t = (double)getTickCount();

    cap.read(imgSrc);

    if (imgSrc.empty()) {
      cout << "The frame is empty." << endl;
      break;
    }

    t = (double)cvGetTickCount() - t;
    cout << "cost time(convert):" << t / ((double)getTickFrequency()*1000.f) << endl;

    if (isTransPerspective == false) {
      imgRoi = imgSrc;
    } else {
      // transformee perspective
      cv::warpPerspective(imgSrc, imgRoi, M, frameSize);
    }
		
    t = (double)cvGetTickCount() - t;
    std::cout << "cost time(calculation):" << t / ((double)getTickFrequency()*1000.f) << endl;

    // tracking
    tracker->update(imgRoi, box);

    cv::Point2f ptCenter = cv::Point2f(box.x + box.width*0.5, box.y + box.height*0.5);
    // draw trajectory of the following point
    cv::circle(imgTrajectoire, ptCenter, 0, Scalar(0, 255, 255), 2);

    // save points
    ptsObjet.push_back(ptCenter);

    cv::Mat imgSuivi = Mat::zeros(imgRoi.size(), imgRoi.type());

    cv::rectangle(imgSuivi, box, Scalar(0, 0, 255), 2);
    cv::circle(imgSuivi, ptCenter, 0, Scalar(0, 255, 255), 2);

    //record two pictures in only one window
		
    // window size 2 for two picutres
    cv::Mat imgAll = cv::Mat(imgRoi.rows, imgRoi.cols * 2, imgRoi.type());
    
    // roi picture delete on default picture
    cv::Mat imgTmp = imgRoi.clone() + imgSuivi.clone();
    
    // copy imgTmpto the first part of window
    imgTmp.copyTo(imgAll(Rect(0, 0, imgRoi.cols, imgRoi.rows)));
    
    // copy imgTrajectoire the first part of window
    imgTrajectoire.copyTo(imgAll(Rect(imgRoi.cols, 0, imgRoi.cols, imgRoi.rows)));

    cv::imshow(name_window, imgAll);
      
    if ((saveVideo == 'y') || (saveVideo == 'Y')) {
	
      // save videos
      outputVideo1.write(imgTrajectoire);
      outputVideo2.write(imgRoi + imgSuivi);
      outputVideo3.write(imgAll);
    }
      
    cout << "frameCount: " << frameCount++ << endl;

    t = (double)cvGetTickCount() - t;
    cout << "cost time(show images):" << t / ((double)getTickFrequency()*1000.f) << endl;

  }
  cv::destroyWindow(name_window);
  tempsCalc -= (double)getTickCount();
  cout << "The calculation has duration equals to : " << tempsCalc << endl;
}

void GraphoScan::saveTrajectoire(const char* filename = "objectsPoints.txt") {
  ofstream fout;
  fout.open(filename);
  if (!fout.is_open()) {
    cout << "Cannot create the file " << filename << "." << endl;
  } else {
    fout.clear();
    for (int i = 0; i < ptsObjet.size(); i++) {
      fout << ptsObjet[i].x << " " << ptsObjet[i].y << endl;
    }
  }
}

void GraphoScan::saveTrajectoire(vector<cv::Point2f> pts, const char * filenName) {
  ofstream fout;
  fout.open(filenName);
  if (!fout.is_open()) {
    cout << "Cannot create the file " << filenName << "." << endl;
  } else {
    fout.clear();
    for (int i = 0; i < pts.size(); i++) {
      fout << pts[i].x << " " << pts[i].y << endl;
    }
  }
}

void GraphoScan::calcImgPtsAndImgTrack() {
  
  string window_hog = "HOG";
  
#ifdef MYHOG
  
  //work on imgTrajectoire and set color to black and white
  imgTrajectoire_cor = getImgTrajectoire_Cor(ptsObjet);
	
  MyHog hog;
  hog.computeGradient(imgRoi);

  cv::Mat imgMag = hog.Mag[0].clone();
  int size = 3;

  cv::Mat kernel_Ero = getStructuringElement(MORPH_ELLIPSE, Size(size, size));
  cv::Mat kernel_Dil = kernel_Ero;

  cv::dilate(imgMag, imgMag, kernel_Dil);

  cout << "type of imgTrajectoire_cor: " << imgTrajectoire_cor.type() << endl;
  cout << "type of imgPtsObjet: " << imgPtsObjet.type() << endl;
  cout << "type of imgMag: " << imgMag.type() << endl;

#else
  
  cv::Mat imgBin;
  cvtColor(imgRoi, imgBin, CV_BGR2GRAY);

  threshold(imgBin, imgBin, 128, 256, CV_THRESH_BINARY_INV);
  
  int size = 3;
  cv::Mat kernel_Ero = getStructuringElement(MORPH_ELLIPSE, Size(size, size));
  cv::Mat kernel_Dil = kernel_Ero;

  cv::dilate(imgBin, imgPtsObjet, kernel_Dil);

  cout << "type of imgTrajectoire_cor: " << imgTrajectoire_cor.type() << endl;
  cout << "type of imgPtsObjet: " << imgPtsObjet.type() << endl;
  cout << "type of imgBin: " << imgPtsObjet.type() << endl;
  
#endif
}

void GraphoScan::showImgTrackAndHog(string imgTrack, string imgHog){
  string window_track_hog = "Tracking + HOG";
  cv::namedWindow(window_track_hog,CV_WINDOW_NORMAL);
  cv::setWindowProperty(window_track_hog,CV_WND_PROP_FULLSCREEN,CV_WINDOW_FULLSCREEN);
  
  Mat imageTrack = imread(imgTrack,CV_LOAD_IMAGE_COLOR);
  Mat imageHOG = imread(imgHog,CV_LOAD_IMAGE_COLOR);
  
  if(!imageTrack.data || !imageHOG.data){
    cout << "Could not open or find the tracking and/or hog image" << endl;
    return;
  }
  
  bool exit = false;
  
  while(!exit){
    cv::Mat imgAll = cv::Mat(imageTrack.rows, imageTrack.cols * 2, imageTrack.type());
    imageTrack.copyTo(imgAll(Rect(0, 0, imageTrack.cols, imageTrack.rows)));
    imageHOG.copyTo(imgAll(Rect(imageTrack.cols, 0, imageHOG.cols, imageHOG.rows)));
    cv::imshow(window_track_hog, imgAll);
    
    if(cv::waitKey(1)==27){
      cv::destroyWindow(window_track_hog);
      exit = true;
    }
    
  }
  
}

void GraphoScan::saveImgPtsAndImgTraject(const char* filename_imgPtsObjet,const char* filename_imgTrajectoire_cor) {

  // save picture with trajectory points
  cv::imwrite(filename_imgPtsObjet, imgTrajectoire);

  // save picture with HOG trajectory
  cv::imwrite(filename_imgTrajectoire_cor, imgTrajectoire_cor);

}

cv::Mat GraphoScan::getImgTrajectoire_Cor(vector<Point2f> ptsObjet) {
  
  cv::Mat imgTmp = cv::Mat::zeros(imgTrajectoire_cor.size(), imgTrajectoire_cor.type());
  int size = 3;
  
  cv::Mat kernel_Ero = getStructuringElement(MORPH_ELLIPSE, Size(size, size));
  cv::Mat kernel_Dil = kernel_Ero;

  // connect all the center points
  for (int i = 0; i < ptsObjet.size() - 1; i++) {
    line(imgTmp, ptsObjet[i], ptsObjet[i + 1], Scalar(0, 255, 255), 2);
  }

  // set picture with grayscale
  cv::cvtColor(imgTmp, imgTmp, CV_BGR2GRAY);

  // delete picture grayscale
  cv::threshold(imgTmp, imgTmp, 128, 256, CV_THRESH_BINARY);
  
  cv::dilate(imgTmp, imgTmp, kernel_Dil); 
  return imgTmp;
}

cv::Mat GraphoScan::transformPerspective(const cv::Mat & imgSrc) {
  cv::namedWindow("Src");
  cv::setMouseCallback("Src", mouseSelectPoint);

  cv::Mat temp = cv::Mat::zeros(imgSrc.size(), imgSrc.type());
  while (true) {
    for (int i = 0; i < roi.size(); i++) {
      cv::circle(temp, roi[i], 2, Scalar(0, 255, 0), 2);
      if (i > 0)
	line(temp, roi[i - 1], roi[i], Scalar(0, 255, 255), 2);
    }
    if (isFinished) {
      mRoi = roi;
      line(temp, mRoi[mRoi.size() - 1], mRoi[0], Scalar(0, 255, 255), 2);

      // Sort the points in anti-clockwise
      Point2f v1 = mRoi[1] - mRoi[0];
      Point2f v2 = mRoi[2] - mRoi[0];

      if (v1.cross(v2) > 0)
	swap(mRoi[1], mRoi[3]);
      break;
    }
      
    cv::imshow("Src", imgSrc + temp);
    if (waitKey(1) == 27)
      exit(1);
  }

  // create the cv::rectangle including the contour
  Rect2f rect = boundingRect(roi);

  cv::rectangle(temp, rect, Scalar(0, 0, 255), 2);

  // cv::imshow("Src", imgSrc + temp);

  // set roi shape to perfect square shape
  Point2f ptM0 = (roi[0] + roi[1]) / 2;
  Point2f ptM1 = (roi[1] + roi[2]) / 2;
  Point2f ptM2 = (roi[2] + roi[3]) / 2;
  Point2f ptM3 = (roi[3] + roi[0]) / 2;

  Point2f vm1 = ptM2 - ptM0;
  Point2f vm2 = ptM3 - ptM1;

  float l1 = sqrt(vm1.x*vm1.x + vm1.y*vm1.y);
  float l2 = sqrt(vm2.x*vm2.x + vm2.y*vm2.y);

  int rect_width = (int)l1;
  int rect_height = (int)l2;

  modelPoint.clear();
  modelPoint.push_back(Point2f(0.0f, 0.0f));
  modelPoint.push_back(Point2f(0.0f, rect_height));
  modelPoint.push_back(Point2f(rect_width, rect_height));
  modelPoint.push_back(Point2f(rect_width, 0.0f));

  // transforme perspective
  M = getPerspectiveTransform(roi, modelPoint);

  frameSize = Size(rect_width, rect_height);
  cv::warpPerspective(imgSrc, imgRoi, M, frameSize);

  return imgRoi;
}

void GraphoScan::readTracjectoire(const char* filename) {
  
  ifstream fin(filename);
  ptsObjet.clear();

  if (!fin.is_open()) {
    std::cout << "Cannot open the file " << filename << "." << endl;
    exit(1);
  } else {
    for (int i = 0; fin.good(); i++) {
      Point2f pt;
      fin >> pt.x;
      fin >> pt.y;
      ptsObjet.push_back(pt);
    }
  }
}

void GraphoScan::calAndSavePointsOf3D(Size sizeImg, const char* filename_left, const char* filename_right, const char* filename_output,const char* filename_outputZ) {

  // matlab data are all float which hard coded
  cv::Mat cameraMatrix1 = (cv::Mat_<double>(3, 3) << 1114.4, 0, 0,
                           0, 1110.7, 0,
                           668.7401, 419.1680, 1);
  cv::Mat cameraMatrix2 = (cv::Mat_<double>(3, 3) << 1108.2, 0, 0,
                           0, 1107.5, 0,
                           652.8420, 457.9152, 1);

  cv::Mat distortionMatrix1 = (cv::Mat_<double>(1, 5) << 8.64952, 31.6793, -0.433353, 0.68756, -2009.77);
  cv::Mat distortionMatrix2 = (cv::Mat_<double>(1, 5) << 0.100274, 0.72718, 0.0357212, 0.17021, -0.851439);

  cv::Mat rotationMatrix = (cv::Mat_<double>(3, 3) << 0.9997, 0.0063, -0.0247,
                            0.0052, 0.8985, 0.4389,
                            0.0250, -0.4389, 0.8982);
  cv::Mat translationMatrix = (cv::Mat_<double>(3, 1) << 32.1360, 222.3364, 19.5136);

  // R1,R2 rotation matrix
  // P1,P2 projection martix
  // Q =  PxR
  cv::Mat R1, R2, P1, P2, Q;
  
  cv::stereoRectify(cameraMatrix1, distortionMatrix1, cameraMatrix2, distortionMatrix2, sizeImg, rotationMatrix, translationMatrix,
                    R1, R2, P1, P2, Q);

  GraphoScan grapho_left, grapho_right;

  // read the points from files
  grapho_left.readTracjectoire(filename_left);
  grapho_right.readTracjectoire(filename_right);

  // save HOG trajectory points of each cameras
  vector<Point2f> ptsLeft = grapho_left.getPtsTrajectoire();
  vector<Point2f> ptsRight = grapho_right.getPtsTrajectoire();

  if (ptsLeft.size() != ptsRight.size()) {
    cout << "ptsLeft.size() != ptsRight.size()." << endl;
    cout << "ptsLeft.size() = " << ptsLeft.size() << endl;
    cout << "ptsRight.size() = " << ptsRight.size() << endl;
    exit(1);
  }

  int size = ptsLeft.size();
  vector<cv::Mat> v;
  for (int i = 0; i < size; i++) {
    vector<Point2f> vectLeftTmp;
    vectLeftTmp.push_back(ptsLeft[i]);

    vector<Point2f> vectRightTmp;
    vectRightTmp.push_back(ptsRight[i]);

    cv::Mat M;
      
    cv::triangulatePoints(P1, P2, vectLeftTmp, vectRightTmp, M);
    std::cout << "Num - " << i << ": " << M << endl;
    v.push_back(M.clone());
  }

  
  ofstream f(filename_output);
  if (!f.is_open()) {
    std::cout << "Cannot open the file" << endl;
    exit(1);
  } else {
    f.clear();
    for (int i = 0; i < v.size(); i++) {
      float* ptr = v[i].ptr<float>(0);
      f << ptr[0] << " " << ptr[1] << " " << ptr[2] << " " << ptr[3] << endl;
    }
  }

  ofstream fz(filename_outputZ);
  
  if(!fz.is_open()) {
    std::cout << "Cannot open the Z file" << endl;
    exit(1);
  } else {
    f.clear();
    for (int i = 0; i < v.size(); i++) {
      float* ptr = v[i].ptr<float>(0);
      float fi= i*1.0f;
      fz << ptr[0] << " " << ptr[1] << " " << i/2000.0f << " " << ptr[3] << endl;
    } 
    
  }

}

void GraphoScan::selectPointManuel(const char* filename) {
  
  VideoCapture cap(filename);
  if (!cap.isOpened()) {
    cout << "Cannot open the file:" << filename << endl;
    exit(1);
  }
  
  int nFrame = cap.get(CAP_PROP_FRAME_COUNT);
  
  char skipFrame;
  cout << "if skip frames? [y/n]: ";
  cin >> skipFrame;
  int nFrameStart = 0;
  int nFrameEnd = nFrame;
  if ((skipFrame == 'y') || (skipFrame == 'Y')) {
    while (true) {
      cout << "start from:";
      string str;
      cin >> str;
      nFrameStart = stoi(str);
      if (nFrameStart < 0) {
	cout << "<error> the number of the frame should be positive." << endl;
	continue;
      } else {
	cap.set(CV_CAP_PROP_POS_FRAMES, nFrameStart);
	cout << "strat from " << nFrameStart << " frame" << endl;
	break;
      }
    }

    while (true) {
      cout << "end at: ";
      string str;
      cin >> str;
      nFrameEnd = stoi(str);
      if ((nFrameEnd <= nFrameStart) || (nFrameEnd > nFrame)) {
	cout << "<error> nFrameEnd <= mFrameStart or nFrameEnd > nFrame." << endl;
	continue;
      } else {
	cout << "end at " << nFrameEnd << endl;
	break;
      }
    }
  }

  cv::Mat imgSrc;
  ptsObjet.clear();
  for (int frameCount = 0; nFrameStart + frameCount < nFrameEnd; frameCount++) {
    cap.read(imgSrc);		
    Rect2d box = selectROI(imgSrc);
      
    cv::Point2f ptCenter = cv::Point2f(box.x + box.width*0.5, box.y + box.height*0.5);
    ptsObjet.push_back(ptCenter);
      
    cout << "nFrame = " << nFrameStart + frameCount << endl;
  }
}

void GraphoScan::insertPoints(int n) {
  if (ptsObjet.size() <= 1) {
    cout << "ptsObjet.size() <= 1" << endl;
    exit(1);
  }

  vector<cv::Point2f> vect;
  float eps = 20;

  vect.clear();
  
  for (size_t i = 0; i < ptsObjet.size() - 1; i++) {
    cv::Point2f pt1 = ptsObjet[i];
    cv::Point2f pt2 = ptsObjet[i + 1];
    
    cv::Point2f v = pt2 - pt1;
    vect.push_back(pt1);
    float dd = sqrt(v.x*v.x + v.y*v.y);
    float step_x = v.x / n;
    float step_y = v.y / n;
    
    for (size_t i = 1; i < n; i++) {
      cv::Point2f pt = cv::Point2f(pt1.x + i * step_x, pt1.y + i*step_y);
      vect.push_back(pt);
    }
  }
  ptsObjet = vect;
}

void GraphoScan::insertPoints(vector<Point2f>& pts, int n) {
  if (pts.size() <= 1) {
    std::cout << "pts.size() <= 1" << endl;
    exit(1);
  }

  vector<cv::Point2f> vect;
  float eps = 20;

  vect.clear();
  for (size_t i = 0; i < pts.size() - 1; i++) {
    cv::Point2f pt1 = pts[i];
    cv::Point2f pt2 = pts[i + 1];

    cv::Point2f v = pt2 - pt1;
    vect.push_back(pt1);
      
    // distance between pt2 and pt1
    float dd = sqrt(v.x*v.x + v.y*v.y);
    float step_x = v.x / n;
    float step_y = v.y / n;

    for (size_t i = 1; i < n; i++) {
      cv::Point2f pt = cv::Point2f(pt1.x + i * step_x, pt1.y + i*step_y);
      vect.push_back(pt);
    }

  }
  pts = vect;
}

void GraphoScan::init_VideoWritter() {
  
  cout << "Video will be saved" << endl;
  
  outputVideo1.open(NAME_L, CV_FOURCC('X', 'V', 'I', 'D'), 10, frameSize, true);
  if (!outputVideo1.isOpened()) {
    cout << "could not open the output video1 for write" << endl;
    return;
  }

  outputVideo2.open(NAME_R, CV_FOURCC('X', 'V', 'I', 'D'), 10, frameSize, true);
  if (!outputVideo2.isOpened()) {
    cout << "could not open the output video2 for write" << endl;
    return;
  }

  outputVideo3.open(NAME_ALL, CV_FOURCC('X', 'V', 'I', 'D'), 10, Size(frameSize.width * 2, frameSize.height), true);
  if (!outputVideo3.isOpened()) {
    cout << "could not open the output video3 for write" << endl;
    return;
  }
}

void GraphoScan::init() {
  for (int i = 0; i < 256; i++) {
    lut[i] = saturate_cast<uchar>(pow((float)(i / 255.0f), fgamma)*255.0f);
  }
  mRoi.clear();
}

vector<Point2f> GraphoScan::getPtsTrajectoire() {
  return ptsObjet;
}
