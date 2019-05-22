#include "2d/timer.h"

#define POTENTIAL_FIELDS 0

int main(){
	//Loads Image
	Img image = loadImage("../resources/kimia99-images/donkey1.png");
	int originalArea = image.rows*image.cols;

	//Get binary Matrix based on image
	intMatrix imgMat = getBinMat(image);
	if(POTENTIAL_FIELDS){
	
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

	Img skeleton = potentialFields(resultantForces);

	cv::imshow("out", skeleton);
	cv::imshow("original", image);

	writeImage(skeleton, "cowSkeleton.png");

	cv::waitKey(0);

	//	plotDoubleMatDots(xForcesMatrix);
	//	plotDoubleMatDots(yForcesMatrix);


	//cv::imshow("original", image);
	//Img outputImg = createImage(imgMat);
	//writeImage(outputImg,"outputImg.png");
	//cv::imshow("new", outputImg);
	//cv::waitKey(0);
	}
	else{	
		Img thinned, auxImage;
		std::fstream executionTimes("../results/thinning/runtime/thinningExecutionTime.csv", std::ios::out);
		executionTimes<<"imageArea,executionTime\n";
		for(double Scale=0.5;Scale<=2.0;Scale+=0.25){
			std::cout<<Scale<<'\n';
			cv::resize(image,auxImage,cv::Size(),Scale,Scale);
			for(int j=0;j<3;++j){
				double duration = thinningExecutionTime(auxImage,thinned);
				executionTimes<<originalArea*pow(Scale,2)<<','<<duration<<'\n';
			}
		}
		executionTimes.close();
		writeImage(thinned, "../results/thinning/imgs/skeleton_donkey1.png");
		cv::imshow("original", auxImage);
		cv::imshow("Thinned", thinned);
		cv::waitKey(0);
	}
	return 0;
}
