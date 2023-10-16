#include <stdio.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include "globalVariables.h"
#include "dispatchingRules.h"
#include "fitness.h"


FILE* input2;
FILE* output;
int numberFleets;
int fleetSizes;
int fleets[150];
int fleetCapacity;
void readAll(void);
FILE* output2;
int numberGroups;
int numberCases;
int numberCities;
int quarantineDays[100];
int numberGroupCityStarting[100][10];
int totalCityStarting[100];
int totalCitizens;
int quarantineLocations;
int numberGroupCity[100][10];
int totalCity[100];
int currentQuarantine[10000];
int priorityValues[10];
int numberPilotGroups;
int pilotAirplaneNumber[500];
int pilotAirplanes[500][100];
int airplanesPilots[500][100];
int airplanesPilotsNumber[500];
int pilotStatus[500][500];
using namespace std;

void dispatchingRules(int rule);
vector <knapsack> KPsequence;

int initialX[100][50][200];
int updatecities(int city, int airplaneSize, int day, int quarantineCapacity, int* passengersOut, int* valueOut);

int findPilot(int airplane, int day, int choiceCase);
int choosePilot(int possiblePilots[], int numberAirplanes[], int numberPilots, int caseChoice);
void main(void)
{
    output2 = fopen("44000-var-40-3.txt", "w");
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 50; j++)
            for (int k = 0; k < 200; k++)
                initialX[i][j][k] = 0;
    printf("\n start");
    output = fopen("comparison.txt", "w");
    getchar();
    int minSize = 10000000;
    int maxSize = 0;
    int sumSize = 0;
    readAll();
    quarantineLocations;
    double value = 0;
    vector <knapsack> dispatchingSolution;
    dispatchingRules(4);
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 50; j++)
            for (int k = 0; k < 200; k++)
                fprintf(output2, "%d\n", initialX[i][j][k]);
}
void dispatchingRules(int rule)
{
    int sum = 0;
    int day = 1;

    int totalNumber = 0;

    for (int i = 0; i < numberCities; i++)
    {
        totalCity[i] = totalCityStarting[i];
    }

    for (int i = 0; i < numberCities; i++)
        for (int j = 0; j < numberGroups; j++)
            numberGroupCity[i][j] = numberGroupCityStarting[i][j];

    for (int i = 0; i < numberCities; i++)
        totalNumber += totalCity[i];
    for (int i = 0; i < 10000; i++)
        currentQuarantine[i] = 0;
    int counter;

    knapsack tempKnapsack;
    tempKnapsack.Start = 1;
    tempKnapsack.knapsackNumber = 0;
    int knapsackNumber = 0;
    vector <TripInfo> tripSequence;
    int knapsackStatus = 0;
    while (totalNumber > 0)
    {

        for (int i = 0; i < fleetSizes && totalNumber>0; i++)
        {
            // printf("\n i=%d fleet size=%d totalNumber=%d",i, fleetSizes, totalNumber);
             // getchar();
            int pilot = findPilot(i, day, 1);
          //
          //  pilot = 1;
         //      printf("\n returned airplane=%d pilot=%d day=%d",i,pilot,day);
          //    getchar();
            int airplaneSize = fleets[i];
            int city;
            int passengers = 0;
            if (rule == 1)
                city = selectTotal(currentQuarantine[day], quarantineLocations);
            if (rule == 2)
                city = selectPriority(currentQuarantine[day], quarantineLocations);
            if (rule == 3)
                city = selectSumWeight(currentQuarantine[day], quarantineLocations);
            //    printf("\n dispatching rule=%d", dispatchineRule);
            if (rule == 4)
                city = selectAirplaneCost(currentQuarantine[day], quarantineLocations, airplaneSize);

            if (city != 1000 && pilot != 1000)
            {
                TripInfo tempTrip;
                tempTrip.airplane = airplaneSize;
                tempTrip.city = city;
                tempTrip.day = day;
                int passengersOut = 0;
                int valueOut = 0;
                totalNumber = updatecities(city, airplaneSize, day, quarantineLocations, &passengersOut, &valueOut);
                tempTrip.passengers = passengersOut;
                tempTrip.value = valueOut;
                tripSequence.push_back(tempTrip);
                for (int t = day; t < day + 7; t++)
                    pilotStatus[pilot][t] = 0;
          //      printf("\n trip city=%d airplane=%d pilot=%d day=%d", city, i, pilot, day);
         //       getchar();
                //        //   totalNumber = updatecities(city, airplaneSize, &currentQuarantine, quarantineLocations);
  //printf("\n day=%d fleet=%d airplane size=%d city=%d pilot=%d quarantine=%d total number=%d passengers=%d", day, i, airplaneSize, city, pilot,currentQuarantine[day],
             //               totalNumber, passengersOut);
     

  
 // getchar();
                initialX[city][i][day - 1] = 1;
            }
            else
            {
                if (city == 1000)
                    i = fleetSizes;
                /*     if (currentQuarantine[day] == quarantineLocations && knapsackStatus == 0)
                     {
                         knapsackStatus = 1;

                         for (int tt = 0; tt < tripSequence.size(); tt++)
                         {
                            // printf("\n tt=%d city=%d", tt, tripSequence[tt].city);
                             //        getchar();
                         }
                         tempKnapsack.tripsSequence = tripSequence;
                     }*/
            }
            //    printf("\n i=%d total=%d",i,totalNumber);
             //   getchar();
        }
        day++;
        if (/*currentQuarantine[day] < quarantineLocations &&*/ knapsackStatus == 1 || totalNumber == 0)
        {
            //       printf("\n in the update");
               //    getchar();
            knapsackStatus = 0;
            tempKnapsack.End = day - 1;
            tempKnapsack.knapsackNumber = knapsackNumber;
            tempKnapsack.Start = day;
            tempKnapsack.tripsSequence = tripSequence;
            KPsequence.push_back(tempKnapsack);
            tripSequence.clear();
            knapsackNumber++;
        }
    };

    printf("\n duration=%d size=%d", day, KPsequence.size());
    getchar();
    /*  for (int i = 0; i < sequence.size(); i++)
      {
          for (int jj = 0; jj < sequence.at(i).tripsSequence.size(); jj++)
          {
              printf("\n knapsack=%d trip=%d city=%d", i, jj, sequence.at(i).tripsSequence[jj].city);
              getchar();

          }
      }*/
    int value = fitenessValue(day - 1);
    // int value = fitenessValue2(day - 1);

}
int findPilot(int airplane, int day, int choiceCase)
{
    int backPilot = 1000;
    int checkOut = 0;
    int possiblePilots[50];
    int numberPilots = 0;
    int numberAirplanes[50];
 //   printf("\n airplane=%d", airplane);
  //  getchar();
    for (int p = 0; p < airplanesPilotsNumber[airplane]; p++)
    {
        int pilot = airplanesPilots[airplane][p];
      //     printf("\n in selection module airplane=%d pilot=%d", airplane, pilot);
       //    getchar();
        if (pilotStatus[pilot][day] == 1)
        {
            possiblePilots[numberPilots] = pilot;
            numberAirplanes[numberPilots] = pilotAirplaneNumber[pilot];
            numberPilots++;

        }
   //      printf("\n in selection module airplane=%d pilot=%d",airplane,pilot);
    //      getchar();
    }
    if (numberPilots > 0)
    {
        int returnedPilot = choosePilot(possiblePilots, numberAirplanes, numberPilots, 1);
        return returnedPilot;

    }

    return 1000;
}
int choosePilot(int possiblePilots[], int numberAirplanes[], int numberPilots, int caseChoice)
{
    srand(2);
    int	temp = rand();
    double random = (double)temp / RAND_MAX;
    int MinNumber = 10000;
    int bestPilot = 0;
    for (int p = 0; p < numberPilots; p++)
        if (numberAirplanes[p] < MinNumber)
        {
            MinNumber = numberAirplanes[p];
            bestPilot = p;
        }
    if (caseChoice == 1)
    {
        double prob = 1.0 / (float)numberPilots;
        double cumProb[100];
        cumProb[0] = 0;
        for (int i = 0; i < numberPilots; i++)
            cumProb[i + 1] = cumProb[i] + prob;
        int	temp = rand();
        double random = (double)temp / RAND_MAX;
        int loc = 0;
        do {
            //       printf("\n loc=%d choice=%d cumProb=%f",loc,possibleCities[loc],cumProbabilities[loc]);
            //       getchar();
            loc++;
        } while (random > cumProb[loc]);
        loc--;
       //      printf("\n random=%f loc=%d pilot=%d",random,loc, possiblePilots[loc]);
         //    getchar();
        return possiblePilots[loc];
    }
    
    //   printf("\n best pilot=%d", bestPilot);
     //  getchar();
    return bestPilot;
}
int updatecities(int city, int airplaneSize, int day, int quarantineCapacity, int* passengersOut, int* valueOut)
{
    int currentCapacity = airplaneSize;
    //   printf("\n  before size=%d city=%d total=%d quarantine=%d", airplaneSize,city, totalCity[city], quarantineDays[city]);
    //   getchar();
    if (quarantineCapacity - (currentQuarantine[day]) < currentCapacity)
        currentCapacity = quarantineCapacity - (currentQuarantine[day]);
    //  printf("\n quarantineCapacity=%d currentCapacity=%d", quarantineCapacity,currentCapacity);
     // getchar();
    int totalLeaving = 0;
    for (int i = 0; i < numberGroups && currentCapacity>0; i++)
    {
        //    printf("\n group=%d number=%d",i, numberGroupCity[city][i]);
        //    getchar();
        if (numberGroupCity[city][i] > currentCapacity)
        {
            int leaving = 0;
            leaving = currentCapacity;
            // quarantine14[0] += leaving;
            for (int j = day; j < day + quarantineDays[city]; j++)
                currentQuarantine[j] += leaving;
            totalLeaving += currentCapacity;
            numberGroupCity[city][i] -= currentCapacity;
            currentCapacity = 0;
            (*valueOut) += leaving * priorityValues[i];
        }
        else
        {
            int leaving = 0;
            leaving = numberGroupCity[city][i];
            // quarantine14[0] += leaving;
            for (int j = day; j < day + quarantineDays[city]; j++)
                currentQuarantine[j] += leaving;
            // (*currentQuarantine) += leaving;
            currentCapacity -= numberGroupCity[city][i];
            totalLeaving += numberGroupCity[city][i];
            (*valueOut) += leaving * priorityValues[i];
            numberGroupCity[city][i] = 0;

        }
        //  printf("\n group=%d number=%d", i, numberGroupCity[city][i]);
        //  getchar();
    }
    //   printf("\n total leaving=%d",totalLeaving);
    //   getchar();
  //  printf("\n before city=%d total before=%d",city, totalCity[city]);
    totalCity[city] -= totalLeaving;
    (*passengersOut) = totalLeaving;
    //   printf("\n after city=%d total before=%d", city, totalCity[city]);
    //   getchar();
    int totalNumbertemp = 0;
    for (int i = 0; i < numberCities; i++)
        totalNumbertemp += totalCity[i];
    //printf("\n city=%d leaving=%d total=%d  quarantine=%d capacity=%d", city, totalLeaving, totalCity[city], (*currentQuarantine), quarantineCapacity);
    //getchar();

    return totalNumbertemp;
}
void readAll(void)
{

    //   input2 = fopen("OneMillionCaseVar.txt", "r");
//    input2 = fopen("returnCasesJordan0-1.txt", "r");
  //  input2 = fopen("returnCasesJordan14-90-77.txt", "r");
 //   input2 = fopen("OneMillionCaseVar.txt", "r");
 //   input2 = fopen("returnCasesJordan14.txt", "r");
    input2 = fopen("returnCasesJordanVar-90-40-3.txt", "r");
    fscanf(input2, "%d", &numberCases);

    int dum;

    totalCitizens = 0;
    fscanf(input2, "%d", &numberCities);
    fscanf(input2, "%d", &numberGroups);
    //   printf("\n numberCases=%d cities=%d groups=%d", numberCases, numberCities, numberGroups);
    //   getchar();
    for (int j = 0; j < numberGroups; j++)
        fscanf(input2, "%d", &priorityValues[j]);
    for (int j = 0; j < numberCities; j++)
    {
        int sum = 0;
        fscanf(input2, "%d", &dum);
        for (int k = 0; k < numberGroups; k++)
        {
            fscanf(input2, "%d", &numberGroupCityStarting[j][k]);
            sum += numberGroupCityStarting[j][k];
                //         printf("\n  j=%d k=%d number=%d sum=%d",j,k, numberGroupCityStarting[j][k],sum);
                 //          getchar();
        }
        totalCityStarting[j] = sum;
        totalCitizens += totalCityStarting[j];
        fscanf(input2, "%d", &quarantineDays[j]);
        //           printf("\n quarantine=%d", quarantineDays[j]);
         //          getchar();
    }
    // printf("\n total citizes=%d", totalCitizens);
    // getchar();
    fscanf(input2, "%d", &fleetSizes);
    for (int j = 0; j < fleetSizes; j++)
    {
        fscanf(input2, "%d", &fleets[j]);
        airplanesPilotsNumber[j] = 0;
        //      printf("\n airplane size=%d", fleets[j]);
         //     getchar();
    }

    fscanf(input2, "%d", &quarantineLocations);
    fscanf(input2, "%d", &numberPilotGroups);
 //   printf("\n pilotGroups=%d", numberPilotGroups);
    for (int i = 0; i < numberPilotGroups; i++)
    {
        fscanf(input2, "%d", &pilotAirplaneNumber[i]);
        for (int j = 0; j < pilotAirplaneNumber[i]; j++)
        {
            fscanf(input2, "%d", &pilotAirplanes[i][j]);
            int airplane = pilotAirplanes[i][j];
            int pilot = i;
   //         printf("\n aiplane=%d pilot=%d", airplane, pilot);
            //   getchar();
            airplanesPilots[airplane][airplanesPilotsNumber[airplane]] = pilot;
            airplanesPilotsNumber[airplane]++;

        }
    }
    for (int i = 0; i < numberPilotGroups; i++)
        for (int t = 0; t < 499; t++)
            pilotStatus[i][t] = 1;
    for (int j = 0; j < fleetSizes - 1; j++)
        for (int k = j + 1; k < fleetSizes; k++)
            if (fleets[k] > fleets[j])
            {
                int dum = fleets[k];
                fleets[k] = fleets[j];
                fleets[j] = dum;
            }
    int capacity = 0;
    for (int j = 0; j < fleetSizes; j++)
    {
        capacity += fleets[j];
    }
    fleetCapacity = capacity;

    fclose(input2);
}