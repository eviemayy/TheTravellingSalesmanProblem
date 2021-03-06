#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <deque>
#include "math.h"
#include <vector>
#include <cstdlib>
#include <string.h>

using namespace std;

/******************************************************
 * ** Program: Nearest Neighbor TSP solution
 * ** Author: Mary May & Daniel Jones & Quinn Campfield
 * ** Date: 03/12/2020
 * ** Description: An application of the nearest neighbor algorithm to the Travelling Salesperson problem.
 * ** Input: a .txt file entered from the command line. File has an id, x-cord, y-cord on every line.
 * ** Output: a file with the same name appended with .tour. Additionally the distance printed to terminal.
 * ******************************************************/

class City { 

	public: 
  
    int x;
    int y;
    int ident;
    int pos;


City(int nid, int nx, int ny, int npos) : ident(nid), x(nx), y(ny), pos(npos) {}

};
//Function Declarations
int outerNN(int numCities,  deque <City*> inputList, deque <City*> currentTourTest, int** matrix, int* tour);
int innerNN(int indexFrom, int numCities, deque <City*> inputList, deque <City*> currentTourTest, int** matrix, int* tour);

//Function called to copy a City to a City
void cityCopy(deque <City*> & fromC, deque <City*> & toC){
    int length = fromC.size();
    toC.clear();
    for (int i = 0; i < length; ++i){
        toC.push_back(new City(*fromC[i]));
    }
}

void printtofile(char* name, int* tour, int numCities, int finalDist){
    ofstream f;
	strcat(name, ".tour");
	cout << "Output File --   "<< name;
    f.open(name);
    if(!f){ //if file cant be opened
        cout << "Cannot open file" << endl;
    }
    else{
		for(int i=0; i<numCities+1; i++){ //First index is length followed by tour
			f<<tour[i]<<endl; 
		}
	}
    f.close();
    cout<<"   -- Has Been Created."<<endl;
}


int getDistance(int x[], int y[], int numCities, int i, int j, int *matrix[]) {
	if (i == j) {
		matrix[i][j] = 0;
		return matrix[i][j];
	}
	//If not, we use the rounding function on the sqrt function, to find the difference
	matrix[i][j] = round(sqrt(pow(x[i] - x[j], 2) + pow(y[i] - y[j], 2)));
	return matrix[i][j];
}

void getData(ifstream &inFile, string line, int numCities, int *identifier, int *x, int *y) {
	for (int i = 0; i < numCities; i++) {
		inFile >> identifier[i];
		inFile >> x[i];
		inFile >> y[i];
	}
}

int main(int argc, char* argv[]) {
//=======Get Input=======//
    ifstream inFile;
	int numCities = 0;
	int p = 0;
	string line;
	if (argc != 2) {
		cout << "Incorrect number of arguments!" << endl;
		return 1;
	}
	inFile.open(argv[1]);
	while (getline(inFile, line)) {
		++numCities;
	}
	int x[numCities];
	int y[numCities];
	int identifier[numCities];
	inFile.clear();
	inFile.seekg(0);
	getData(inFile, line, numCities, identifier, x, y);
	int **matrix = new int*[numCities];
	for (int i = 0; i < numCities; i++) {
		matrix[i] = new int[numCities];
	}
	for (int i = 0; i < numCities; i++) {
		for (int j = 0; j < numCities; j++) {
			getDistance(x, y, numCities, i, j, matrix);
		}
	}
//=======Get Input=======//

	 
//=======Setup=======//
    int finalDist = 0;
    deque <City*> inputList;
	deque <City*> currentTourTest;
    int tour[numCities+1];
    
    for(int i = 0; i< numCities; i++){
        //Create list of cities with infromation from files that we put in arrays
        inputList.push_back(new City(identifier[i], x[i], y[i], identifier[i])); 
        
    }
//=======Setup=======//
    
//=======Call Funcs=======//
    finalDist = outerNN(numCities, inputList, currentTourTest, matrix, tour);//Calling NN
    cout<<"Final: "<<finalDist<<endl;//Print final distance once NN is finished
//=======Call Funcs=======//

//=======Print to File=======//
    printtofile(argv[1], tour, numCities, finalDist);//Use tour array to print the tour and distance
//=======Print to File=======//
  
    return 0; 
}

int outerNN(int numCities,  deque <City*> inputList, deque <City*> currentTourTest, int** matrix, int* tour){
    int bestDistance = 1000000000; //Make it high to make it the worst to start.
    int currentRun;
    int tourTemp[numCities+1];//Array to hold tour values before knowing if it is better or not

    for (int i = 0; i < numCities; ++i){
        currentRun = innerNN(i, numCities, inputList, currentTourTest, matrix, tourTemp);
        if (currentRun < bestDistance){//if lower we found a better tour so we replace info
            bestDistance = currentRun;
            cout << "Writing currentTourTest " << bestDistance << endl;
            //update tour
            for(int i = 0; i<numCities+1; i++){
                tour[i]=tourTemp[i];//copy the tour to a original array when it is a better tour
            }
        }
    }
    
    return bestDistance; 
}

int innerNN(int indexFrom, int numCities,  deque <City*> inputList, deque <City*> currentTourTest, int** matrix, int* tourTemp){
    int tourIndex= 1; //STart at 1 to save Index 0 for the length of tour
    int citiesPlaced = 0;
    int distCloseCity = 1000000000;
    int distanceF = 0;
    int citiesDist = 0;
    int closeCityI = 0;
    int citiesLeft = numCities;
    deque <City*> editList;
    cityCopy(inputList, editList);    //Copy the inputList of cities into a temp editList   

    //currentTourTest.clear();
    currentTourTest.push_back(editList[indexFrom]);
    tourTemp[tourIndex] = editList[indexFrom]->ident;
    tourIndex++;     
    editList.erase(editList.begin() + indexFrom);       
    --citiesLeft;        
    ++citiesPlaced;       
    while(citiesLeft != 0){
        distCloseCity = 10000000; //Maxed so it can be reset every loop 
        for (int i = 0; i < citiesLeft; ++i){
            citiesDist = matrix[editList[i]->ident][currentTourTest[citiesPlaced-1]->ident];
            if (citiesDist < distCloseCity){
                closeCityI = i;
                distCloseCity = citiesDist;
            }
        }

        
        currentTourTest.push_back(editList[closeCityI]);
        tourTemp[tourIndex] = editList[closeCityI]->ident;
        tourIndex++;
        editList.erase(editList.begin() + closeCityI);

        distanceF += distCloseCity;

        --citiesLeft;
        ++citiesPlaced;
    }
      
    int t = matrix[currentTourTest[0]->ident][currentTourTest[citiesPlaced-1]->ident];
    tourTemp[0] = distanceF + t;
    return distanceF + t;
}

