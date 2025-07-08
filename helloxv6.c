#include "types.h"
#include "stat.h"
#include "user.h"
int 
main(int argc, char **argv)
{
	printf(1, "Hello xv6 World\n");
	// user.h: void printf(int, const char *, ...);
	exit();
	// user.h: int exit(void) __attribute__((noreturn));
}
