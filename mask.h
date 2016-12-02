#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void forkNexec();
char * storeUmaskInFile();
char * readUmaskInFile(char *temp_file);
char * makeRanFile();

