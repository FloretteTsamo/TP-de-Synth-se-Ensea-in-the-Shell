#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char **argv)
{	const char * mess = "$ ./enseash Bienvenue dans le Shell  ENSEA. Pour quitter, tapez 'exit'.enseash % \n\r";
	int n= strlen(mess);
	write(STDOUT_FILENO, mess,n);
	char buf[n];
	read(STDOUT_FILENO,buf,n);
	int l=strlen(buf);
	write(STDOUT_FILENO,buf,l);
	
	return 0;
}