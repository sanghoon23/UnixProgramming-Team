#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void forkNexec()
{
	pid_t pid;
	int status;

	switch( pid = fork())
	{
		case -1 :
			perror("umask");
			exit(1);

			break;

		case 0 :
			if(execlp(" ", (char *)NULL) == -1)
			{
				perror("execlp");

				exit(1);
			}

		default :
			break;
	}


}

char *storeUmaskInFile()
{
	int fd;
	
	char * temp_file = tmpnam(NULL);

	temp_file = tmpnam(NULL);

	fd = open(temp_file, O_CREAT | O_WRONLy | O_TRUNC, 0644);
	if(fd == -1)
	{
		perror("Excl");
		exit(1);
	}
}
