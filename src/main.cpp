#include "2d/skeletonization.h"

int main(){
	//Loads Image
	Img image = loadImage("../resources/kimia99-images/cow2.png");	
	//Get binary Matrix based on imgae
	intMatrix imgMat = getBinMat(image);
	
	//Get boundaries of image 
	imgMat = getBinMatBounds(imgMat);

	//Get potential for every internal point
	//Forces in X
	doubleMatrix xForcesMatrix = potentialXMatrix(imgMat);
	//Forces in Y
	doubleMatrix yForcesMatrix = potentialYMatrix(imgMat);

	doubleMatrix resultantForces = resultantForceMatrix(xForcesMatrix, yForcesMatrix);
	
	//writeFile(resultantForces, "data2.csv");

	plotDoubleMatDots(resultantForces);	

	Img skeleton = Skeleton(resultantForces);

	cv::imshow("out", skeleton);
	cv::imshow("original", image);

	writeImage(skeleton, "cowSkeleton.png");

	cv::waitKey(0);



/*
	//Get gradient for every point
	int ** gradientMat = getGradient(potentialMat);

	//Plot the gradient Matrix
	
	plotMatrix(gradientMat);
	*/
	//Plot the skeleton based on gradientMat
	
	//Create the image based on the skeleton
	
	//plotMatDots(imgMat);

		
//	plotDoubleMatDots(xForcesMatrix);
//	plotDoubleMatDots(yForcesMatrix);


	//cv::imshow("original", image);
	//Img outputImg = createImage(imgMat);
	//writeImage(outputImg,"outputImg.png");
	//cv::imshow("new", outputImg);
	//cv::waitKey(0);
	return 0;
}
