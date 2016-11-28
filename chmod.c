#include "chmod.h"

int auth_[3][3] = {
		{S_IRUSR, S_IWUSR, S_IXUSR},
		{S_IRGRP, S_IWGRP, S_IXGRP},
		{S_IROTH, S_IWOTH, S_IXOTH}
		};

void changePermCharMode(char name[], char command, int i, int j)	// function that how to change authority depending on command(if the authority mode is the char mode ex>> u+rx,o-x)
{
	struct stat buf;

	lstat(name, &buf);

	switch(command)
	{
		case '+':
			buf.st_mode |= auth_[i][j];
			chmod(name, buf.st_mode);
			break;

		case '-':
			buf.st_mode &= ~(auth_[i][j]);
			chmod(name, buf.st_mode);
			break;

		case '=':
			buf.st_mode |= auth_[i][j];
			chmod(name, buf.st_mode);
			break;
	}
}

void changePermNumMode(char name[], char perm[])	// function that how to change authority depending on authoirty number(if the authority mode is the number mode ex>> 4755, 644)
{
	int i, j;
    	struct stat buf;
	int temp_num;
	int num;

	lstat(name, &buf);

	if(strlen(perm)==4)				// if the authorirty number's length is 4(ex>> 4755, 2644)
	{
		num=atoi(perm);				// make asc to int perm string
		temp_num=num/1000;			// get thousands in num variable

		buf.st_mode &= ~S_ISUID;		// delete setuid authority
	    	buf.st_mode &= ~S_ISGID;		// delete setgid authority
	    	buf.st_mode &= ~S_ISVTX;		// delete sticky bit authority
		switch(temp_num)
		{
			case 0 :
				break;
	    		case 1 :
	    			buf.st_mode |= S_ISVTX;
				break;
	    		case 2 :
	    			buf.st_mode |= S_ISGID;
				break;
	    		case 3 :
	    			buf.st_mode |= S_ISGID;
	    			buf.st_mode |= S_ISVTX;
				break;
	    		case 4 :
	    			buf.st_mode |= S_ISUID;
				break;
	    		case 5 :
	    			buf.st_mode |= S_ISUID;
	    			buf.st_mode |= S_ISVTX;
				break;
	    		case 6 :
	    			buf.st_mode |= S_ISUID;
	    			buf.st_mode |= S_ISGID;
	    		case 7 :
	    			buf.st_mode |= S_ISUID;
	    			buf.st_mode |= S_ISGID;
	    			buf.st_mode |= S_ISVTX;
				break;
		}
		chmod(name, buf.st_mode);
	}

	num=atoi(perm);
	for(i=0; i<3; i++)
	{
    		buf.st_mode &= ~(auth_[i][0]);
		buf.st_mode &= ~(auth_[i][1]);
	    	buf.st_mode &= ~(auth_[i][2]);
	    
	    	if(i==0) temp_num=num/100%10;
	    	else if(i==1) temp_num=num/10%10;
	    	else if(i==2) temp_num=num%10;
	    	switch(temp_num)
	    	{
			case 0 :
				break;
			case 1 :
				buf.st_mode |= auth_[i][2]; 
				break;
			case 2 :
				buf.st_mode |= auth_[i][1];
				break;
			case 3 :
				buf.st_mode |= auth_[i][2];
				buf.st_mode |= auth_[i][1];
				break;
			case 4 :
				buf.st_mode |= auth_[i][0];
				break;
			case 5 :
				buf.st_mode |= auth_[i][2];
				buf.st_mode |= auth_[i][0];
				break;
			case 6 :
				buf.st_mode |= auth_[i][1];
				buf.st_mode |= auth_[i][0];
				break;
			case 7 :
				buf.st_mode |= auth_[i][2];
				buf.st_mode |= auth_[i][1];
				buf.st_mode |= auth_[i][0];
				break;
		}
		chmod(name, buf.st_mode);
	}
}

void minusAuth_usingUmask(char name[], int mask_int[]) // function that minus authority using umask
{
    int i;

    for(i=0; i<3; i++)
    {
	switch(mask_int[i])
	{
	    case 0 :
		break;
	    case 1 :
	    	changePermCharMode(name, '-', i, 2);
		break;
	    case 2 :
	    	changePermCharMode(name, '-', i, 1);
		break;
	    case 3 :
	    	changePermCharMode(name, '-', i, 2);
	    	changePermCharMode(name, '-', i, 1);
		break;
	    case 4 :
	    	changePermCharMode(name, '-', i, 0);
		break;
	    case 5 :
	    	changePermCharMode(name, '-', i, 2);
	    	changePermCharMode(name, '-', i, 0);
		break;
	    case 6 :
	    	changePermCharMode(name, '-', i, 1);
	    	changePermCharMode(name, '-', i, 0);
		break;
	    case 7 :
	    	changePermCharMode(name, '-', i, 2);
	    	changePermCharMode(name, '-', i, 1);
	    	changePermCharMode(name, '-', i, 0);
		break;
	}

    }
}
