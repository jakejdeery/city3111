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
	array<double, 3> bestFit;
	double startBound = -16384.0;
	double endBound = 16384.0;
	double boundRange = abs(startBound - endBound);
	double fitness;
	int worldSize;
	int worldRank;
	int i;
	uTimeOut deltaTime;

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
		cout << "[I] Op1 program by Jake Deery, 2020" << "\n";
		cout << "[I] Program commencing, please wait . . . " << "\n";
		cout << "\n";
	}

	// vars
	startBound = startBound + (bunchSize * worldRank);
	endBound = endBound - (bunchSize  * ((worldSize - 1) - worldRank));
	i = 0;

	// getting time
	auto startTime = uTimeGet::now();

	// main for loop
	for(double x = startBound; x < endBound + .1; x += .1) {
		for(double y = startBound; y < endBound + .1; y += .1) {
			fitness = getFit(x,y);

			if(fitness < bestFit[2]) {
				bestFit[0] = x;
				bestFit[1] = y;
				bestFit[2] = fitness;
			};

			// alert counter
			i++;
		}
	}

	// end and calculate time
	auto endTime = uTimeGet::now();
	deltaTime = endTime - startTime;

	// report findings
	cout << "[I] Process " << worldRank << " ran through " << i << " loops in " << deltaTime.count() / 1000 << "ms\n";
	cout << "[I] Process " << worldRank << " says best fit was " << bestFit[2] << " found at [" << bestFit[0] << "," << bestFit[1] << "]\n";
	cout << "\n";

	// end process
	MPI_Finalize();
	return 0;
}

