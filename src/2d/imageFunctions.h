#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <utility>
#include <iomanip>
#include <limits>
#include <fstream>

// Open CV

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

typedef cv::Mat Img;
typedef std::vector<std::vector<int>> intMatrix;
typedef std::vector<int> intVector;
typedef std::vector<std::pair<int, int>> Coordinates;
typedef std::vector<std::vector<double>> doubleMatrix;
typedef std::vector<std::pair<double, double>> forceVectors;

double max = std::numeric_limits<double>::max(); 

Coordinates internalPixels, boundaryPixels;

Img loadImage(std::string filename){
	std::string path = "../../resources/kimia99-images/"+ filename;
	Img image = cv::imread(path, cv::IMREAD_GRAYSCALE);
	if(!image.data) {
        	std::cout << "No se logró abrir la imagen\n";
        	exit (EXIT_FAILURE);
	}
	else
		return image;
}

intMatrix getBinMat(Img &image){
	intMatrix binaryMat;
	for(int i=0;i<image.rows;++i){
		intVector binaryArray;
		for(int j=0;j<image.cols;++j)
			binaryArray.push_back(image.at<uchar>(i,j));
		binaryMat.push_back(binaryArray);
	}
	return binaryMat;
}

intMatrix getBinMatBounds(intMatrix &matrix){
	intMatrix auxMatrix= matrix;

	for(int i=0;i<matrix.size();++i){
		for(int j=0;j<matrix[i].size();++j){
			std::pair<int, int> coords(i,j);
			if(!matrix[i][j]){
				if(matrix[i+1][j] || matrix[i-1][j] || 
				   matrix[i][j+1] || matrix[i][j-1] ||
				   matrix[i+1][j+1] || matrix[i-1][j+1] ||
				   matrix[i-1][j-1] || matrix[i+1][j-1]){
					boundaryPixels.push_back(coords);
				}
				else{
					auxMatrix[i][j] = 255;
					internalPixels.push_back(coords);
				}
			}
		}
	}
	return auxMatrix;
}

Img createImage(intMatrix &matrix){
	Img image(cv::Size(matrix[0].size(), matrix.size()),CV_8UC(1),cv::Scalar::all(0));
	for(int i=0;i<matrix.size();++i){
		for(int j=0;j<matrix[i].size();++j){
			image.at<uchar>(i,j) = matrix[i][j];
		}
	}
	return image;
}

void displayImage(Img originalImage, Img skeleton){
	char* window_name = "Skeletonization";

	int scale = 2;
	
	cv::resize(originalImage, originalImage, cv::Size(),scale,scale);
	cv::resize(skeleton, skeleton, cv::Size(),scale,scale);

	int width = originalImage.cols, height = originalImage.rows;

	Img windowImg(cv::Size(width*2,height),CV_8UC1,cv::Scalar::all(0));

	originalImage.copyTo(windowImg(cv::Rect(0,0,width,height)));
	skeleton.copyTo(windowImg(cv::Rect(width,0,width,height)));
	
	cv::putText(windowImg, "Original", cv::Point(5,height-10), cv::FONT_HERSHEY_DUPLEX,
		0.4, cv::Scalar(0),1.5);

	cv::putText(windowImg, "Skeleton", cv::Point(width+5,height-10), cv::FONT_HERSHEY_DUPLEX,
		0.4, cv::Scalar(255),1.5);

	cv::namedWindow(window_name, cv::WINDOW_NORMAL);
	
	cv::imshow(window_name, windowImg);
	cv::waitKey(0);
}

void writeImage(const Img& image, std::string filename){
	std::string path = "../results/thinning/imgs/skeleton_"+filename;
	cv::imwrite(path, image);
}

void writeFile(doubleMatrix& matrix, std::string filename){
	std::string path = "../results/thinning/runtime/"+filename;
	std::fstream output(path,std::ios::out);
	for(auto & i : matrix){
		for(int j=0;j<i.size();++j){
			if(i[j]==max)
				output<<0<<',';
			else output<<i[j]<<',';
		}
		output<<'\n';
	}
}
