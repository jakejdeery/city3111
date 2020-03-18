// city3111, main.cpp, jake deery, 2020
#include "main.h"

int main(int argc, char** argv) {
	// init - setup
	locale systemLocale("fr_FR.UTF-8");
	cout.imbue(systemLocale);

	// create opClass object
	opClass * opClassObj = new opClass();

	// call the method
	opClassObj->doSearch(-512.0, 512.0);

	// delete object
	delete opClassObj;

	return 0;
}
