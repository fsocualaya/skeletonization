#include "imageFunctions.h"
#include "thinning.h"
#include "potentialFields.h"
#include <ctime>

double thinningExecutionTime(Img &originalImage, Img &thinnedImage){
	
	/*----------------- To get the Binary Matrix ------------------*/
	intMatrix imgMatrix = getBinMat(originalImage);	

	/*--------------- To invert the matrix colors ----------------*/
	imgMatrix = invertColors(imgMatrix);
	
	/*--------------- To get the Running Time  ----------------*/
	
	std::clock_t start = std::clock();
	thinnedImage = thinning(imgMatrix);
	double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
	return duration;
}
