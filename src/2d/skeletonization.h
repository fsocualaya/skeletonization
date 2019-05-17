#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <utility>
#include <iomanip>
#include <limits>
#include <fstream>
//OpenCV

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#define POWER 1.69 

typedef cv::Mat Img;
typedef std::vector<std::vector<int>> intMatrix;
typedef std::vector<int> intVector;
typedef std::vector<std::pair<int, int>> Coordinates;
typedef std::vector<std::vector<double>> doubleMatrix;
typedef std::vector<std::pair<double, double>> forceVectors;

double max = std::numeric_limits<double>::max();

Coordinates internalPixels, boundaryPixels;

Img loadImage(std::string filename){
	Img image = cv::imread(filename, cv::IMREAD_GRAYSCALE);
	if(!image.data)
		std::cout<< "No se logró abrir la imagen\n";
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

doubleMatrix potentialXMatrix(intMatrix &matrix){
	doubleMatrix auxMatrix(matrix.size(), std::vector<double>(matrix[0].size(),max));
	for(auto&i:internalPixels){
		auxMatrix[i.first][i.second] = 0;
		for(auto&j:boundaryPixels){
			double distance = sqrt(pow(j.first-i.first,2) + pow(j.second-i.second,2));
			auxMatrix[i.first][i.second]+=(j.first-i.first)/pow(distance,POWER+1); 
		}
	}
	return auxMatrix;
}

doubleMatrix potentialYMatrix(intMatrix &matrix){
	doubleMatrix auxMatrix(matrix.size(), std::vector<double>(matrix[0].size(),max));
	for(auto&i:internalPixels){
		auxMatrix[i.first][i.second] = 0;
		for(auto&j:boundaryPixels){
			double distance = sqrt(pow(j.first-i.first,2) + pow(j.second-i.second,2));
			auxMatrix[i.first][i.second]+=(j.second-i.second)/pow(distance,POWER+1); 
		}
	}
	return auxMatrix;
}

doubleMatrix resultantForceMatrix(doubleMatrix &fx, doubleMatrix &fy){
	doubleMatrix resultantForces(fx.size(), std::vector<double>(fx[0].size(),max));
	for(int i=0;i<fx.size();++i){
		for(int j=0;j<fx[i].size();++j){
			if(fx[i][j]!=max and fy[i][j]!= max)
				resultantForces[i][j] = round(sqrt(pow(fx[i][j],2)+pow(fy[i][j],2)));
			//else resultantForces[i][j] = 0; 
		}
	}
	return resultantForces;
}

void plotDoubleMatDots(doubleMatrix &matrix){
	for(int i=0;i<matrix.size();++i){
		for(int j=0;j<matrix[i].size();++j){
				if(matrix[i][j] <= 10 and matrix[i][j]!=0) std::cout<<'.';//std::fixed<<std::setprecision(2)<<matrix[i][j]<<' ';
				else std::cout<<' ';
		}
		std::cout<<'\n';
	}
}

void plotIntMatDots(intMatrix &matrix){
	for(int i=0;i<matrix.size();++i){
		for(int j=0;j<matrix[i].size();++j){
				if(!matrix[i][j]) std::cout<<'.';
				else if(matrix[i][j]==255) std::cout<<' ';
				else std::cout<<'*';
		}
		std::cout<<'\n';
	}
}

Coordinates criticalPixels(doubleMatrix forces){
	Coordinates criticalPxls;
	for(int i=0;i<forces.size();++i){
		for(int j=0;j<forces[i].size();++j){
			if(!forces[i][j]){
				std::pair<int,int> criticalPixel(i,j);
				if(!forces[i+1][j] || !forces[i-1][j] || 
			   	   !forces[i][j+1] || !forces[i][j-1] ||
		 	           !forces[i+1][j+1] || !forces[i-1][j+1] ||
			   	   !forces[i-1][j-1] || !forces[i+1][j-1])
					forces[i][j] = 0.01;
				else
					criticalPxls.push_back(criticalPixel);
			}
		}
	}
	return criticalPxls;
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

double distance(cv::Point a, cv::Point b){
	return sqrt(pow(a.x-b.x,2)+ pow(a.y-b.y,2));
}

bool colorNeighbors(std::pair<int,int> point,intMatrix&matrix, doubleMatrix forces){
	if(forces[point.first][point.second]<5){	
		double auxForce = 0;
		
		for(int i=point.first-1;i<point.first+1;++i){
			fstd::pair<int,int> closestNeighbor;
			for(int j=point.second;j<point.second;++j){
				if(forces[i][j]<auxForce){
					auxForce = forces[i][j];
					closestNeighbor.first = i; closestNeighbor.second = j;
				}
			}
		}
		matrix[closestNeighbor.first][closestNeighbor.second] = 255;
		return colorNeighbors(closestNeighbor,matrix, forces);
	}
	return true;
}


Img Skeleton(doubleMatrix forces){
	Coordinates criticalPxls = criticalPixels(forces);
	intMatrix skeletonMatrix(forces.size(), std::vector<int>(forces[0].size(), 0));
	//std::vector<cv::Point> pointVector;
	for(auto&i:criticalPxls){
		skeletonMatrix[i.first][i.second]=255;
	//	cv::Point point(i.first,i.second);
	//	pointVector.push_back(point);
	}	
	
	for(auto i:criticalPxls){
		colorNeighbors(i, skeletonMatrix, forces);
	}	

	Img skeleton = createImage(skeletonMatrix);
	return skeleton;
}

void writeImage(Img image, std::string filename){
	std::string path = "../results/"+filename;
	cv::imwrite(path, image);
}

void writeFile(doubleMatrix& matrix, std::string filename){
	std::string path = "../results/"+filename;
	std::fstream output(path,std::ios::out);
	for(int i=0;i<matrix.size();++i){
		for(int j=0;j<matrix[i].size();++j){
			if(matrix[i][j]==max)
				output<<0<<',';
			else output<<matrix[i][j]<<',';
		}
		output<<'\n';
	}
}
