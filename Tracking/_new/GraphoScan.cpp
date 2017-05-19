#include <iostream>
#include <fstream>

#include "HOG.hpp"

#include "GraphoScan.hpp"

using namespace cv;
using namespace std;

#define MIN_RECT_WIDTH 50
#define MIN_RECT_HEIGHT 50

#define MYHOG

vector<Point2f> roi;
bool isFinished = false;
int mNum = 4;
string name_window="Tracking";

// selectionne un quadrilatère pour possiblement ne travailler à l'intérieur de celui ci
void GraphoScan::mySelectBg(const char * fileName_video, const char * fileName_pt_bg){
  cv::namedWindow(name_window);
  cv::setMouseCallback(name_window, mouseSelectPoint);

  VideoCapture cap(fileName_video);
  cap.read(imgSrc);
  if (!cap.isOpened())
    {
      std::cout << "Cannot open the file: " << fileName_video << endl;
      exit(1);
    }

  cv::Mat temp = cv::Mat::zeros(imgSrc.size(), imgSrc.type());
	
  vector<cv::Point2f> vect;
  vect.clear();
  while (true)
    {
      for (int i = 0; i < roi.size(); i++)
	{
	  cv::circle(temp, roi[i], 2, Scalar(0, 255, 0), 2);
	  if (i > 0)
	    line(temp, roi[i - 1], roi[i], Scalar(0, 255, 255), 2);
	}
		
      //isFinished est mis ¨¤ true quand les 4 points ont ¨¦t¨¦ selectionn¨¦s ¨¤ la souris
      if (isFinished)
	{
	  line(temp, roi[roi.size() - 1], roi[0], Scalar(0, 255, 255), 2);

	  //Sort the points in anti-clockwise
	  Point2f v1 = roi[1] - roi[0];
	  Point2f v2 = roi[2] - roi[0];

	  if (v1.cross(v2) > 0)
	    swap(roi[1], roi[3]);
	  vect = roi;
	  vect.push_back(roi[0]);
	  break;
	}

      cv::imshow(name_window, imgSrc + temp);

      if (waitKey(1) == 27){
	cv::destroyWindow(name_window);
	exit(1);
      }
    }
  //ajoute des points pour obtenir une zone carr¨¦e (maybe)
  insertPoints(vect, 20);
  //sauvegarde vect dans un fichier
  saveTrajectoire(vect, fileName_pt_bg);

}

void mouseSelectPoint(int event, int x, int y, int flags, void* userData)
{
  switch (event)
    {
    case CV_EVENT_LBUTTONDOWN:
      if (!isFinished)
	{
	  roi.push_back(Point2f(x, y));
	  if (roi.size() >= mNum)
	    isFinished = true;
	}
    default:
      break;
    }
}

//prend une video en entr¨¦e, applique dessus le tracking KCF
//en sortie : - une video compos¨¦e de la video originale et du tracker
//			  - les points correspondants au centre du tracker dans les frames successives
//			  - une video compos¨¦e des 2 videos precedentes
void GraphoScan::myTrackerKCF(const char* filename, bool isTransPerspective)
{
  //si enregistrer le video
  std::cout << "Save video? [y/n]: ";
  cin >> saveVideo;
  if ((saveVideo == 'y') || (saveVideo == 'Y'))
    {
      init_VideoWritter();
    }

  VideoCapture cap;
  cap.open(filename);

  //my webCam
  //cap.open(0);

  if (!cap.isOpened())
    {
      std::cout << "Cannot open the webcam." << endl;
      exit(1);
    }
  //nombre de frame de la video
  int nFrame = cap.get(CAP_PROP_FRAME_COUNT);
  //si sauter des frames?
  char skipFrame;
  std::cout << "if skip frames? [y/n]: ";
  cin >> skipFrame;
  int nFrameStart = 0;
  int nFrameEnd = nFrame;
	
  //permet de commencer et de finir aux frames voulues
  if ((skipFrame == 'y')|| (skipFrame == 'Y'))
    {
      while (true)
	{
	  std::cout << "start from:";
	  string str;
	  cin >> str;
	  nFrameStart = stoi(str);
	  if (nFrameStart < 0)
	    {
	      cout << "<error> the number of the frame should be positive." << endl;
	      continue;
	    }
	  else
	    {
	      //set ajoute une propri¨¦t¨¦ ¨¤ la VideoCapture, ici la prochaine frame a ¨ºtre d¨¦cod¨¦e
	      cap.set(CV_CAP_PROP_POS_FRAMES, nFrameStart);
	      std::cout << "strat from " << nFrameStart << " frame" << endl;
	      break;
	    }
	}
      while (true)
	{
	  std::cout << "end at: ";
	  string str;
	  cin >> str;
	  nFrameEnd = stoi(str);
	  if ((nFrameEnd <= nFrameStart) || (nFrameEnd > nFrame))
	    {
	      std::cout << "nFrame = " << nFrame << endl;
	      std::cout << "<error> nFrameEnd <= mFrameStart or nFrameEnd > nFrame." << endl;
	      continue;
	    }
	  else
	    {
	      std::cout << "end at " << nFrameEnd << endl;
	      break;
	    }
	}
    }

  cap.read(imgSrc);

  //verifier si l'image est vide ou pas
  if (imgSrc.empty())
    {
      std::cout << "The frame is empty." << endl;
      waitKey();
      exit(1);
    }

  //before removing the distortion
  //cv::imshow("imgSrc_bf", imgSrc);

  //remove the distortion
  //cameraCalibrator.rectifyMap(imgSrc, imgSrc); 

  if (isTransPerspective == false)
    {
      imgRoi = imgSrc;
    }
  else
    {
      //transformee perspective
      imgRoi = transformPerspective(imgSrc);
    }

  //cv::namedWindow("ROI");
  //choisir la ROI
  //selectROI(windowName,img,showCrossair,fromCenter)
  Rect2d box = selectROI("selectRoi", imgRoi, true, true);
  cv::destroyWindow("selectRoi");
  //limiter la taille du rectangle de la ROI
  if (box.width < MIN_RECT_WIDTH)
    {
      box.x += box.width * 0.5;
      box.width = MIN_RECT_WIDTH;
      box.x -= MIN_RECT_WIDTH* 0.5;
    }
  if (box.height < MIN_RECT_HEIGHT)
    {
      box.y += box.height* 0.5;
      box.height = MIN_RECT_HEIGHT;
      box.y -= MIN_RECT_HEIGHT * 0.5;
    }

  //boundary protection
  box &= Rect2d(0, 0, imgRoi.cols, imgRoi.rows);

  //quit if ROI was not selected
  if (box.width == 0 || box.height == 0)
    {
      cout << "error: no box was seleted." << endl;
      exit(1);
    }

  //create a tracker 'KCF'
  Ptr<Tracker> tracker = Tracker::create("KCF");

  tracker->init(imgRoi, box);

  //initialiser le matrix pour enregistrer la trajectoire
  cv::Mat imgBin, imgDst;
  imgTrajectoire = cv::Mat::zeros(imgRoi.size(), imgRoi.type());
  imgTrajectoire_cor = imgTrajectoire.clone();

  cv::namedWindow(name_window,CV_WINDOW_NORMAL);
  cv::setWindowProperty(name_window,CV_WND_PROP_FULLSCREEN,CV_WINDOW_FULLSCREEN);
  
  int frameCount = 0;
  double tempsCalc = (double)getTickCount();
  for (;nFrameStart + frameCount < nFrameEnd;)
    {
      if(cv::waitKey(1)==27){
	cv::setWindowProperty(name_window,CV_WND_PROP_FULLSCREEN,CV_WINDOW_NORMAL);
      }
      
      double t = (double)getTickCount();

      cap.read(imgSrc);

      //remove the distortion
      //cameraCalibrator.rectifyMap(imgSrc, imgSrc);

      //verifier si l'image est vide
      if (imgSrc.empty())
	{
	  std::cout << "The frame is empty." << endl;
	  break;
	}

      t = (double)cvGetTickCount() - t;
      std::cout << "cost time(convert):" << t / ((double)getTickFrequency()*1000.f) << endl;

      //very slow
      //myGammaCorrection(imgGray, 0.5);

      //MatIterator_<uchar> it, end;
      //for (it = imgGray.begin<uchar>(), end = imgGray.end<uchar>(); it != end; it++)
      //{
      //	*it = lut[(*it)];
      //}

      if (isTransPerspective == false)
	{
	  imgRoi = imgSrc;
	}
      else
	{
	  //transformee perspective
	  cv::warpPerspective(imgSrc, imgRoi, M, frameSize);
	}
		
      /*
	ptCenter = centre de la roi
	Les 3 vid¨¦os:
	- imgTrajectoire = les points au centre de la ROI
	- imgSuivi = le rectangle de la ROI
	- imgSrc = video originale avec le temps pass¨¦
		
		
      */
		
      t = (double)cvGetTickCount() - t;
      std::cout << "cost time(calculation):" << t / ((double)getTickFrequency()*1000.f) << endl;

      //trackingKCF
      tracker->update(imgRoi, box);

      cv::Point2f ptCenter = cv::Point2f(box.x + box.width*0.5, box.y + box.height*0.5);
      //d¨¦ssiner le trajectoire du point que l'on suit
      cv::circle(imgTrajectoire, ptCenter, 0, Scalar(0, 255, 255), 2);

      //enregistrement des points
      ptsObjet.push_back(ptCenter);

      cv::Mat imgSuivi = Mat::zeros(imgRoi.size(), imgRoi.type());

      cv::rectangle(imgSuivi, box, Scalar(0, 0, 255), 2);
      cv::circle(imgSuivi, ptCenter, 0, Scalar(0, 255, 255), 2);
      
      //cv::imshow("ROI", imgRoi + imgSuivi);
      //cv::imshow("Trajectoire", imgTrajectoire);
      //cv::imshow("Src", imgSrc + temp);

      //enregistrer deux images dans une seule fen¨ºtre
		
      // fenetre de taille de 2 vid¨¦os
      cv::Mat imgAll = cv::Mat(imgRoi.rows, imgRoi.cols * 2, imgRoi.type());
      // image de la roi surimprim¨¦e sur l'image de base
      cv::Mat imgTmp = imgRoi.clone() + imgSuivi.clone();
      //copie imgTmp dans la 1ere moiti¨¦ d'imgAll
      imgTmp.copyTo(imgAll(Rect(0, 0, imgRoi.cols, imgRoi.rows)));
      //copie imgTrajectoire dans la 2eme moitie d'imgAll
      imgTrajectoire.copyTo(imgAll(Rect(imgRoi.cols, 0, imgRoi.cols, imgRoi.rows)));

      cv::imshow(name_window, imgAll);
      
      if ((saveVideo == 'y') || (saveVideo == 'Y'))
	//stocker les vid¨¦os
	{
	  outputVideo1.write(imgTrajectoire);
	  outputVideo2.write(imgRoi + imgSuivi);
	  outputVideo3.write(imgAll);
	}

      std::cout << "frameCount: " << frameCount++ << endl;

      t = (double)cvGetTickCount() - t;
      std::cout << "cost time(show images):" << t / ((double)getTickFrequency()*1000.f) << endl;

    }
  cv::destroyWindow(name_window);
  tempsCalc -= (double)getTickCount();
  std::cout << "le temps de calcul est : " << tempsCalc << endl;
}

void GraphoScan::saveTrajectoire(const char* filename = "objectsPoints.txt")
{
  ofstream fout;
  fout.open(filename);
  if (!fout.is_open())
    {
      std::cout << "Cannot create the file " << filename << "." << endl;

    }
  else
    {
      fout.clear();
      for (int i = 0; i < ptsObjet.size(); i++)
	{
	  fout << ptsObjet[i].x << " " << ptsObjet[i].y << endl;
	}
    }
}

void GraphoScan::saveTrajectoire(vector<cv::Point2f> pts, const char * filenName)
{
  ofstream fout;
  fout.open(filenName);
  if (!fout.is_open())
    {
      std::cout << "Cannot create the file " << filenName << "." << endl;

    }
  else
    {
      fout.clear();
      for (int i = 0; i < pts.size(); i++)
	{
	  fout << pts[i].x << " " << pts[i].y << endl;
	}
    }
}

//C
void GraphoScan::calcImgPtsAndImgTrack()
{
  string window_hog = "HOG";
#ifdef MYHOG
  //transforme l'image d'imgTrajectoire_cor :
  //les points sont reli¨¦s et mis en noirs et blancs
  imgTrajectoire_cor = getImgTrajectoire_Cor(ptsObjet);
  //  cv::imshow("imgTrajectoire_cor", imgTrajectoire_cor);
	
  //utiliser la caracteristique de Hog
  MyHog hog;
  hog.computeGradient(imgRoi);
  //cv::imshow("Mag[0]", hog.Mag[0]);

  cv::Mat imgMag = hog.Mag[0].clone();
  int size = 3;

  cv::Mat kernel_Ero = getStructuringElement(MORPH_ELLIPSE, Size(size, size));
  cv::Mat kernel_Dil = kernel_Ero;

  //erode(imgMag, imgMag, kernel_Ero);
  cv::dilate(imgMag, imgMag, kernel_Dil);

  //parce que le type de cet image n'est pas correspondant ¨¤ l'enregirement, il faut la transformation.
  std::cout << "type of imgTrajectoire_cor: " << imgTrajectoire_cor.type() << endl;
  std::cout << "type of imgPtsObjet: " << imgPtsObjet.type() << endl;
  std::cout << "type of imgMag: " << imgMag.type() << endl;

  /*
  cv::namedWindow(window_hog);

  int C = 1;
  int beta = 8;
  int alpha = 0;
  cv::createTrackbar("Constant", window_hog, &C, 50);
  cv::createTrackbar("Beta", window_hog, &beta, 10);
  cv::createTrackbar("Alpha", window_hog, &alpha, 50);
  while (true)
    {
      //cv::Mat mask = cv::Mat(imgMag.size(), CV_8UC1).setTo(-128);
      //cv::normalize(imgMag, imgMag, 255, 0, NORM_INF, -1, mask);

      //C = 1, beta = 5, or C = 2, beta = 10; alpha = 0
      // CV_8UC1 = uchar
      imgMag.convertTo(imgPtsObjet, CV_8UC1, (double)beta, (double)alpha);
      //cv::adaptiveThreshold(imgPtsObjet, imgPtsObjet, 255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 5, C);

      //C = 1, beta = 8, alpha = 0, 	imgMag.convertTo(imgTmp, CV_8UC1, (double)beta, (double)alpha);
      cv::threshold(imgPtsObjet, imgPtsObjet, C, 256, THRESH_BINARY);

      cv::dilate(imgPtsObjet, imgPtsObjet, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
      //erode(imgPtsObjet, imgPtsObjet, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

      // cv::imshow(window_hog, imgPtsObjet);

      if (waitKey(1) == 27)
	{
	  cv::destroyWindow(window_hog);
	break;
	}
	}*/
#else
  
  cv::Mat imgBin;
  cvtColor(imgRoi, imgBin, CV_BGR2GRAY);

  threshold(imgBin, imgBin, 128, 256, CV_THRESH_BINARY_INV);
  //cv::adaptiveThreshold(imgMag, imgMag, 255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 5, 2);

  //cv::imshow("imgBin", imgBin);

  int size = 3;
  cv::Mat kernel_Ero = getStructuringElement(MORPH_ELLIPSE, Size(size, size));
  cv::Mat kernel_Dil = kernel_Ero;

  //erode(imgMag, imgMag, kernel_Ero);
  cv::dilate(imgBin, imgPtsObjet, kernel_Dil);

  //parce que le type de cet image n'est pas correspondant ¨¤ l'enregirement, il faut la transformation.
  std::cout << "type of imgTrajectoire_cor: " << imgTrajectoire_cor.type() << endl;
  std::cout << "type of imgPtsObjet: " << imgPtsObjet.type() << endl;
  std::cout << "type of imgBin: " << imgPtsObjet.type() << endl;
  /*
  cv::namedWindow(window_hog);

  int C = 1;
  int beta = 8;
  int alpha = 0;
  cv::createTrackbar("Constant", window_hog, &C, 50);
  cv::createTrackbar("Beta", window_hog, &beta, 10);
  cv::createTrackbar("Alpha",window_hog , &alpha, 50);
  while (true)
    {

       cv::imshow(window_hog, imgPtsObjet);

      if (waitKey(1) == 27)
	{
	  cv::destroyWindow(window_hog);
	  return;
	}
    }
  */

  
#endif // MYHOG
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

void GraphoScan::saveImgPtsAndImgTraject(const char* filename_imgPtsObjet,const char* filename_imgTrajectoire_cor)
{
  //enregistrer l'image des points de la trajectoire
  cv::imwrite(filename_imgPtsObjet, imgTrajectoire);

  //enregistrer la trajectoire
  cv::imwrite(filename_imgTrajectoire_cor, imgTrajectoire_cor);

}

cv::Mat GraphoScan::getImgTrajectoire_Cor(vector<Point2f> ptsObjet)
{
  cv::Mat imgTmp = cv::Mat::zeros(imgTrajectoire_cor.size(), imgTrajectoire_cor.type());

  int size = 3;
  //une ellipse dans le rectangle size*size
  cv::Mat kernel_Ero = getStructuringElement(MORPH_ELLIPSE, Size(size, size));
  cv::Mat kernel_Dil = kernel_Ero;

  //connect all the center points
  for (int i = 0; i < ptsObjet.size() - 1; i++)
    {
      line(imgTmp, ptsObjet[i], ptsObjet[i + 1], Scalar(0, 255, 255), 2);
    }

  //transforme l'image en niveau de gris
  cv::cvtColor(imgTmp, imgTmp, CV_BGR2GRAY);

  //supprime les niveaux de gris pour avoir soit du blanc soit du noir
  cv::threshold(imgTmp, imgTmp, 128, 256, CV_THRESH_BINARY);
  //arrondit les angles 
  cv::dilate(imgTmp, imgTmp, kernel_Dil);
  //redondant car affich¨¦ apr¨¨s
  //cv::imshow("imgTmp", imgTmp); 
  return imgTmp;
}

cv::Mat GraphoScan::transformPerspective(const cv::Mat & imgSrc) {
  cv::namedWindow("Src");
  cv::setMouseCallback("Src", mouseSelectPoint);

  cv::Mat temp = cv::Mat::zeros(imgSrc.size(), imgSrc.type());
  while (true)
    {
      for (int i = 0; i < roi.size(); i++)
	{
	  cv::circle(temp, roi[i], 2, Scalar(0, 255, 0), 2);
	  if (i > 0)
	    line(temp, roi[i - 1], roi[i], Scalar(0, 255, 255), 2);
	}
      if (isFinished)
	{
	  mRoi = roi;
	  line(temp, mRoi[mRoi.size() - 1], mRoi[0], Scalar(0, 255, 255), 2);

	  //Sort the points in anti-clockwise
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
  //setMouseCallback("Src", NULL);

  //create the cv::rectangle including the contour
  Rect2f rect = boundingRect(roi);

  cv::rectangle(temp, rect, Scalar(0, 0, 255), 2);

  //cv::imshow("Src", imgSrc + temp);

  //ajuster le rectangle de la ROI
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

  //transformee perspective
  M = getPerspectiveTransform(roi, modelPoint);

  frameSize = Size(rect_width, rect_height);
  cv::warpPerspective(imgSrc, imgRoi, M, frameSize);

  return imgRoi;
}



//
void GraphoScan::readTracjectoire(const char* filename)
{
  ifstream fin(filename);
  ptsObjet.clear();

  if (!fin.is_open())
    {
      std::cout << "Cannot open the file " << filename << "." << endl;
      exit(1);
    }
  else
    {
      for (int i = 0; fin.good(); i++)
	{
	  Point2f pt;
	  fin >> pt.x;
	  fin >> pt.y;
	  ptsObjet.push_back(pt);
	}
    }
}

vector<Point2f> GraphoScan::getPtsTrajectoire()
{
  return ptsObjet;
}

void GraphoScan::calAndSavePointsOf3D(Size sizeImg, const char* filename_left,
                                      const char* filename_right, const char* filename_output,
                                      const char* filename_outputZ)
{
  /* ========= 1 ========= */
  //	cv::Mat cameraMatrix1 = (cv::Mat_<double>(3, 3) << 1105.44288, 0, 675.41065, 0, 1107.46806, 449.85884, 0, 0, 1);
  //	cv::Mat cameraMatrix2 = (cv::Mat_<double>(3, 3) << 1097.27838, 0, 653.73491, 0, 1100.13557, 462.01760, 0, 0, 1);
  //
  //	cv::Mat distortionMatrix1 = (cv::Mat_<double>(1, 5) << -0.25772, 0.16186, -0.00084, -0.00165, 0.00000);
  //	cv::Mat distortionMatrix2 = (cv::Mat_<double>(1, 5) << -0.23751, 0.09606, -0.00040, -0.00194, 0.00000);
  //
  //	cv::Mat rotation3_1 = (cv::Mat_<double>(3, 1) << 0.41045, 0.03139, -0.02397);
  //	cv::Mat rotationMatrix;
  //	cv::Rodrigues(rotation3_1, rotationMatrix);
  //	cv::Mat translationMatrix = (cv::Mat_<double>(3, 1) << 21.00185, 178.03986, 18.90310);

  /* ========= 2 ========= */
  //cv::Mat cameraMatrix1 = (cv::Mat_<double>(3, 3) << 1089.44583, 0, 668.75803, 0, 1091.90131, 468.74164, 0, 0, 1);
  //cv::Mat cameraMatrix2 = (cv::Mat_<double>(3, 3) << 1095.15709, 0, 652.91437, 0, 1098.01808, 462.40336, 0, 0, 1);

  //cv::Mat distortionMatrix1 = (cv::Mat_<double>(1, 5) << -0.23212, 0.07308, 0.00120, -0.00133, 0.00000);
  //cv::Mat distortionMatrix2 = (cv::Mat_<double>(1, 5) << -0.25283, 0.13401, -0.00077, -0.00013, 0.00000);

  //cv::Mat rotation3_1 = (cv::Mat_<double>(3, 1) << -0.00003, 0.00041, 0.00611);
  //cv::Mat rotationMatrix;
  //cv::Rodrigues(rotation3_1, rotationMatrix);
  //cv::Mat translationMatrix = (cv::Mat_<double>(3, 1) << -0.82428, 59.62611, 1.73222);

  /* ========= 3 ========= */
	
	
  //Donn¨¦es matlab fournies en dur 
  //a modifier
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

  cv::Mat R1, R2, P1, P2, Q;
  //stereoRectify (cameraMatrix1,camera1 distortion,cameraMatrix2, camera2 distortion,taille de l'image,matrice de rotation entre les 2 cameras,
  // vecteur de translation entre les 2 cameras, R1: matrice de rotation 1ere cam, R2: matrice de rotation 2eme cam, P1: matrice de projection 1ere cam,
  // P2: matrice de projection 2eme cam,Q: matrice obtenue pour une projection dans un espace 3D)
  cv::stereoRectify(cameraMatrix1, distortionMatrix1, cameraMatrix2, distortionMatrix2, sizeImg, rotationMatrix, translationMatrix,
                    R1, R2, P1, P2, Q);

  //OpenCV
  GraphoScan grapho_left, grapho_right;

  //read the points from files
  grapho_left.readTracjectoire(filename_left);
  grapho_right.readTracjectoire(filename_right);

  //recupere dans ptsR/L les points du tracking de chaque camera
  vector<Point2f> ptsLeft = grapho_left.getPtsTrajectoire();
  vector<Point2f> ptsRight = grapho_right.getPtsTrajectoire();

  if (ptsLeft.size() != ptsRight.size())
    {
      std::cout << "ptsLeft.size() != ptsRight.size()." << endl;
      std::cout << "ptsLeft.size() = " << ptsLeft.size() << endl;
      std::cout << "ptsRight.size() = " << ptsRight.size() << endl;
      exit(1);
    }

  int size = ptsLeft.size();
  vector<cv::Mat> v;
  for (int i = 0; i < size; i++)
    {
      vector<Point2f> vectLeftTmp;
      vectLeftTmp.push_back(ptsLeft[i]);

      vector<Point2f> vectRightTmp;
      vectRightTmp.push_back(ptsRight[i]);

      cv::Mat M;
      //triangulatePoints(projection cam1, projection cam2, pts cam1, pts cam2, pts3D)
      cv::triangulatePoints(P1, P2, vectLeftTmp, vectRightTmp, M);
      std::cout << "Num - " << i << ": " << M << endl;
      v.push_back(M.clone());
    }
	
  //cv::Mat M;
  //cv::triangulatePoints(P1, P2, ptsLeft, ptsRight, M);

  ofstream f(filename_output);
  if (!f.is_open())
    {
      std::cout << "Cannot open the file" << endl;
      exit(1);
    }
  else
    {
      f.clear();
      for (int i = 0; i < v.size(); i++)
        {
          float* ptr = v[i].ptr<float>(0);
          f << ptr[0] << " " << ptr[1] << " " << ptr[2] << " " << ptr[3] << endl;
        }
     }

  ofstream fz(filename_outputZ);
  if(!fz.is_open()){
    std::cout << "Cannot open the Z file" << endl;
    exit(1);
  }else{
    f.clear();
    for (int i = 0; i < v.size(); i++)
      {
        float* ptr = v[i].ptr<float>(0);
        float fi= i*1.0f;
        fz << ptr[0] << " " << ptr[1] << " " << i/2000.0f << " " << ptr[3] << endl;
      }
    
  }

}

void GraphoScan::selectPointManuel(const char* filename)
{
  VideoCapture cap(filename);
  if (!cap.isOpened())
    {
      cout << "Cannot open the file:" << filename << endl;
      exit(1);
    }
  int nFrame = cap.get(CAP_PROP_FRAME_COUNT);

  //si sauter des frames?
  char skipFrame;
  std::cout << "if skip frames? [y/n]: ";
  cin >> skipFrame;
  int nFrameStart = 0;
  int nFrameEnd = nFrame;
  if ((skipFrame == 'y') || (skipFrame == 'Y'))
    {
      while (true)
	{
	  std::cout << "start from:";
	  string str;
	  cin >> str;
	  nFrameStart = stoi(str);
	  if (nFrameStart < 0)
	    {
	      cout << "<error> the number of the frame should be positive." << endl;
	      continue;
	    }
	  else
	    {
	      cap.set(CV_CAP_PROP_POS_FRAMES, nFrameStart);
	      std::cout << "strat from " << nFrameStart << " frame" << endl;
	      break;
	    }
	}

      while (true)
	{
	  std::cout << "end at: ";
	  string str;
	  cin >> str;
	  nFrameEnd = stoi(str);
	  if ((nFrameEnd <= nFrameStart) || (nFrameEnd > nFrame))
	    {
	      std::cout << "<error> nFrameEnd <= mFrameStart or nFrameEnd > nFrame." << endl;
	      continue;
	    }
	  else
	    {
	      std::cout << "end at " << nFrameEnd << endl;
	      break;
	    }
	}
    }

  cv::Mat imgSrc;
  int frameCount = 0;
  ptsObjet.clear();
  for (; nFrameStart + frameCount < nFrameEnd; frameCount++)
    {
      cap.read(imgSrc);
		
      Rect2d box = selectROI(imgSrc);
		
      cv::Point2f ptCenter = cv::Point2f(box.x + box.width*0.5, box.y + box.height*0.5);
		
      ptsObjet.push_back(ptCenter);
		
      std::cout << "nFrame = " << nFrameStart + frameCount << endl;
    }
}

//a tester : 1er = ¨¦paissir le trait 
//			 2eme = donner un effet de plume
//           3eme = adoucir les droites
void GraphoScan::insertPoints(int n)
{
  if (ptsObjet.size() <= 1)
    {
      std::cout << "ptsObjet.size() <= 1" << endl;
      exit(1);
    }

  vector<cv::Point2f> vect;
  float eps = 20;

  vect.clear();
  for (size_t i = 0; i < ptsObjet.size() - 1; i++)
    {
      cv::Point2f pt1 = ptsObjet[i];
      cv::Point2f pt2 = ptsObjet[i + 1];

      cv::Point2f v = pt2 - pt1;
      vect.push_back(pt1);
      float dd = sqrt(v.x*v.x + v.y*v.y);
      float step_x = v.x / n;
      float step_y = v.y / n;

      for (size_t i = 1; i < n; i++)
	{
	  cv::Point2f pt = cv::Point2f(pt1.x + i * step_x, pt1.y + i*step_y);
	  vect.push_back(pt);
	}
		
    }
  ptsObjet = vect;
}

//pts = 4 points du quadrilat¨¨re de selection
void GraphoScan::insertPoints(vector<Point2f>& pts, int n)
{
  if (pts.size() <= 1)
    {
      std::cout << "pts.size() <= 1" << endl;
      exit(1);
    }

  vector<cv::Point2f> vect;
  float eps = 20;

  vect.clear();
  for (size_t i = 0; i < pts.size() - 1; i++)
    {
      cv::Point2f pt1 = pts[i];
      cv::Point2f pt2 = pts[i + 1];

      cv::Point2f v = pt2 - pt1;
      vect.push_back(pt1);
      //distance entre pt2 et pt1
      float dd = sqrt(v.x*v.x + v.y*v.y);
      float step_x = v.x / n;
      float step_y = v.y / n;

      for (size_t i = 1; i < n; i++)
	{
	  cv::Point2f pt = cv::Point2f(pt1.x + i * step_x, pt1.y + i*step_y);
	  vect.push_back(pt);
	}

    }
  pts = vect;
}

void GraphoScan::init_VideoWritter() {
  std::cout << "Video will be saved" << endl;
  outputVideo1.open(NAME_L, CV_FOURCC('X', 'V', 'I', 'D'), 10, frameSize, true);
  if (!outputVideo1.isOpened()) {
    std::cout << "could not open the output video1 for write" << endl;
    return;
  }

  //problem here => outputVideo1 is bigger than outputVideo
  outputVideo2.open(NAME_R, CV_FOURCC('X', 'V', 'I', 'D'), 10, frameSize, true);
  if (!outputVideo2.isOpened()) {
    std::cout << "could not open the output video2 for write" << endl;
    return;
  }

  outputVideo3.open(NAME_ALL, CV_FOURCC('X', 'V', 'I', 'D'), 10, Size(frameSize.width * 2, frameSize.height), true);
  if (!outputVideo3.isOpened()) {
    std::cout << "could not open the output video3 for write" << endl;
    return;
  }
}

void GraphoScan::init() {
  for (int i = 0; i < 256; i++) {
    lut[i] = saturate_cast<uchar>(pow((float)(i / 255.0f), fgamma)*255.0f);
  }
  mRoi.clear();
}
