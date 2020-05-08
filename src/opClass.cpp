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
	float64_t startXBound = 0;
	float64_t endXBound = 0;
	float64_t startYBound = 0;
	float64_t endYBound = 0;
	float64_t bestFit[3];
	float64_t bestRank = 0.0;
	float64_t effort = 0.0;
	float64_t totalEffort = 0.0;
	float64_t bunchSize = 0.0;
	uTimeOut deltaTime;
	
	// defines
	bunchSize = defaultBoundRange / worldSize;
	
	// begin calculating current bunches -- basic
	startXBound = defaultStartBound + (bunchSize * worldRank);
	endXBound = startXBound + bunchSize; // note: corrected for precision to avoid overlap
	
	// calculating current bunches -- fix for precision to avoid overlap
	if(endXBound < 0) endXBound += precision;
	else if(endXBound == 0 || endXBound > 0) endXBound -= precision;
	
	// calculating current bunches -- re-adjust last process to cover entire range
	if(worldRank == cWorldSize) endXBound += precision;
	
	// set y-bounds to cover entire range
	startYBound = -512.0;
	endYBound = 512.0;
	
	// declaare start of search
	if(worldRank == 0) cout << "\n[I] Starting op search . . . " << flush;
	
	//
	// main for loop
	
	// getting time
	auto startTime = uTimeGet::now();
	
	for(float64_t x = startXBound; x < endXBound + precision; x += precision) { // x = limited
		for(float64_t y = startYBound; y < endYBound + precision; y += precision) { // y = do whole search-space
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
	
	// copy master effort to total
	totalEffort += effort;
	
	// alert done
	if(worldRank == 0) cout << "done" << "\n";
	
	//
	// main system echo
	if(worldRank == 0) {
		
		// for loop for slave messages
		for(uint64_t i = 0; i < (worldSize - 1); i++) { // -1 because we have already stored data from proc#0
			// vars
			float64_t currentFit[3];
			float64_t currentEffort = 0.0;
			uint64_t currentRank = 0;
			
			// wait for news
			MPI_Recv(&currentFit[0], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&currentFit[1], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&currentFit[2], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&currentEffort, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&currentRank, 1, MPI_INT, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			// if the received values are the best, keep them
			if(currentFit[2] < bestFit[2]) {
				bestFit[0] = currentFit[0];
				bestFit[1] = currentFit[1];
				bestFit[2] = currentFit[2];
				bestRank = currentRank;
			}
			
			// update totalEffort
			totalEffort += currentEffort;
		}
		
		cout << "[I] Each process ran through approx. " << effort << " loops each" << "\n";
		cout << "[I] The program ran through approx. " << totalEffort << " loops in total" << "\n";
		cout << "[I] This took approx. " << deltaTime.count() << " ms per process" << "\n";
		cout << "[I] The best fit was " << bestFit[2] << " at {" << bestFit[0] << ";" << bestFit[1] << "}" << "\n";
		cout << "[I] The best fit was found by process " << bestRank + 1 << " of " << worldSize << "\n";
		cout << "\n";
	} else {
		// send the results
		MPI_Send(&bestFit[0], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&bestFit[1], 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&bestFit[2], 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&effort, 1, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
		MPI_Send(&worldRank, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
	}
	
	// end process
	return 0;
}
