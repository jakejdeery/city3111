// crunchy_breakfast_demo_code, main.cpp, jake deery, 2020
#include "main.h"

double getFit(double x, double y) {
	// vars
	double fit;

	// calculate the fit
	fit = -1.0 * (y + 47.0) * sin(sqrt(abs(y + x / 2.0 + 47.0))) + -1.0 * x * sin(sqrt(abs(x - (y + 47.0))));

	// end process
	return fit;
}

int main(int argc, char** argv) {
	// vars
	double startBound = -128.0;
	double endBound = 128.0;
	double precision = 0.01;
	double boundRange = abs(startBound - endBound);
	int worldSize = 0;
	int worldRank = 0;

	// setup
	locale systemLocale("en_GB.UTF-8");
	cout.imbue(systemLocale);

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	// Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	// Get the rank of this process
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

	// Get the bunch size
	int bunchSize = boundRange / worldSize;

	// main system echo
	if(worldRank == 0) {
		// vars
		int iTotal = 0;
		double bestFit[4];
		uTimeOut deltaTime;
		
		cout << "[I] Op1 program by Jake Deery, 2020" << "\n";
		cout << "[I] Program commencing, please wait . . . " << "\n";
		cout << "\n";
		
		// getting time
		auto startTime = uTimeGet::now();
		
		// for loop for slave messages
		for(int i = 0; i < (worldSize - 1); i++) {
			// vars
			int iCurrent = 0;
			double currentFit[4];
			
			// wait for news
			MPI_Recv(&currentFit[0], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&currentFit[1], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&currentFit[2], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&currentFit[3], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&iCurrent, 1, MPI_INT, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			// calculate iterations
			iTotal += iCurrent;
			
			// if the received values are the best, keep them
			if(currentFit[2] < bestFit[2]) {
				bestFit[0] = currentFit[0];
				bestFit[1] = currentFit[1];
				bestFit[2] = currentFit[2];
			}
		}
		
		// end and calculate time
		auto endTime = uTimeGet::now();
		deltaTime = endTime - startTime;
		
		cout << "[I] The program ran through " << iTotal << " loops in " << deltaTime.count() / 1000 << " ms" << "\n";
		cout << "[I] The best fit was " << bestFit[2] << " at {" << bestFit[0] << "," << bestFit[1] << "}" << "\n";
		cout << "[E] Program quits" << "\n";
		cout << "\n";
	} else {
		// vars
		double localFit[3];
		double fitness = 0.0;
		int i = 0;
		
		// defines
		startBound = startBound + (bunchSize * worldRank);
		endBound = endBound - (bunchSize  * ((worldSize - 1) - worldRank));

		// main for loop
		for(double x = startBound; x < endBound + precision; x += precision) {
			for(double y = startBound; y < endBound + precision; y += precision) {
				fitness = getFit(x,y);

				if(fitness < localFit[2]) {
					localFit[0] = x;
					localFit[1] = y;
					localFit[2] = fitness;
				};

				// alert counter
				i++;
			}
		}
		
		// send the results to the master process
		MPI_Send(&localFit[0], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&localFit[1], 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&localFit[2], 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&worldRank, 1, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
		MPI_Send(&i, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
	}
	
	// end process
	MPI_Finalize();
	return 0;
}

