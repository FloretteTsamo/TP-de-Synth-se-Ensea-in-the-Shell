// Picquart Tom   Miroslava Florette
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define NULLPTR ((char *) NULL)
#define BILLION  1000000000.0f
#define BUFLEN 50
int main(int argc, char **argv)
{	pid_t pid;
	int status;
	struct timespec start, stop;
	while(1){
		const char * mess = "$ ./enseash Bienvenue dans le Shell  ENSEA. Pour quitter, tapez 'exit'.enseash % \n\r";
		int n= strlen(mess);
		write(STDOUT_FILENO, mess,n);
		char buf[n];
		int l=read(STDOUT_FILENO,buf,n);
		buf[l-1]=*"\0";
		clock_gettime( CLOCK_REALTIME, &start );
		if (strcmp(buf,"exit")==0 || l==0){
			write(STDOUT_FILENO,"Bye bye\n\r",8);
			exit(EXIT_SUCCESS);}
		if ((pid = fork()) ==-1){
			exit(EXIT_FAILURE);}
		else if (pid==0){
			execlp(buf, buf, NULLPTR);
			}
		else{
			wait(&status);
			if (WIFEXITED(status)){
				clock_gettime( CLOCK_REALTIME, &stop );
				char buffer[BUFLEN];
				float accum =  stop.tv_sec - start.tv_sec + ( stop.tv_nsec - start.tv_nsec ) / BILLION;
				sprintf(buffer,"code exit: %d | %f s\n\r",WEXITSTATUS(status),accum);
				n= strlen(buffer);
				write(STDOUT_FILENO, buffer,n);}
			else if (WIFSIGNALED(status)){
				clock_gettime( CLOCK_REALTIME, &stop );
				char buffer[BUFLEN];
				float accum =  stop.tv_sec - start.tv_sec + ( stop.tv_nsec - start.tv_nsec ) / BILLION;
				sprintf(buffer,"code exit: %d | %f s\n\r",WEXITSTATUS(status),accum);
				n= strlen(buffer);
				write(STDOUT_FILENO, buffer,n);
				}
			}
		}
		
	return 0;
}

