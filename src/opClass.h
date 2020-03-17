// opClass.h, jake deery, 2020
#ifndef OPCLASS_H
#define OPCLASS_H

// includes - etc
#include <mpi.h>
#include <cmath>
#include <chrono>

// namespaces - std
using std::cout;
using std::flush;
using std::locale;

// namespaces - etc
using uTimeGet = std::chrono::steady_clock;
using uTimeOut = std::chrono::duration<double, std::micro>;

class opClass {
public:
	// public methods
	opClass();
	~opClass();
	double doSearch(double startBound, double endBound);

	// publics vars
	
private:
	// private methods
	double getFit(double x, double y);

	// private vars
	int worldSize = 0;
	int worldRank = 0;
};

#endif /* OPCLASS_H */
