#include <stdio.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <iostream>

#include "dispatchingRules.h"
#include "globalVariables.h"

#include "fitness.h"
#include <vector>
using namespace std;


int fitenessValue2(int Tmax)
{

	int Pvalues[100][10][200];
	int Pcum[100][10][200];
	for (int j = 0; j < numberCities; j++)
	{
		for (int k = 0; k < numberGroups; k++)
		{

			Pvalues[j][k][0] = numberGroupCityStarting[j][k];
			printf("\n  j=%d k=%d number=%d ", j, k, numberGroupCityStarting[j][k]);
			getchar();
			for (int kk = k; kk >= 0; kk--)
				Pcum[j][k][0] = Pvalues[j][k][0];
			printf("\n  j=%d k=%d number=%d Pcum=%d", j, k, Pvalues[j][k][0], Pcum[j][k][0]);
			getchar();
		}
	}

	return 0;
}
int fitenessValue(int Tmax)
{
	int value = 0;
	//	printf("\n in fiteness Value size=%d tmax=%d",KPsequence.size(),Tmax);
	//	getchar();
	int sumPos = 0;
	int sumNegative = 0;
	for (int city = 0; city < numberCities; city++)
		for (int group = 0; group < numberGroups; group++)
			sumPos += numberGroupCityStarting[city][group] * priorityValues[group] * Tmax;
	//	printf("\n sum positive=%d",sumPos);
	//	getchar();
	for (int kp = 0; kp < KPsequence.size(); kp++)
	{
		int start = KPsequence.at(kp).Start;
		int end = KPsequence.at(kp).End;
		int kpNumber = KPsequence.at(kp).knapsackNumber;
		//		printf("\n start=%d end=%d kp number=%d",start,end,kpNumber);
		//		getchar();
		for (int t = 0; t < KPsequence.at(kp).tripsSequence.size(); t++)
		{
			int tempValue = KPsequence.at(kp).tripsSequence.at(t).value;
			int tempDay = KPsequence.at(kp).tripsSequence.at(t).day;
			int airplane = KPsequence.at(kp).tripsSequence.at(t).airplane;
			int city = KPsequence.at(kp).tripsSequence.at(t).city;
			sumNegative += (Tmax - tempDay + 1) * tempValue;
			int numberPassengers = KPsequence.at(kp).tripsSequence.at(t).passengers;
			//		printf("\n trip=%d day=%d passengers=%d sum negative=%d airplane=%d city=%d",tempValue,tempDay, numberPassengers,
			//			sumNegative, airplane,city);
			//		getchar();
		}
	}

	value = sumPos - sumNegative;
	printf("\n value=%d", value);
	getchar();
	return value;
}

