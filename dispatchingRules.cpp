#include <stdio.h>
#include <math.h>
#include <iostream>

#include "dispatchingRules.h"
#include "globalVariables.h"

int selectRandom(int currentQuarantine, int quarantineCapacity)
{
    if (currentQuarantine == quarantineCapacity)
        return 1000;
    int possibleNumber = 0;
    int possibleCities[100];
    double cumProbabilities[100];
    cumProbabilities[0] = 0;
    for (int i = 0; i < numberCities; i++)
    {
        if (totalCity[i] > 0)
        {
            possibleCities[possibleNumber] = i;
            possibleNumber++;
        }
    }
    double probability = 1.0 / (double)possibleNumber;
    for (int i = 0; i < possibleNumber; i++)
    {
        cumProbabilities[i + 1] = cumProbabilities[i] + probability;
        //     printf("\n i=%d cum=%f probabilit=%f",i, cumProbabilities[i],probability);
        //     getchar();
    }
    if (cumProbabilities[possibleNumber] < 1)
        cumProbabilities[possibleNumber] = 1;
    //   printf("\n possible number=%d cum0=%f cumP=%f", possibleNumber, cumProbabilities[0], cumProbabilities[possibleNumber]);
    //   getchar();
    int loc = 0;
    int	temp = rand();
    double random = (double)temp / RAND_MAX;
    //    printf("\n possible number=%d cum0=%f random=%f cumP=%f ", possibleNumber, cumProbabilities[0],random, cumProbabilities[possibleNumber]);
    //    getchar();
    do {
        //       printf("\n loc=%d choice=%d cumProb=%f",loc,possibleCities[loc],cumProbabilities[loc]);
        //       getchar();
        loc++;
    } while (random > cumProbabilities[loc]);
    loc--;
    //    printf("\n loc=%d city=%d",loc, possibleCities[loc]);
    //    getchar();
    return possibleCities[loc];
}
int selectQuarantineLAS(int currentQuarantine, int quarantineCapacity, int airplaneSize)
{
    if (currentQuarantine == quarantineCapacity)
        return 1000;
    int city = 0;
    int bestQuarantine = 100;
    int bestLAS = 0;
    int size = airplaneSize;
    if (quarantineCapacity - currentQuarantine < size)
        size = quarantineCapacity - currentQuarantine;
    int LAScity[200];
    for (int i = 0; i < numberCities; i++)
    {
        int currentSize = size;
        float cost = 0;
        for (int j = 0; j < numberGroups && currentSize >0; j++)
        {
            int groupCost = 10 - j;
            int number = numberGroupCity[i][j];
            int flyingNumber;
            if (number > currentSize)
                flyingNumber = currentSize;
            else
                flyingNumber = number;
            cost += groupCost * flyingNumber;
            currentSize -= flyingNumber;
        }
        LAScity[i] = cost;
    }
    for (int i = 0; i < numberCities; i++)
    {


        if (quarantineDays[i] <= bestQuarantine && LAScity[i] > bestLAS)
        {
            bestQuarantine = quarantineDays[i];
            bestLAS = LAScity[i];
            city = i;
        }
        //   printf("\n i=%d total city=%d priority=%d best priority=%d",i,totalCity[i], TopPriority, bestPriority);
        //  getchar();
    }
    return city;
}
int selectAirplaneCostQuarantine(int currentQuarantine, int quarantineCapacity, int airplaneSize)
{
    if (currentQuarantine == quarantineCapacity)
        return 1000;
    int city = 0;
    int maxCost = -10000;
    int size = airplaneSize;
    if (quarantineCapacity - currentQuarantine < size)
        size = quarantineCapacity - currentQuarantine;
    for (int i = 0; i < numberCities; i++)
    {
        int currentSize = size;
        float cost = 0;
        for (int j = 0; j < 10 && currentSize >0; j++)
        {
            int groupCost = 10 - j;
            int number = numberGroupCity[i][j];
            int flyingNumber;
            if (number > currentSize)
                flyingNumber = currentSize;
            else
                flyingNumber = number;
            cost += groupCost * flyingNumber;
            currentSize -= flyingNumber;
        }
        //    printf("\n city=%d cost=%f quarantine=%d size=%d", i, cost, quarantineDays[i],size);
        //    getchar();
        cost = cost / (quarantineDays[i] + 1);
        //   printf("\n city=%d cost=%f",i,cost);
       //    getchar();
        if (cost > maxCost)
        {
            maxCost = cost;
            city = i;
        }
    }
    return city;




}
int selectAirplaneCost(int currentQuarantine, int quarantineCapacity, int airplaneSize)
{
    if (currentQuarantine == quarantineCapacity)
        return 1000;
    int city = 0;
    int maxCost = -10000;
    int size = airplaneSize;
    if (quarantineCapacity - currentQuarantine < size)
        size = quarantineCapacity - currentQuarantine;
    for (int i = 0; i < numberCities; i++)
    {
        int currentSize = size;
        int cost = 0;
        for (int j = 0; j < 10 && currentSize >0; j++)
        {
            int groupCost = 10 - j;
            int number = numberGroupCity[i][j];
            int flyingNumber;
            if (number > currentSize)
                flyingNumber = currentSize;
            else
                flyingNumber = number;
            cost += groupCost * flyingNumber;
            currentSize -= flyingNumber;
        }
        if (cost > maxCost)
        {
            maxCost = cost;
            city = i;
        }
    }
    return city;
}
int selectLeastQuarantine(int currentQuarantine, int quarantineCapacity)
{
    if (currentQuarantine == quarantineCapacity)
        return 1000;
    int city = 0;
    int minQuarantine = 1000;
    int maxPop = 0;
    for (int i = 0; i < numberCities; i++)
    {
        if (quarantineDays[i] <= minQuarantine && totalCity[i] > 0 && totalCity[i] > maxPop)
        {
            minQuarantine = quarantineDays[i];
            maxPop = totalCity[i];
            city = i;
        }
        //       printf("\n i=%d total city=%d city=%d max Pop=%d",i,totalCity[i],city,maxPop);
        //      getchar();
    }
    //       printf("\n city=%d",city);
     //      getchar();
    return city;
}
int selectSumWeight(int currentQuarantine, int quarantineCapacity)
{
    if (currentQuarantine == quarantineCapacity)
        return 1000;
    int city = 0;
    int sumWeight = 0;

    for (int i = 0; i < numberCities; i++)
    {
        int tempWeight = 0;
        for (int j = 0; j < 10; j++)
        {
            int number = numberGroupCity[i][j];
            tempWeight += number * (10 - j);
        }
        if (tempWeight > sumWeight)
        {
            sumWeight = tempWeight;
            city = i;
        }


        //      printf("\n i=%d total city=%d city=%d max=%d",i,totalCity[i],city,maxCity);
        //      getchar();
    }

    return city;




}
int selectPriority(int currentQuarantine, int quarantineCapacity)
{
    if (currentQuarantine == quarantineCapacity)
        return 1000;
    int city = 0;
    int maxPriority = 10;
    int maxPriorityPopulation = 0;
    for (int i = 0; i < numberCities; i++)
    {
        for (int j = 0; j < numberGroups; j++)
        {
            int number = numberGroupCity[i][j];
            if (number > 0 && j < maxPriority)
            {
                maxPriority = j;
                maxPriorityPopulation = number;
                city = i;
            }
            if (number > maxPriorityPopulation && j == maxPriority)
            {
                maxPriorityPopulation = number;
                city = i;
            }
        }
        //      printf("\n i=%d total city=%d city=%d ",i,totalCity[i],city);
         //    getchar();
    }
    // printf("\n back city=%d ",   city);
    // getchar();
    return city;



}
int selectTotal(int currentQuarantine, int quarantineCapacity)
{
    if (currentQuarantine == quarantineCapacity)
        return 1000;
    //  printf("\n currentQuaranite=%d quarantine capacity=%d", currentQuarantine, quarantineCapacity);
     //  getchar();
    int city = 0;
    int maxCity = 0;
    for (int i = 0; i < numberCities; i++)
    {
        if (totalCity[i] > maxCity)
        {
            maxCity = totalCity[i];
            city = i;
        }
        //          printf("\n i=%d total city=%d city=%d max=%d",i,totalCity[i],city,maxCity);
         //         getchar();
    }

    return city;



}