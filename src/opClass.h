// opClass.h, jake deery, 2020
#ifndef OPCLASS_H
#define OPCLASS_H

// includes - etc
#include <mpi.h>
#include <cmath>
#include <chrono>

// typedef - float
typedef float f32_t;
typedef long double f64_t;

// namespaces - std
using std::cout;
using std::flush;
using std::locale;

// namespaces - etc
using uTimeGet = std::chrono::steady_clock;
using uTimeOut = std::chrono::duration<f64_t, std::micro>;

class opClass {
public:
	// public methods
	opClass();
	~opClass();
	f64_t doSearch(f64_t startBound, f64_t endBound);

	// publics vars
	
private:
	// private methods
	f64_t getFit(f64_t x, f64_t y);

	// private vars
	int32_t worldSize = 0;
    int32_t worldRank = 0;
};

#endif /* OPCLASS_H */
