#include <iostream>
#include "Application.h"

int main(int argc, char *argv[])
{	
	Application application;
	
	if (application.init())
		application.run();

	return 0;
}