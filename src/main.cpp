#include "2d/timer.h"
//#include "2d/thinning.h"

#define POTENTIAL_FIELDS 0
#define PARALLEL 0 

int main(){

	std::string filename;
	std::cin>>filename;

	Img image = loadImage(filename);
	
	intMatrix imgMat = getBinMat(image);
	
	if(POTENTIAL_FIELDS){
		imgMat = getBinMatBounds(imgMat);

		doubleMatrix xForcesMatrix = potentialXMatrix(imgMat);
	
		doubleMatrix yForcesMatrix = potentialYMatrix(imgMat);

		doubleMatrix resultantForces = resultantForceMatrix(xForcesMatrix, yForcesMatrix);
	
		plotDoubleMatDots(resultantForces);
	
		Img skeleton = potentialFields(resultantForces);

		cv::imshow("out", skeleton);
		cv::imshow("original", image);

		writeImage(skeleton, filename);

		cv::waitKey(0);
	}

	if(PARALLEL){

		imgMat = invertColors(imgMat);
		std::thread threads[NUMBER_OF_THREADS];
		for(int i=0;i<NUMBER_OF_THREADS;++i){
			threads[i] = std::thread(parallelThinning, std::ref(imgMat), i);
		}
		for(int i=0;i<NUMBER_OF_THREADS;++i){
			threads[i].join();
		}
		Img skeleton = createImage(imgMat);
		displayImage(image,skeleton);
	}

	else{	
		imgMat = invertColors(imgMat);
		Img skeleton = thinning(imgMat);

		displayImage(image, skeleton);	
	
		writeImage(skeleton, filename);	
	}
	
	return 0;
}
