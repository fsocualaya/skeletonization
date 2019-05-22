#define POWER 1.7

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


Img potentialFields(doubleMatrix forces){
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
