#include <stdio.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iostream>



#include <ilcplex/cplex.h>
#include <ilcplex/ilocplex.h>
using namespace std;
ILOSTLBEGIN

int fleetSize;
int airplanes[50];
int quarantine;
int Tmax;

void readSolution(void);

FILE* readX;
int initialX[100][100][300];

void readData(void);
FILE* caseData;
int numberCitizens;
int numberCities;
int numberGroups;
int priorities[10];
int cityGroup[100][10];
int quarantineDuation[100];
int totalCityOriginal[100];
int numberPilotGroups;
int pilotAirplaneNumber[200];
int pilotAirplanes[200][30];

int totalOut;
void addDVs(IloEnv env, IloModel model, IloNumVarArray x,
	IloNumVarArray P, IloNumVarArray L, IloNumVarArray D, IloNumVarArray O, int time1, int time2, IloNumVarArray y, IloNumVarArray z);
int xindices[100][100][300];
int PIndices[100][20][300];
int LIndices[100][20][300];
int DIndices[100][300];
int OIndices[300];

int yindices[200][100][300];
int zindices[200][300];


int Xindex = 0;
int Yindex = 0;
int Zindex = 0;
int Pcounter = 0;
int Lcounter = 0;
int DCounter = 0;
int Ocounter = 0;
void updateL(IloEnv env, IloModel model, IloNumVarArray P, IloNumVarArray L, IloRangeArray rngLall, IloRangeArray Pinitialize, int time1, int time2);
void updateCapacity(IloEnv env, IloModel model, IloNumVarArray x, IloNumVarArray L, IloRangeArray rngCapacities, int time1, int time2);

void airplaneUse(IloEnv env, IloModel model, IloNumVarArray x, IloNumVarArray y,
	IloNumVarArray z, IloRangeArray  rngAirplaneUse);
void findDs(IloEnv env, IloModel model, IloNumVarArray L, IloNumVarArray D,
	IloRangeArray  rngD, IloNumVarArray O, IloRangeArray  rngO);
void pushXtoZero(IloEnv env, IloModel  model, IloNumVarArray  x, IloNumVarArray P, IloRangeArray  rngNoAssignment);

void startingSolution(IloEnv env, IloModel model, IloCplex cplex, IloNumVarArray x);
void solutionOutput(IloEnv env, IloModel model, IloCplex cplex, IloNumVarArray x, IloNumVarArray P, IloNumVarArray L, IloNumVarArray D, IloNumVarArray O);
void pilotQuarantine(IloEnv env, IloModel  model, IloNumVarArray  z, IloRangeArray  rngPquarantine);
int Qpilot = 7;
ILOMIPINFOCALLBACK0(trial)
{
	double val = getBestObjValue();
	double gap = getMIPRelativeGap();
	double timeS = getStartTime();
	double timeE = getCplexTime();
	int opt = getIncumbentObjValue();
	double timeDif = (timeE - timeS) / 60;
	if (timeDif > 180)
	{
		printf(" \n hi  val=%f gap=%f timeS=%f timeE=%f timeD=%f", val, gap, timeS, timeE, timeDif);
		//getchar();
		//abort();
	}
	if (gap < 0.00000001)
	{
		printf("\n in call back val=%f best obj=%d", val, opt);
		getchar();
		abort();

	}
	//getchar();
}

int main()
{

	readData();
	printf("\n end of reading data");
	getchar();
	IloEnv   env;
	IloModel model(env);
	IloNumVarArray x(env);
	IloNumVarArray P(env, 0);
	IloNumVarArray L(env, 0);
	IloNumVarArray D(env, 0);
	IloNumVarArray O(env, 0);
	IloNumVarArray y(env);
	IloNumVarArray z(env);

	IloRangeArray  rngLall(env);
	IloRangeArray  Pinitialize(env);
	IloRangeArray  rngCapacities(env);
	IloRangeArray  rngAirplaneUse(env);
	IloRangeArray  rngD(env);
	IloRangeArray  rngO(env);

	IloRangeArray  rngXY(env);
	IloRangeArray  rngYZ(env);
	IloRangeArray  rngXZ(env);
	IloRangeArray  rngXYZ(env);
	IloRangeArray  rngPquarantine(env);
	IloRangeArray  rngNoAssignment(env);

	IloExprArray airplaneCapacities(env);
	Tmax = 61;

	addDVs(env, model, x, P, L, D, O, 0, Tmax, y, z);
	printf("\n 1");
	getchar();
	updateL(env, model, P, L, rngLall, Pinitialize, 0, Tmax);
	printf("\n 2");
	getchar();
	updateCapacity(env, model, x, L, rngCapacities, 0, Tmax);
	printf("\n 3");
	getchar();
	airplaneUse(env, model, x, y, z, rngAirplaneUse);
	printf("\n 4");
	getchar();
	findDs(env, model, L, D, rngD, O, rngO);
	printf("\n 5");
	pushXtoZero(env, model, x, P, rngNoAssignment);
	printf("\n 6");
	getchar();
	pilotQuarantine(env, model, z, rngPquarantine);
//	printf("\n 7");
	IloExpr objExpression(env);
	for (int i = 0; i < numberCities; i++)
	{
		for (int j = 0; j < numberGroups; j++)
		{
			for (int t = 1; t < Tmax; t++)
			{
				int index = PIndices[i][j][t];
				//	printf("\n i=%d j=%d t=%d index=%d", i, j, t, index);
				//	getchar();
				objExpression += priorities[j] * P[index];

			}
		}
	}
	printf("\n Obj");
		getchar();
	//	getchar();
	//cout << objExpression << endl;
	//getchar();
	model.add(IloMinimize(env, objExpression));
	IloCplex cplex(model);

	//cplex.use(trial(env));
	readSolution();
	startingSolution(env, model, cplex, x);
	solutionOutput(env, model, cplex, x, P, L, D, O);
}
void solutionOutput(IloEnv env, IloModel model, IloCplex cplex, IloNumVarArray x, IloNumVarArray P, IloNumVarArray L, IloNumVarArray D, IloNumVarArray O)
{
	//	cplex.setParam(IloCplex::MIPEmphasis, 1);
	//		cplex.setParam(IloCplex::EpGap, 0.00001);
	//	cplex.setParam(IloCplex::SubMIPNodeLimit, 2);
	
	cplex.solve();
	cplex.exportModel("not correct 1.lp");
	exit(0);
	cout << "solution status" << cplex.getStatus() << endl;
	getchar();

	printf("\n objective=%f", cplex.getObjValue());
	printf("\n objective =%15.2f", cplex.getObjValue());
	getchar();


	/*for (int t = 0; t < Tmax; t++)
	{
		cout << "O(" << t << ")" << cplex.getValue(O[t]);
		getchar();
	}*/

	/*	for (int i = 0; i < numberCities; i++)
		{
			for (int j = 0; j < numberGroups; j++)
			{
				for (int t = 0; t < 1; t++)
				{
					int loc = alphaIndices[i][j][t];

					cout << "alpha(" << i << "," << j << "," << t << ")" << cplex.getValue(alpha[loc]);
					cout << "beta(" << i << "," << j << "," << t << ")" << cplex.getValue(beta[loc]);

					cout << "Pcum(" << i << "," << j << "," << t << ")" << cplex.getValue(Pcum[loc]) << endl;
					getchar();
				}
			}

		}*/


	for (int i = 0; i < numberCities; i++)
	{
		for (int k = 0; k < fleetSize; k++)
		{
			for (int t = 0; t <= Tmax; t++)

			{
				int loc = xindices[i][k][t];
				int airplaneCapacity = airplanes[k];
				if (cplex.getValue(x[loc]) == 1)
				{
					cout << "x(" << i << "," << k << "," << t << ")" <<
						cplex.getValue(x[loc]) << " capacity=" << airplaneCapacity;
					//cout << "Pcum(" << i << "," << j << "," << t << ")" << cplex.getValue(Pcum[loc]) << endl;
					getchar();

				}
			}
		}

	}
	int sum = 0;
	for (int i = 0; i < numberCities; i++)
	{
		for (int j = 0; j < numberGroups; j++)
		{
			for (int t = 0; t < Tmax - 1; t++)
			{
				int Lloc = LIndices[i][j][t];
				int loc = PIndices[i][j][t];
				int loc1 = PIndices[i][j][t + 1];
				int quantity = (int)cplex.getValue(P[loc]);
				sum += priorities[j] * quantity;
				if (cplex.getValue(P[loc]) > 0.1 && cplex.getValue(L[loc]) > 0.1) {
					//	cout << "P(" << i << "," << j << "," << t << ")" << cplex.getValue(P[loc]);
					cout << "L(" << i << "," << j << "," << t << ")" << cplex.getValue(L[loc]) << endl;
					cout << "P(" << i << "," << j << "," << t << ")" << cplex.getValue(P[loc]) << endl;
					cout << "P(" << i << "," << j << "," << t + 1 << ")" << cplex.getValue(P[loc1]) << endl;
					getchar();
				}
			}

		}
		printf("\n sum=%d", sum);
		getchar();
	}

	exit(0);



}
void pilotQuarantine(IloEnv env, IloModel  model, IloNumVarArray  z, IloRangeArray  rngPquarantine)
{
	for (int t = 0; t < Tmax - Qpilot; t++)
	{
		for (int p = 0; p < numberPilotGroups; p++)
		{
			IloExpr constQ(env);
			for (int tt = 0; tt < Qpilot; tt++)
			{

				int locZ = zindices[p][t + tt];
				constQ += z[locZ];

			}
			IloRange rangeQuarantine(env, 0, constQ, 1);
			rngPquarantine.add(rangeQuarantine);
		}


	}
	model.add(rngPquarantine);

}
void pushXtoZero(IloEnv env, IloModel  model, IloNumVarArray  x, IloNumVarArray P, IloRangeArray  rngNoAssignment)
{
	for (int t = 0; t <= Tmax; t++)
	{
		IloExpr constNumber1(env);
		for (int i = 0; i < numberCities; i++)
		{
			for (int j = 0; j < numberGroups; j++)
			{
				int Ploc = PIndices[i][j][t];
				constNumber1 += P[Ploc];
			}
			for (int k = 0; k < fleetSize; k++)
			{
				int xLoc = xindices[i][k][t];
				IloRange rangeX0(env, 0, -x[xLoc] + constNumber1, IloInfinity);
				rngNoAssignment.add(rangeX0);
			}
		}

	}
	model.add(rngNoAssignment);
}

void findDs(IloEnv env, IloModel model, IloNumVarArray L, IloNumVarArray D,
	IloRangeArray  rngD, IloNumVarArray O, IloRangeArray  rngO)
{
	for (int i = 0; i < numberCities; i++)
	{
		for (int t = 0; t < Tmax; t++)
		{
			IloExpr Dexpr(env);
			int dind = DIndices[i][t];
			if (t < quarantineDuation[i])
			{
				Dexpr -= D[dind];
				Dexpr += 0;
			}
			else
			{
				//int timeBack = t - quarantineDuation[i];
				int timeBack = t - quarantineDuation[i];
				//	printf("\n t=%d time back=%d Lind=%d duration=%d", i, t, timeBack, quarantineDuation[i]);
				//	getchar();
				Dexpr -= D[dind];
				for (int j = 0; j < numberGroups; j++)
				{
					int locL = LIndices[i][j][timeBack];
					Dexpr += L[locL];
				}
			}
			IloRange DexprConst(env, 0, Dexpr, 0);
			rngD.add(DexprConst);
		}
	}
	model.add(rngD);
	for (int t = 0; t < Tmax; t++)
	{
		IloExpr Oexpr(env);
		Oexpr += O[t];
		for (int i = 0; i < numberCities; i++)
		{
			int Dind = DIndices[i][t];
			for (int j = 0; j < numberGroups; j++)
			{
				int locL = LIndices[i][j][t];
				Oexpr -= L[locL];
			}

			Oexpr += D[Dind];
		}
		if (t > 0)
			Oexpr -= O[t - 1];
		IloRange OexprConst(env, 0, Oexpr, 0);
		rngO.add(OexprConst);
	}
	model.add(rngO);



}
void airplaneUse(IloEnv env, IloModel model, IloNumVarArray x, IloNumVarArray y,
	IloNumVarArray z, IloRangeArray  rngAirplaneUse)
{
	for (int k = 0; k < fleetSize; k++)
		for (int t = 0; t < Tmax; t++)
		{
			IloExpr totalflights(env);
			for (int i = 0; i < numberCities; i++)
			{
				int loc = xindices[i][k][t];
				totalflights += x[loc];
			}
			IloRange totalFlightsConst(env, 0, totalflights, 1);
			rngAirplaneUse.add(totalFlightsConst);
		}
		model.add(rngAirplaneUse);
	for (int k = 0; k < fleetSize; k++)
		for (int t = 0; t < Tmax; t++)
		{

			for (int i = 0; i < numberCities; i++)
			{
				IloExpr constXY1(env);
				int loc = xindices[i][k][t];
				constXY1 += x[loc];

				IloExpr constXY2(env);
				for (int l = 0; l < numberPilotGroups; l++)
				{
					int loc = yindices[l][k][t];
					constXY2 += y[loc];
				}
				IloRange constXY(env, 0, -constXY1 + constXY2, IloInfinity);
				rngAirplaneUse.add(constXY);
			}
		}

	//model.add(rngXY);


	for (int p = 0; p < numberPilotGroups; p++)
		for (int t = 0; t < Tmax; t++)
		{
			IloExpr numberFlights(env);
			for (int k = 0; k < fleetSize; k++)
			{
				int loc = yindices[p][k][t];
				numberFlights += y[loc];
			}
			IloRange totalFlightsConst(env, 0, numberFlights, 1);
			rngAirplaneUse.add(totalFlightsConst);
		}
	for (int k = 0; k < fleetSize; k++)
		for (int t = 0; t < Tmax; t++)
		{
			IloExpr numberPilots(env);
			for (int p = 0; p < numberPilotGroups; p++)
			{
				int loc = yindices[p][k][t];
				numberPilots += y[loc];
			}
			IloRange totalFlightsConst(env, 0, numberPilots, 1);
			rngAirplaneUse.add(totalFlightsConst);
		}

	for (int p = 0; p < numberPilotGroups; p++)
		for (int t = 0; t < Tmax; t++)
		{
			IloExpr RHSpilot(env);
			int zLoc = zindices[p][t];
			RHSpilot += z[zLoc];
			for (int k = 0; k < fleetSize; k++)
			{
				int yloc = yindices[p][k][t];
				IloExpr LHSpilot(env);
				LHSpilot += y[yloc];
				IloRange totalFlightsConst(env, 0, RHSpilot - LHSpilot, 1);
				rngAirplaneUse.add(totalFlightsConst);
			}

		}

	model.add(rngAirplaneUse); 
}
void updateCapacity(IloEnv env, IloModel model, IloNumVarArray x, IloNumVarArray L, IloRangeArray rngCapacities, int time1, int time2)
{
	for (int t = time1; t < time2; t++)
	{
		for (int i = 0; i < numberCities; i++)
		{
			IloExpr Lexpression(env);

			for (int j = 0; j < numberGroups; j++)
			{
				int locL = LIndices[i][j][t];
				Lexpression -= L[locL];
			}

			for (int k = 0; k < fleetSize; k++)
			{
				int xLoc = xindices[i][k][t];
				Lexpression += x[xLoc] * airplanes[k];
			}
			IloRange constraint(env, 0, Lexpression, INFINITY);
			rngCapacities.add(constraint);
		}
	}
	model.add(rngCapacities);
}

void updateL(IloEnv env, IloModel model, IloNumVarArray P, IloNumVarArray L, IloRangeArray rngLall, IloRangeArray Pinitialize, int time1, int time2)
{
	for (int i = 0; i < numberCities; i++)
	{
		for (int j = 0; j < numberGroups; j++)
		{
			IloExpr Pexpression(env);
			int locP1 = PIndices[i][j][0];
			Pexpression += P[locP1];
			IloRange constraint(env, cityGroup[i][j], Pexpression, cityGroup[i][j]);
			Pinitialize.add(constraint);
		}
	}
	for (int i = 0; i < numberCities; i++)
	{
		for (int j = 0; j < numberGroups; j++)
		{
			IloExpr Pexpression(env);
			int locP1 = PIndices[i][j][10];
			Pexpression += P[locP1];
			IloRange constraint(env, 0, Pexpression, 0);
			//		Pinitialize.add(constraint);
		}
	}
	model.add(Pinitialize);
	for (int i = 0; i < numberCities; i++)
	{
		for (int j = 0; j < numberGroups; j++)
		{
			for (int t = 1; t < time2; t++)
			{
				IloExpr Lexpression(env);
				int locL = LIndices[i][j][t - 1];
				int locP1 = PIndices[i][j][t - 1];
				int locP2 = PIndices[i][j][t];
				Lexpression -= L[locL];
				Lexpression -= P[locP2];
				Lexpression += P[locP1];
				IloRange constraint(env, 0, Lexpression, 0);
				rngLall.add(constraint);
				//	cout << P[locP1] << endl;
				//	cout<< P[locP2] << endl;
				//	cout << constraint << endl;
				//	printf("\n locL=%d locP1=%d locP2=%d",locL,locP1,locP2);
				//	getchar();
			}
		}
	}
	model.add(rngLall);
}
void startingSolution(IloEnv env, IloModel model, IloCplex cplex, IloNumVarArray x)
{
	IloNumVarArray startVar(env);
	IloNumArray startVal(env);
	for (int i = 0; i < numberCities; ++i)
		for (int j = 0; j < fleetSize; ++j)
			for (int t = 0; t < Tmax; t++)
			{
				int xLoc = xindices[i][j][t];
				startVar.add(x[xLoc]);
				startVal.add(initialX[i][j][t]);
				/*	if (initialX[i][j][t] == 1)
					{
						printf("\n flight i=%d j=%d t=%d",i,j,t);
						getchar();

					}*/
			}
	cplex.addMIPStart(startVar, startVal);
	startVal.end();
	startVar.end();



}
void addDVs(IloEnv env, IloModel model, IloNumVarArray x,
	IloNumVarArray P, IloNumVarArray L, IloNumVarArray D, IloNumVarArray O, int time1, int time2, IloNumVarArray y, IloNumVarArray z)
{
	printf("\n number citizens=%d number cities=%d number groups=%d", numberCitizens, numberCities, numberGroups);
	getchar();
	for (int i = 0; i < numberCities; i++)
	{
		for (int k = 0; k < fleetSize; k++)
		{
			for (int t = time1; t <= time2; t++)
			{
				char name[10];
				char name1[2];
				char name2[2];
				char name3[2];
				strcpy(name, "X(");
				_itoa(i, name1, 10);
				_itoa(k, name2, 10);
				_itoa(t, name3, 10);
				strcat(name, name1);
				strcat(name, ",");
				strcat(name, name2);
				strcat(name, ",");
				strcat(name, name3);
				strcat(name, ")");
				x.add(IloNumVar(env, 0.0, 1.0, ILOINT, name));
				//	printf("\n i=%d k=%d t=%d",i,k,t);
				xindices[i][k][t] = Xindex;
				Xindex++;
			}//end of t
		}//end of k
	}//end of i
	model.add(x);

	for (int p = 0; p < numberPilotGroups; p++)
	{
		for (int k = 0; k < fleetSize; k++)
		{
			for (int t = time1; t <= time2; t++)
			{
				char name[10];
				char name1[2];
				char name2[2];
				char name3[2];
				strcpy(name, "Y(");
				_itoa(p, name1, 10);
				_itoa(k, name2, 10);
				_itoa(t, name3, 10);
				strcat(name, name1);
				strcat(name, ",");
				strcat(name, name2);
				strcat(name, ",");
				strcat(name, name3);
				strcat(name, ")");
				int checkPossibility = 0;
				for (int jj = 0; jj < pilotAirplaneNumber[jj]; jj++)
					if (pilotAirplanes[p][jj] == k)
						checkPossibility = 1;
		//		printf("\n p=%d k=%d t=%d possibility=%d",p,k,t,checkPossibility);
			//	getchar();
				if (checkPossibility == 1)
					y.add(IloNumVar(env, 0.0, 1.0, ILOINT, name));
				else
					y.add(IloNumVar(env, 0.0, 0.0, ILOINT, name));
				//	printf("\n i=%d k=%d t=%d",i,k,t);
				yindices[p][k][t] = Yindex;
				Yindex++;
			}//end of t
		//	getchar();
		}//end of k
	}//end of i
	model.add(y);

	for (int p = 0; p < numberPilotGroups; p++)
	{
		for (int t = time1; t <= time2; t++)
		{
			char name[10];
			char name1[2];
			char name2[2];
			char name3[2];
			strcpy(name, "Z(");
			_itoa(p, name1, 10);
			_itoa(t, name3, 10);
			strcat(name, name1);
			strcat(name, ",");
			strcat(name, name2);
			strcat(name, ",");
			strcat(name, name3);
			strcat(name, ")");
			z.add(IloNumVar(env, 0.0, 1.0, ILOINT, name));
			//	printf("\n i=%d k=%d t=%d",i,k,t);
			zindices[p][t] = Zindex;
			Zindex++;
		}//end of t

	}//end of i
	model.add(z);

	for (int i = 0; i < numberCities; i++)
	{
		//	printf("\n i=%d index=%d",i, PcumIndices[i]);
		//	getchar();
		for (int j = 0; j < numberGroups; j++)
			for (int t = time1; t <= time2; t++)
			{
				char name[10];
				char name1[2];
				char name2[2];
				char name3[2];
				strcpy(name, "P(");
				_itoa(i, name1, 10);
				_itoa(j, name2, 10);
				_itoa(t, name3, 10);
				strcat(name, name1);
				strcat(name, ",");
				strcat(name, name2);
				strcat(name, ",");
				strcat(name, name3);
				strcat(name, ")");
				P.add(IloNumVar(env, 0.0, 2000000, ILOFLOAT, name));
				PIndices[i][j][t] = Pcounter;
				Pcounter++;
			}

	}
	model.add(P);

	for (int i = 0; i < numberCities; i++)
	{
		//	printf("\n i=%d index=%d",i, PcumIndices[i]);
		//	getchar();
		for (int j = 0; j < numberGroups; j++)
			for (int t = time1; t <= time2; t++)
			{
				char name[10];
				char name1[2];
				char name2[2];
				char name3[2];
				strcpy(name, "L(");
				_itoa(i, name1, 10);
				_itoa(j, name2, 10);
				_itoa(t, name3, 10);
				strcat(name, name1);
				strcat(name, ",");
				strcat(name, name2);
				strcat(name, ",");
				strcat(name, name3);
				strcat(name, ")");
				L.add(IloNumVar(env, 0.0, 2000000, ILOFLOAT, name));
				LIndices[i][j][t] = Lcounter;
				//	printf("\n i=%d t=%d Lindex=%d", i, t, LallIndices[i][t]);
				//	getchar();
				Lcounter++;

			}

	}
	model.add(L);





	for (int i = 0; i < numberCities; i++)
	{
		for (int t = 0; t < Tmax; t++)
		{
			char name[10];
			char name1[2];
			char name2[2];
			char name3[2];
			strcpy(name, "D(");
			_itoa(i, name1, 10);
			_itoa(t, name3, 10);
			strcat(name, name1);
			strcat(name, ",");
			strcat(name, name3);
			strcat(name, ")");
			D.add(IloNumVar(env, 0.0, 200000, ILOFLOAT, name));
			DIndices[i][t] = DCounter;
			DCounter++;
		}

	}
	//	model.add(D);

	for (int t = 0; t < Tmax; t++)
	{
		char name[10];

		char name3[2];
		strcpy(name, "O(");
		_itoa(t, name3, 10);
		strcat(name, ")");
		O.add(IloNumVar(env, 0.0, quarantine, ILOFLOAT, name));
		OIndices[t] = Ocounter;
		Ocounter++;
	}


	//	model.add(D);
}


void readSolution(void)
{
	readX = fopen("44000-var-40-4.txt", "r");
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 50; j++)
			for (int k = 0; k < 200; k++)
			{
				fscanf(readX, "%d", &initialX[i][j][k]);

				/*	if (initialX[i][j][k] == 1)
					{
						printf("\n flight i=%d j=%d t=%d", i, j, k);
						getchar();

					}*/
			}


}

void readData(void)
{
	//	caseData = fopen("OneMillionCaseVar.txt", "r");
	//caseData = fopen("OneMillionCase14.txt", "r");
//	caseData = fopen("returnCasesJordanVar-90-1.txt", "r");
	   caseData = fopen("returnCasesJordanVar-90-40-4.txt", "r");
	//caseData = fopen("returnCasesJordan0-1.txt", "r");
//	caseData = fopen("OneMillionCase0.txt", "r");
	//	caseData = fopen("returnCasesJordan0.txt", "r");
	//caseData = fopen("returnCasesJordan14-90-77.txt", "r");
	//	caseData = fopen("returnCasesJordan14.txt", "r");
	fscanf(caseData, "%d", &numberCitizens);
	fscanf(caseData, "%d", &numberCities);
	fscanf(caseData, "%d", &numberGroups);
	printf("\n number citizens=%d number cities=%d number groups=%d", numberCitizens, numberCities, numberGroups);
	getchar();
	for (int i = 0; i < numberGroups; i++)
		fscanf(caseData, "%d", &priorities[i]);
	for (int i = 0; i < numberCities; i++)
	{
		totalCityOriginal[i] = 0;
		int dum;
		fscanf(caseData, "%d", &dum);
		for (int j = 0; j < numberGroups; j++)
		{
			fscanf(caseData, "%d", &cityGroup[i][j]);
			totalCityOriginal[i] += cityGroup[i][j];
		}
		fscanf(caseData, "%d", &quarantineDuation[i]);
	}
	fscanf(caseData, "%d", &fleetSize);
	int sumCapacity = 0;
	for (int i = 0; i < fleetSize; i++)
	{
		fscanf(caseData, "%d", &airplanes[i]);
		sumCapacity += airplanes[i];
	}
	fscanf(caseData, "%d", &quarantine);
	printf("\n fleet size=%d quarantine=%d capacity=%d", fleetSize, quarantine, sumCapacity);
	getchar();
	fscanf(caseData, "%d", &numberPilotGroups);
  printf("\n pilotGroups=%d", numberPilotGroups);
	for (int i = 0; i < numberPilotGroups; i++)
	{
		fscanf(caseData, "%d", &pilotAirplaneNumber[i]);
		for (int j = 0; j < pilotAirplaneNumber[i]; j++)
		{
			fscanf(caseData, "%d", &pilotAirplanes[i][j]);
			int airplane = pilotAirplanes[i][j];
			int pilot = i;
			//         printf("\n aiplane=%d pilot=%d", airplane, pilot);
					 //   getchar();
			

		}
	}
}