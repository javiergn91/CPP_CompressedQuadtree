#include <stdio.h>

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("USAGE: <DATASET>\n");
		return -1;
	}

	FILE* dataset = fopen(argv[1], "rb");
	
	int N; //size of the matrix, NxN
 	fread(&N, sizeof(int), 1, dataset);

	long int M; //number of points
	fread(&M, sizeof(long), 1, dataset);

	int integer;
	long int countPoints = 0;	
	int countN = 0;	
	int cnt = 10;	
	while(fread(&integer, sizeof(int), 1, dataset) == 1)
	{
		if(integer > 0)
		{
			countPoints++;
		}
	
		if(integer < 0)
		{
			countN = integer;
		}
	}

	if(countPoints == M)
	{
		printf("Number of points OK.\n");
	}
	else
	{
		printf("Number of points mismatch: %ld points, but there are %ld points.\n", countPoints, M);
	}	

	countN = -countN;
	if(countN == N)
	{
		printf("Number of edges OK.\n");
	}
	else
	{
		printf("N is %d but dataset returns %d\n", N, countN);
	}

	fclose(dataset);
}
