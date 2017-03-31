#include "tracking.hpp"

int main( int argc, char** argv){
	if (argc<3){
		cout << "Exemple:" << argv[0] << " video_name algorithm" << endl;
		return 0;
	}
	string video = argv[1];
	string algo = argv[2];
	runTracking(algo,video);
	return 1;
}
