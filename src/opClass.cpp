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

double opClass::getFit(double x, double y) {
	// vars
	double fit;

	// calculate the fit
	fit = -1.0 * (y + 47.0) * sin(sqrt(abs(y + x / 2.0 + 47.0))) + -1.0 * x * sin(sqrt(abs(x - (y + 47.0))));

	// end process
	return fit;
}

double opClass::doSearch(double startBound, double endBound) {
	// vars
	double precision = 0.01;
	double boundRange = abs(startBound - endBound);
	double localFit[3];
	double bestFit[3];
	int bestRank = 0;
	int effort = 0;
	int bunchSize = 0;

	// defines
	bunchSize = boundRange / worldSize;
	startBound = startBound + (bunchSize * worldRank);
	endBound = endBound - (bunchSize  * ((worldSize - 1) - worldRank)); // these bounds overlap, but that's okay
	
	// alert searching
	if(worldRank == 0) cout << "[I] Starting op search . . . ";

	//
	// main for loop
	for(double x = startBound; x < endBound + precision; x += precision) {
		for(double y = startBound; y < endBound + precision; y += precision) {
			double fitness = getFit(x,y);

			if(fitness < localFit[2]) {
				localFit[0] = x;
				localFit[1] = y;
				localFit[2] = fitness;
			};
			
			// effort counter
			effort++;
		}
	}
	
	// alert done
	if(worldRank == 0) cout << "done" << "\n";

	//
	// main system echo
	if(worldRank == 0) {
		// vars
		uTimeOut deltaTime;
		
		// getting time
		auto startTime = uTimeGet::now();
		
		// for loop for slave messages
		for(int i = 0; i < (worldSize - 1); i++) { // -1 because we have already stored data from proc#0
			// vars
			double currentFit[3];
			int currentRank = 0;
			
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
		
		// end and calculate time
		auto endTime = uTimeGet::now();
		deltaTime = endTime - startTime;
		
		cout << "[I] Each process ran through approx. " << effort << " loops each" << "\n";
		cout << "[I] This took approx. " << deltaTime.count() / 1000 << " ms per process" << "\n";
		cout << "[I] The best fit was " << bestFit[2] << " at {" << bestFit[0] << "," << bestFit[1] << "}" << "\n";
		cout << "[I] The best fit was found by process " << bestRank << "\n";
		cout << "\n";
	} else {
		// send the results
		MPI_Send(&localFit[0], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&localFit[1], 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&localFit[2], 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&worldRank, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
	}
	
	// end process
	return 0;
}
