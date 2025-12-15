// Picquart Tom   Miroslava Florette
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#define NULLPTR ((char *) NULL)
#define BUFLEN 50
int main(int argc, char **argv)
{	pid_t pid;
	int status;
	while(1){
		const char * mess = "$ ./enseash Bienvenue dans le Shell  ENSEA. Pour quitter, tapez 'exit'.enseash % \n\r";
		int n= strlen(mess);
		write(STDOUT_FILENO, mess,n);
		char buf[n];
		int l=read(STDOUT_FILENO,buf,n);
		buf[l-1]=*"\0";
		if (strcmp(buf,"exit")==0 || l==0){
			write(STDOUT_FILENO,"Bye bye\n\r",8);
			exit(EXIT_SUCCESS);}
		if ((pid = fork()) ==-1){
			exit(EXIT_FAILURE);}
		else if (pid==0){
			execlp(buf, buf, NULLPTR);
			exit(EXIT_SUCCESS);
			}
		else{
			wait(&status);
			if (WIFEXITED(status)){
				char buffer[BUFLEN];
				sprintf(buffer,"code exit: %d \n\r",WEXITSTATUS(status));
				n= strlen(buffer);
				write(STDOUT_FILENO, buffer,n);}
			else if (WIFSIGNALED(status)){
				char buffer[BUFLEN];
				sprintf(buffer," signal exit:%d \n\r",WTERMSIG(status));
				n= strlen(buffer);
				write(STDOUT_FILENO, buffer,n);}
			}
		}
		
	return 0;
}

