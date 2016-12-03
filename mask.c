#include "mask.h"

void forkNexec()
{
		pid_t pid;
		int status;

		switch(pid=fork())
		{
			case -1:
				perror("umask");
				exit(1);

				break;

			case 0:
				if(execlp("umask", (char *)NULL)==-1)
				{
					perror("execlp");

					exit(1);
				}

				exit(0);

				break;
			default :
				while(wait(&status)!=pid)
				    continue;

				break;
		}
}

char * storeUmaskInFile()
{
	    int fd, fd_dup;
	    int stdout_copy;
	    char *temp_file;

	    temp_file = tmpnam(NULL);

	    fd = open(temp_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	    if(fd==-1)
	    {
			perror("Excl");
										
			exit(1);
	    }

	    stdout_copy=dup(1);
	    close(1);
	    fd_dup=dup(fd);

	    forkNexec();

	    close(fd);

	    fflush(stdout);
	    dup2(stdout_copy, 1);

	    close(stdout_copy);

	    return temp_file;
}

char * readUmaskInFile(char *temp_file)
{
	    int fd, n;
	    char *buf;

	    buf = (char *)malloc(sizeof(char)*4);

	    fd = open(temp_file, O_RDONLY);
	    if(fd==-1)
	    {
			perror("Read");

			exit(1);
	    }

	    n=read(fd, buf, 3);
	    if(n==-1)
	    {
			perror("Read");
													
			exit(1);
	    }

		    buf[n]='\0';

		    return buf;
}

char * makeRanFile()
{
    char *fname;

    fname = tmpnam(NULL);

    return fname;
}
