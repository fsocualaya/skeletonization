#define ON 255
#define OFF 0

intMatrix invertColors(intMatrix&imgMatrix){
	intMatrix invertedColorsMatrix = imgMatrix;
	for(auto&i:invertedColorsMatrix){
		for(auto&j:i){
			if(j) j = OFF;
			else j = ON;
		}
	}
	return invertedColorsMatrix;
}

intVector getNeighbors(int&i, int&j, intMatrix&matrix){
	intVector neighbors;
	neighbors.push_back(matrix[i-1][j]); 
	neighbors.push_back(matrix[i-1][j+1]); 
	neighbors.push_back(matrix[i][j+1]); 
	neighbors.push_back(matrix[i+1][j+1]); 
	neighbors.push_back(matrix[i+1][j]); 
	neighbors.push_back(matrix[i+1][j-1]); 
	neighbors.push_back(matrix[i][j-1]); 
	neighbors.push_back(matrix[i-1][j-1]); 
	neighbors.push_back(matrix[i-1][j]);
	return neighbors;
}

int numberOfOnNeighbors(intVector &neighbors){
	int sum = 0;
	for(int i = 0;i < neighbors.size() - 1;++i){
		if(neighbors[i]==ON)
			++sum;
	}
	return sum;
}

int numberOf01Transitions(intVector &neighbors){
	int n = 0;
	for(int i=1;i<neighbors.size();++i){
		if(neighbors[i-1]==OFF and neighbors[i]==ON)
			++n;		
	}
	return n;
}

bool positionsOff(int a, int b, int c, intVector &neighbors){
	return (neighbors[a-2]==OFF or neighbors[b-2]==OFF or neighbors[c-2]==OFF);
}

bool firstIterationConditions(int &i, int &j, intVector &neighbors, intMatrix &matrix){
	bool isOn = matrix[i][j]==ON;		// if 0 -> False
    if (isOn) {
        int numberOfNeighbors = numberOfOnNeighbors(neighbors);
        bool numberOfNeighborsIsCorrect = (numberOfNeighbors >= 2) and
                                          (numberOfNeighbors <= 6);
        if (numberOfNeighborsIsCorrect){
            int numberOfTransitions = numberOf01Transitions(neighbors);
            bool numberOfTransitionsIsCorrect = (numberOfTransitions == 1);

            if (numberOfTransitionsIsCorrect) {
                bool positionsOffAreCorrect = positionsOff(2, 4, 6, neighbors);
                if (positionsOffAreCorrect) {
                    bool positionsOffAreCorrect_again = positionsOff(4, 6, 8, neighbors);
                    if(positionsOffAreCorrect_again) return true;
                }
            }
        }
    }
    return false;
}

bool secondIterationConditions(int &i, int &j, intVector &neighbors, intMatrix &matrix){
    bool isOn = matrix[i][j]==ON;		// if 0 -> False
    if (isOn) {
        int numberOfNeighbors = numberOfOnNeighbors(neighbors);
        bool numberOfNeighborsIsCorrect = (numberOfNeighbors >= 2) and
                                          (numberOfNeighbors <= 6);
        if (numberOfNeighborsIsCorrect){
            int numberOfTransitions = numberOf01Transitions(neighbors);
            bool numberOfTransitionsIsCorrect = (numberOfTransitions == 1);

            if (numberOfTransitionsIsCorrect) {
                bool positionsOffAreCorrect = positionsOff(2, 4, 8, neighbors);
                if (positionsOffAreCorrect) {
                    bool positionsOffAreCorrect_again = positionsOff(2, 6, 8, neighbors);
                    if(positionsOffAreCorrect_again) return true;
                }
            }
        }
    }
    return false;
}

Img thinning(intMatrix matrix){
	Coordinates firstIterationCoords={}, secondIterationCoords={};
	do{
		firstIterationCoords = secondIterationCoords = {};
		for(int i=1;i<matrix.size()-1;++i){
			for(int j=1;j<matrix[i].size()-1;++j){
				intVector neighbors = getNeighbors(i,j,matrix);
				if(firstIterationConditions(i, j, neighbors, matrix)){
					std::pair<int, int> point(i,j);
					firstIterationCoords.push_back(point);
				}
			}
		}

		for(auto i:firstIterationCoords)
			matrix[i.first][i.second] = OFF;

		for(int i=1;i<matrix.size()-1;++i){
			for(int j=0;j<matrix[i].size()-1;++j){
				intVector neighbors = getNeighbors(i,j,matrix);
				if(secondIterationConditions(i,j, neighbors, matrix)){
					std::pair<int, int> point(i,j);
					secondIterationCoords.push_back(point);
				}
			}
		}

		for(auto i:secondIterationCoords)
			matrix[i.first][i.second] = OFF;

	}while(!firstIterationCoords.empty() or !secondIterationCoords.empty());
	
	Img thinnedImage = createImage(matrix);
	return thinnedImage;
}
