#include <main.h>
#include "tp1.h"

int main(int argc, const char* argv[])
{
	// First argument is log file, if it exists
	if (argc >= 1) {
		Log::SetFile(argv[0]);
	}

	// Don't forget that CoreTP1 inherits from Core ...
	CoreTP1 core;

	// Run game
	core.Run();

	return 0;
}