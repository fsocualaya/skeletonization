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

#define POWER 1.7
#define ON 255
#define OFF 0

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

bool colorNeighbors(std::pair<int,int> &point, intMatrix &matrix, doubleMatrix forces){
    if(forces[point.first][point.second]==0){
        double auxForce = 0;
        std::pair<int,int> closestNeighbor;
        for(int i=point.first-1;i<=point.first+1;++i){
            for(int j=point.second;j<=point.second;++j){
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
	for(auto&i:criticalPxls){
		skeletonMatrix[i.first][i.second]=255;
	}	
	
	for(auto i:criticalPxls){
		colorNeighbors(i, skeletonMatrix, forces);
	}	

	Img skeleton = createImage(skeletonMatrix);
	return skeleton;
}

Coordinates neighbs(int i, int j, intMatrix matrix){
	Coordinates neighbors;
	std::pair<int,int> point2(i-1,j);
	neighbors.push_back(point2);
	std::pair<int,int> point3(i-1,j+1);
	neighbors.push_back(point3);
	std::pair<int,int> point4(i,j+1);
	neighbors.push_back(point4);
	std::pair<int,int> point5(i+1,j+1);
	neighbors.push_back(point5);
	std::pair<int,int> point6(i+1,j);
	neighbors.push_back(point6);
	std::pair<int,int> point7(i+1,j-1);
	neighbors.push_back(point7);
	std::pair<int,int> point8(i,j-1);
	neighbors.push_back(point8);
	std::pair<int,int> point9(i-1,j-1);
	neighbors.push_back(point9);
	return neighbors;
}

int sumOfCoordinates(Coordinates coords, intMatrix matrix){
	int n=0;
	for(auto i:coords)
		n++;
	return n;
}

bool secondCondition(Coordinates coords, intMatrix matrix){
	return matrix[coords[0].first][coords[0].second]*matrix[coords[2].first][coords[2].second]*matrix[coords[4].first][coords[4].second];
}

bool thirdCondition(Coordinates coords, intMatrix matrix){
	return matrix[coords[2].first][coords[2].second]*matrix[coords[4].first][coords[4].second]*matrix[coords[6].first][coords[6].second];
}

bool SecCondition(Coordinates coords, intMatrix matrix){
	return (matrix[coords[0].first][coords[0].second]*matrix[coords[2].first][coords[2].second]*matrix[coords[6].first][coords[6].second]) and (matrix[coords[0].first][coords[0].second]*matrix[coords[4].first][coords[4].second]*matrix[coords[6].first][coords[6].second]);
}

int orderedPatterns(Coordinates coords, intMatrix matrix){
	int n=0;
	for(int i=0;i<coords.size()-1;++i){
		//Iterate over next neighbor
		if(matrix[coords[i].first][coords[i].second]==0 and matrix[coords[i+1].first][coords[i+1].second]==1)
			++n;
	}
	return n;
}

Img thinning(intMatrix matrix){
	Coordinates firstIterationCoords={}, secondIterationCoords={};

	for(auto & i : matrix){
	    for(int & j : i){
	        if(j==0) j = ON;
	        else j = OFF;
	    }
	}
    for(auto&i: internalPixels)
        matrix[i.first][i.second] = ON;

	do{
		firstIterationCoords = secondIterationCoords = {};
		for(int i=1;i<matrix.size()-2;++i){
			for(int j=1;j<matrix[i].size()-2;++j){
				Coordinates neighbors = neighbs(i,j,matrix);
				if(matrix[i][j]==ON and orderedPatterns(neighbors, matrix)==1 and
				sumOfCoordinates(neighbors,matrix)>=2 and sumOfCoordinates(neighbors, matrix)<=6
				and secondCondition(neighbors, matrix) and thirdCondition(neighbors,matrix)){
					std::pair<int, int> pixel(i,j);
					firstIterationCoords.push_back(pixel);
				}
			}
		}
		for(auto i:firstIterationCoords)
				matrix[i.first][i.second]=OFF;
		for(int i=1;i<matrix.size()-2;++i){
			for(int j=0;j<matrix[i].size()-2;++j){
				Coordinates neighbors = neighbs(i,j,matrix);
				if(matrix[i][j]==ON and orderedPatterns(neighbors, matrix)==1 and
				sumOfCoordinates(neighbors,matrix)>=2*ON and sumOfCoordinates(neighbors, matrix)<=6*ON
			and SecCondition(neighbors, matrix)){
					std::pair<int, int> pixel(i,j);
					firstIterationCoords.push_back(pixel);
				}
			}

		}
		for(auto i:secondIterationCoords)
			matrix[i.first][i.second] = OFF;
	}while(!firstIterationCoords.empty() or !secondIterationCoords.empty());
	Img thinnedImage = createImage(matrix);
	return thinnedImage;
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
