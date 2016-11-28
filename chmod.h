#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void changePermCharMode(char name[], char command, int i, int j);
void changePermNumMode(char name[], char perm[]);
void minusAuth_usingUmask(char name[], int mask_int[]);
