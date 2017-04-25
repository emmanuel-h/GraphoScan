#include "tracking.hpp"
#include "roiSelector.hpp"

int runTracking(string algo,string video ){
	
	Mat frame;
	vector<Rect2d> objects;

	MultiTracker trackers(algo);
	VideoCapture cap(video);
	cap >> frame;

	float frame_width = frame.size().width;
	float frame_height = frame.size().height;
	cout << frame_width << " " << frame_height << endl;
	
	
	cout << "Selectionner la ou les zones de capture \n ESPACE: valider la zone \n ECHAP: Quitter la selection " << endl;
	
	selectROI("tracker",frame,objects);

	// Mettre limitation de fenetre !

	if (objects.size()<1){
		cout << "Vous n'avez rien selectionné" << endl;
		return 0;
	}
	trackers.add(frame,objects);
	
	cout << "Debut du tracking" << endl;
	vector<Point2f> ptsObjet;
	vector<Point2f> ptsObjet2;
	bool end = true;

	Mat imgTrajectoire;
	
	while (end){
		cap >> frame;
		if (frame.rows > 0 && frame.cols > 0){
		  imgTrajectoire = cv::Mat::zeros(frame.size(), frame.type());
		  trackers.update(frame);
		  Point2f ptCenter = Point2f(trackers.objects[0].x + trackers.objects[0].width*0.5,
					     trackers.objects[0].y + trackers.objects[0].height*0.5);
		  Point2f ptCenter2 =  Point2f(trackers.objects[1].x + trackers.objects[1].width*0.5,
					     trackers.objects[1].y + trackers.objects[1].height*0.5);
		  circle(imgTrajectoire, ptCenter, 0, Scalar(0, 255, 255), 2);
		  ptsObjet.push_back(ptCenter);
		  ptsObjet2.push_back(ptCenter2);
		  for (unsigned int i=0; i<trackers.objects.size();i++){
		    rectangle(frame,trackers.objects[i],Scalar(255,0,0),2,1);
		  }
		  imshow("tracker",frame);
		  if (waitKey(1)==27)
		    end = false;
		}else{
		  end = false;
		}
	}
	
	ofstream fout;
	const char * filename = "test_sortie.txt";
	fout.open(filename);
	if (!fout.is_open()) {
	  cout << "Cannot create the file " << filename << "." << endl;
	  
	} else {
	  fout.clear();
	  for (unsigned int i = 0; i < ptsObjet.size(); i++) {
	    fout << (ptsObjet[i].x - frame_width/2 )  << " " << (ptsObjet[i].y -frame_height/2 )  << endl;
	  }
	}
	fout.close();
	 filename = "test_sortie2.txt";
	fout.open(filename);
	if (!fout.is_open()) {
	  cout << "Cannot create the file " << filename << "." << endl;
	  
	} else {
	  fout.clear();
	  for (unsigned int i = 0; i < ptsObjet2.size(); i++) {
	    fout << (ptsObjet2[i].x - frame_width/2) << " " << (ptsObjet2[i].y - frame_height/2) << endl;
	  }
	}

	return 1;

	
}
