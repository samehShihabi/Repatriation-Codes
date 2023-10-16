#pragma once
#pragma once
#pragma once
#ifndef __dispatching_h__
#define __dispatching_h__
int selectTotal(int currentQuarantine, int quarantineCapacity);
int selectPriority(int currentQuarantine, int quarantineCapacity);
int selectSumWeight(int currentQuarantine, int quarantineCapacity);
int selectLeastQuarantine(int currentQuarantine, int quarantineCapacity);
int selectAirplaneCost(int currentQuarantine, int quarantineCapacity, int airplaneSize);
int selectAirplaneCostQuarantine(int currentQuarantine, int quarantineCapacity, int airplaneSize);
int selectQuarantineLAS(int currentQuarantine, int quarantineCapacity, int airplaneSize);
int selectRandom(int currentQuarantine, int quarantineCapacity);
#endif