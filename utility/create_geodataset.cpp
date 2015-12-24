#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <map>
using namespace std;

/*
 * <DATA> is obtained from http://geonames.nga.mil/gns/html/namefiles.html
 */

/***********************
* CLASSES              *
************************/
class Point
{
public:
  Point(int _x, int _y)
  {
      x = _x;
      y = _y;
  }
  
  int x;
  int y;
};


/*********************** 
* GLOBAL VARIABLES     *
************************/
vector< pair<float, float> > elementList;
vector< Point > points;
float xCellSize, yCellSize;
float minDegreeX;
float minDegreeY;
float maxDegreeX;
float maxDegreeY;
int xArraySize;
int yArraySize;
int numBits;

/********************
* PROTOTYPES        *
*********************/

//Extract lat and lon parameters from the file. Save pairs in elementList array.
void parseTXTFile(string filename);

//Mapping lat lon to [0, N] matrix representation.
void determineArrayLimits();

//One point can represent multiple (lat, lont) coords because of granularity. So 
//each duplicate is eliminated.
void preprocessPointListUniqueValue();

void writeBinaryFile(const char* filename);

//Get the number of bits of a number.
int bits(int number);

bool orderPoints(Point p1, Point p2);

/****************
* MAIN          *
*****************/
int main(int argc, char* argv[])
{
	if(argc < 4)
	{
		printf("USAGE: <DATA> <FILENAME> <GRANULARITY>\n");
		printf("<GRANULARITY> is a number representing the size of a cell. For example, 100 contains from lat = 0 to lat 99 in one cell, 100 to 199 en another.\n");
		return -1;
	}	
	
	parseTXTFile(argv[1]);

	//Granularity
	xCellSize = yCellSize = atof(argv[3]);
	
	determineArrayLimits();

	points.clear();  
  for(int i = 0; i < (int)elementList.size(); i++)
  {
      int xPos = (int)ceil((elementList[i].first - minDegreeX) / xCellSize);
      int yPos = (int)ceil((elementList[i].second - minDegreeY) / yCellSize);;
			points.push_back(Point(xPos, yPos));
	}

	preprocessPointListUniqueValue();
	writeBinaryFile(argv[2]);

	return 0;
}


/**************************************
* FUNCTIONS                           *
***************************************/

void parseTXTFile(string filename)
{
    string line;
    ifstream myFile(filename.c_str());

    bool bIsTagLine = true;
    
    if(myFile.is_open()) 
    {
        while(getline(myFile, line)) 
				{
            if(bIsTagLine)
            {
                bIsTagLine = false;
                continue;
            }

            istringstream iss(line);
            string token;
            string lat = "";
            string lon = "";
            int parameterNum = 1;

            while(getline(iss, token, '\t'))
            {
                if(parameterNum == 4)
                    lat = token;
                else if(parameterNum == 5)
                    lon = token;

                parameterNum++;
            };

            float latValue = (float)atof(lat.c_str());
            float lonValue = (float)atof(lon.c_str());

						elementList.push_back( make_pair( latValue, lonValue ) );

        } 

    } 

    myFile.close();
} //parseTXTFile function

void determineArrayLimits()
{
    if(elementList.size() > 0)
    {
        minDegreeX = elementList[0].first;
        minDegreeY = elementList[0].second;
        maxDegreeX = elementList[0].first;
        maxDegreeY = elementList[0].second;

        for(int i = 1; i < (int)elementList.size(); i++)
        {
            float currX = elementList[i].first;
            float currY = elementList[i].second;

            minDegreeX = min(minDegreeX, currX);
            minDegreeY = min(minDegreeY, currY);
            maxDegreeX = max(maxDegreeX, currX);
            maxDegreeY = max(maxDegreeY, currY);
        }

        xArraySize = (int)ceil((fabs(maxDegreeX - minDegreeX)) / xCellSize);
        yArraySize = (int)ceil((fabs(maxDegreeY - minDegreeY)) / yCellSize);

				int maxSize = max(xArraySize, yArraySize);
				numBits = bits(maxSize) * 2;
  }

} //determineArrayLimits function

void preprocessPointListUniqueValue()
{
  vector<Point> newPointList;
  
  map< pair<int, int> , bool> M;
  for(int i = 0; i < (int)points.size(); i++)
  {
    if(!M[make_pair(points[i].x, points[i].y)])
    {
      newPointList.push_back(points[i]);
      M[make_pair(points[i].x, points[i].y)] = true;
    }
  }
  
  points.clear();
  for(int i = 0; i < (int)newPointList.size(); i++)
  {
    points.push_back(newPointList[i]);
  }

} //PreprocessPointListUniqueValue function

void writeBinaryFile(const char* filename)
{ 
  sort(points.begin(), points.end(), orderPoints);
  
  ofstream myFile(filename, ios::out | ios::binary);
  
  int N = 1 << (numBits / 2);
  long numPoints = points.size();
  
  myFile.write((char*)&N, sizeof(int));
  myFile.write((char*)&numPoints, sizeof(long int));
  
  map<int, vector<int> > M;
  for(int i = 0; i < numPoints; i++)
  {
    M[points[i].y].push_back(points[i].x);
  }

  for(int i = 1; i <= N; i++)
  {
    int newList = i * (-1);
    
    myFile.write((char*)&newList, sizeof(int));
    
    for(int j = 0; j < M[i - 1].size(); j++)
    {
      int x = M[i - 1][j] + 1;
      myFile.write((char*)&x, sizeof(int));
    }
  }
  
  myFile.close();

} //writeBinaryFile function

int bits(int number)
{
	int cnt = 0;
	while(number) 
	{
		number /= 2;
		cnt++;	
	}

	return cnt;
} //bits function

bool orderPoints(Point p1, Point p2)
{
  if(p1.y == p2.y)
  {
    return p1.x < p2.x;
  }
  
  return p1.y < p2.y;
} //orderPoints function


