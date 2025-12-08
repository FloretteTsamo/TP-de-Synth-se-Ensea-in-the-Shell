#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#define NULLPTR ((char *) NULL)
int main(int argc, char **argv)
{	
 
	execlp("ls", "ls", NULLPTR);
	return 0;
}
