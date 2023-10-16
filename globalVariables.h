#pragma once
#ifndef __globalVariables_h__
#define __globalVariables_h__
#include <vector>
using namespace std;
extern int numberFleets;
extern int numberGroups;
extern int fleetSizes;
extern int fleets[150];
extern int fleetCapacity;
extern int numberCases;
extern int numberCities;
extern int quarantineDays[100];
extern int numberGroupCityStarting[100][10];
extern int totalCityStarting[100];
extern int numberGroupCity[100][10];
extern int totalCity[100];
extern int totalCitizens;
extern int quarantineLocations;
extern int currentQuarantine[10000];
extern int priorityValues[10];
extern int numberPilotGroups;
extern int pilotAirplaneNumber[500];
extern int pilotAirplanes[500][100];
extern int airplanesPilots[500][100];
extern int airplanesPilotsNumber[500];
extern int pilotStatus[500][500];
struct TripInfo
{
	int day;
	int city;
	int airplane;
	int passengers;
	int value;
};
typedef struct
{
	vector <TripInfo> tripsSequence;
	int knapsackNumber;
	int Start;
	int End;
}knapsack;
extern knapsack knapsackG;
extern vector <knapsack> KPsequence;
#endif