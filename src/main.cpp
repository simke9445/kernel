#include "kernel.h"

// this is the "main" program that will be executed
// which is expected to be defined by user

extern int userMain(int argc, char *argv[]);


int main(int argc, char *argv[])
{
	Kernel::init();
	int value = userMain(argc,argv);
	Kernel::term();
	return value;
}