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
