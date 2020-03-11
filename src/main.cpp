// city3111, main.cpp, jake deery, 2020
#include "main.h"

int main(int argc, char** argv) {
	// init - setup
	locale systemLocale("es_ES.UTF-8");
	cout.imbue(systemLocale);

	// create opClass object
	opClass * opClassObj = new opClass();

	// call the method
	opClassObj->doSearch(-8192, 8192);
	
	// delete object
	delete opClassObj;

	return 0;
}
