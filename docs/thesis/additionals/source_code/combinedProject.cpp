// city3111, main.h, jake deery, 2020
#pragma once
#ifndef MAIN_H
#define MAIN_H

// include - objects
#include "opClass.h"

#endif





// city3111, main.cpp, jake deery, 2020
#include "main.h"

int main(int argc, char** argv) {
	// init - setup
	locale systemLocale("fr_FR.UTF-8");
	cout.imbue(systemLocale);

	// create opClass object
	opClass * opClassObj = new opClass();

	// call the method
	opClassObj->doSearch(0.01);

	// delete object
	delete opClassObj;

	return 0;
}





// opClass.h, jake deery, 2020
#pragma once
#ifndef OPCLASS_H
#define OPCLASS_H

// includes - etc
#include <mpi.h>
#include <cmath>
#include <chrono>

// typedef - float
typedef float float32_t;
typedef double float64_t;
typedef long double float96_t;

// namespaces - std
using std::cout;
using std::flush;
using std::locale;

// namespaces - etc
using uTimeGet = std::chrono::steady_clock;
using uTimeOut = std::chrono::duration<float64_t, std::milli>;

class opClass {
public:
	// public methods
	opClass();
	~opClass();
	float64_t doSearch(float64_t precision);

	// publics vars
	
private:
	// private methods
	float64_t getFit(float64_t x, float64_t y);

	// private vars
	int32_t worldSize = 0;
	int32_t worldRank = 0;
};

#endif /* OPCLASS_H */





// opClass.cpp, jake deery, 2020
#include "opClass.h"

opClass::opClass() {
	// init - mpi
	MPI_Init(NULL, NULL);

	// Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	// Get the rank of this process
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

	if(worldRank == 0) {
		// init - cout
		cout << "[I] CITY3111 (Op1) program by Jake Deery, 2020 . . . " << "\n";
		cout << "[I] Initialising program . . . done" << "\n";

		// init - cout
		cout << "[I] Initialising op object . . . done" << "\n";
		cout << "\n";
	}
}

opClass::~opClass() {
	// end mpi
	MPI_Finalize();

	if(worldRank == 0) {
		// deleting - cout
		cout << "[I] Deleting op module . . . done" << "\n";

		// cout done
		cout << "[E] Program quitting . . . done" << "\n";
	}
}

float64_t opClass::getFit(float64_t x, float64_t y) {
	// vars
	float64_t fit;

	// calculate the fit
	fit = -1.0 * (y + 47.0) * sin(sqrt(abs(y + x / 2.0 + 47.0))) + -1.0 * x * sin(sqrt(abs(x - (y + 47.0))));

	// end process
	return fit;
}

float64_t opClass::doSearch(float64_t precision) {
	// consts
	const float64_t defaultStartBound = -512.0;
	const float64_t defaultEndBound = 512.0;
	const float64_t defaultBoundRange = 1024.0; // abs(512.0 - (-512.0))
	const float64_t cWorldSize = worldSize - 1.0;
	
	// vars
	float64_t startBound = 0;
	float64_t endBound = 0;
	float64_t bestFit[3];
	float64_t bestRank = 0.0;
	float64_t effort = 0.0;
	float64_t bunchSize = 0.0;
	uTimeOut deltaTime;

	// defines
	bunchSize = defaultBoundRange / worldSize;
	
	// begin calculating current bunches -- basic
	startBound = defaultStartBound + (bunchSize * worldRank);
	endBound = startBound + bunchSize;
	
	// calculating current bunches -- fix for precision to avoid overlap
	if(endBound < 0) endBound += precision;
	else if(endBound == 0 || endBound > 0) endBound -= precision;
	
	// calculating current bunches -- re-adjust last process to cover entire range
	if(worldRank == cWorldSize) endBound += precision;

	// declaare start of search
	if(worldRank == 0) cout << "\n[I] Starting op search . . . " << flush;

	//
	// main for loop
	
	// getting time
	auto startTime = uTimeGet::now();

	for(float64_t x = startBound; x < endBound + precision; x += precision) {
		for(float64_t y = startBound; y < endBound + precision; y += precision) {
			float64_t fitness = getFit(x,y);

			if(fitness < bestFit[2]) {
				bestFit[0] = x;
				bestFit[1] = y;
				bestFit[2] = fitness;
			};

			// effort counter
			effort++;
		}
	}
	
	// end and calculate time
	auto endTime = uTimeGet::now();
	deltaTime = endTime - startTime;
	
	// alert done
	if(worldRank == 0) cout << "done" << "\n";

	//
	// main system echo
	if(worldRank == 0) {


		// for loop for slave messages
		for(uint64_t i = 0; i < (worldSize - 1); i++) { // -1 because we have already stored data from proc#0
			// vars
			float64_t currentFit[3];
			uint64_t currentRank = 0;

			// wait for news
			MPI_Recv(&currentFit[0], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&currentFit[1], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&currentFit[2], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&currentRank, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			// if the received values are the best, keep them
			if(currentFit[2] < bestFit[2]) {
				bestFit[0] = currentFit[0];
				bestFit[1] = currentFit[1];
				bestFit[2] = currentFit[2];
				bestRank = currentRank;
			}
		}
		
		cout << "[I] Each process ran through approx. " << effort << " loops each" << "\n";
		cout << "[I] The program ran through approx. " << effort * worldSize << " loops in total" << "\n";
		cout << "[I] This took approx. " << deltaTime.count() << " ms per process" << "\n";
		cout << "[I] The best fit was " << bestFit[2] << " at {" << bestFit[0] << ";" << bestFit[1] << "}" << "\n";
		cout << "[I] The best fit was found by process " << bestRank + 1 << " of " << worldSize << "\n";
		cout << "\n";
	} else {
		// send the results
		MPI_Send(&bestFit[0], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&bestFit[1], 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&bestFit[2], 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&worldRank, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
	}

	// end process
	return 0;
}




