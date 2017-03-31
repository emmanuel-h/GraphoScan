#include "tracking.hpp"
#include "roiSelector.hpp"

int runTracking(string algo,string video ){
	
	Mat frame;
	vector<Rect2d> objects;

	MultiTracker trackers(algo);
	VideoCapture cap(video);
	cap >> frame;

	cout << "Selectionner la ou les zones de capture \n ESPACE: valider la zone \n ECHAP: Quitter la selection " << endl;
	
	selectROI("tracker",frame,objects);

	if (objects.size()<1){
		cout << "Vous n'avez rien selectionné" << endl;
		return 0;
	}
	trackers.add(frame,objects);
	
	cout << "Debut du tracking" << endl;

	bool end = true;
	while (end){
		cap >> frame;
		if (frame.rows > 0 && frame.cols > 0){
			trackers.update(frame);
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
	return 1;
}
