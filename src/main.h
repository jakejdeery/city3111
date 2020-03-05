// crunchy_breakfast_demo_code, main.h, jake deery, 2020
#ifndef MAIN_H
#define MAIN_H

// includes
#include <mpi.h>
#include <iostream>
#include <cmath>
#include <chrono>
#include <locale>

// namespaces - std
using std::cout;
using std::locale;
using std::array;

// namespaces - etc
using uTimeGet = std::chrono::steady_clock;
using uTimeOut = std::chrono::duration<double, std::micro>;

// declare methods
extern double getFit(double x, double y);

#endif
