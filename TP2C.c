
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#define NULLPTR ((char *) NULL)
int main(int argc, char **argv)
{	pid_t pid;
 	int status;
	while(1){
		const char * mess = "$ ./enseash Bienvenue dans le Shell  ENSEA. Pour quitter, tapez 'exit'.enseash % \n\r";
		int n= strlen(mess);
		write(STDOUT_FILENO, mess,n);
		char buf[n];
		int l=read(STDOUT_FILENO,buf,n);
		// on modifie le dernier caractère de buf pour permettre l'exécution avec execlp
		buf[l-1]=*"\0";
		if ((pid = fork()) ==-1) //erreur
		{
			exit(EXIT_FAILURE);}
		else if (pid==0)//fils
		{
			execlp(buf, buf, NULLPTR);
			}
		else // père
		{
			wait(&status);
			}
		}
		
	return 0;
}



