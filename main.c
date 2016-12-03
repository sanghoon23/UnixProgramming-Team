#include "chmod.h"
#include "mask.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int auth[3][3] = {
	{ S_IRUSR, S_IWUSR, S_IXUSR },
	{ S_IRGRP, S_IWGRP, S_IXGRP },
	{ S_IROTH, S_IWOTH, S_IXOTH }
};
int pointArray = 0;

typedef struct splitChar  //�⺻ ����ü
{
	char user[4]; // 0���� user,group,all,other �̳�?,
	char command; // command('+', '-', '=')
	char permission[4]; // r, w, x
	int structPresent; // ����ü����
} splitChar;

char * accessAuth(int mode); // ����
splitChar * makeStruct(); // ����ü ����
void initStruct(splitChar *p); // �ش� ����ü 0���� �ʱ�ȭ
splitChar * splitPermString(char *perm);
void routinePerm(splitChar *p, char name[], int i);

int main(int argc, char *argv[])
{
	int k;
	struct stat buf;
	splitChar *p;
	char *mask;
	int mask_int[3];

	if (argc != 3)
	{
		printf("usage : %s <���ٱ�������> <���ٱ��Ѻ������ϸ�>\n", argv[0]);

		exit(1);
	}

	lstat(argv[2], &buf); // �ش� ������ ������ �޾ƿ´�. ���ϸ� -> buf�� ����
	printf("Before change authority : %s\n", accessAuth(buf.st_mode)); // �ٲ���� ����

	p = makeStruct(p);									// ����ü ���� / ������ ���� ����
	p->structPresent = 1;									// ����ü ���¸� 1����

	while(p->structPresent==1)								// until structPresent that finds ',' in string argv[1] doesnt become 1
	{
	    	if((argv[1][0]>='0'&&argv[1][0]<='7')&&(argv[1][1]>='0'&&argv[1][1]<='7')
		    &&(argv[1][2]>='0'&&argv[1][2]<='7')&&(argv[1][3]>='0'&&argv[1][3]<='7')
		    &&(strlen(argv[1])==4))							// if the authority is the number mode and have 4 length ex>>4644
		{
		    changePermNumMode(argv[2], argv[1]);

		    break;
		}

	    	if((argv[1][0]>='0'&&argv[1][0]<='7')&&(argv[1][1]>='0'&&argv[1][1]<='7')
		    &&(argv[1][2]>='0'&&argv[1][2]<='7')&&(strlen(argv[1])==3))			// if the authority is the number mode and have 3 length ex>>644
		{
		    changePermNumMode(argv[2], argv[1]);

		    break;
		}
		    
		p = splitPermString(argv[1]+pointArray);					// cut the argv[1] as of char ',' and check the authority is char mode

		for(k=0; k<4; k++)
		{
			if(p->user[0]=='\0')							// if there are no char like 'u', 'g', 'o', it thinks 'a'
			{
				routinePerm(p, argv[2], 0);
				routinePerm(p, argv[2], 1);
				routinePerm(p, argv[2], 2);

				if(p->command=='=')						// buf if the command is '=', we have to minus authority as umask
				{
				    mask = readUmaskInFile(storeUmaskInFile());			// store umask value in pointer value

				    mask_int[0]=(int)(*(mask+0)-48);				// change asc to int, store user umask in mask_int[0]
				    mask_int[1]=(int)(*(mask+1)-48);				// change asc to int, store user umask in mask_int[1]
				    mask_int[2]=(int)(*(mask+2)-48);				// change asc to int, store user umask in mask_int[2]

				    minusAuth_usingUmask(argv[2], mask_int);			// minus authority as much as umask
				}

				break;
			}
			else if(p->user[k]=='\0')						// we dont have to still go 'for loop' despite the string(p->user[k]) is ended
				break;
		
			if(p->user[k]=='u')							// p->user[k] is 'u'
				routinePerm(p, argv[2], 0);
			else if(p->user[k]=='g')
				routinePerm(p, argv[2], 1);					// p->user[k] is 'g'
			else if(p->user[k]=='o')
				routinePerm(p, argv[2], 2);					// p->user[k] is 'o'
			else if(p->user[k]=='a')
			{
				routinePerm(p, argv[2], 0);					// p->user[k] is 'u'
				routinePerm(p, argv[2], 1);					// p->user[k] is 'g'
				routinePerm(p, argv[2], 2);					// p->user[k] is 'o'
			}
		}
	}

	lstat(argv[2], &buf); // �ش� ������ ������ �޾ƿ´�. buf�� ����
	printf("After change authority : %s\n", accessAuth(buf.st_mode)); // �ٲ��� ����

	return 0;
}

char * accessAuth(int mode) //  �ش������� "rw_r__r__"�̷��� �������� ������ �˾ƿ��� �Լ�
{
	static char p[10];
	int i;

	for (i = 0; i <= 6; i = i + 3)
	{
		if ((mode&(S_IREAD >> i)) != 0)
			p[i] = 'r';
		else
			p[i] = '_';

		if ((mode&(S_IREAD >> (i + 1))) != 0)
			p[i + 1] = 'w';
		else
			p[i + 1] = '_';

		if ((mode&(S_IREAD >> (i + 2))) != 0)
		{
			p[i + 2] = 'x';

			if (((mode&S_ISUID) != 0) && (i == 0))
				p[i + 2] = 's';
			else if (((mode&S_ISGID) != 0) && (i == 3))
				p[i + 2] = 's';
			else if (((mode&S_ISVTX) != 0) && (i == 6))
				p[i + 2] = 't';
		}
		else
		{
			p[i + 2] = '_';

			if (((mode&S_ISUID) != 0) && (i == 0))
				p[i + 2] = 'S';
			else if (((mode&S_ISGID) != 0) && (i == 3))
				p[i + 2] = 'S';
			else if (((mode&S_ISVTX) != 0) && (i == 6))
				p[i + 2] = 'T';
		}
	}
	p[i] = '\0';

	return p;
}

splitChar * makeStruct() // ����ä ���� �� ��������
{
	splitChar *p;

	p = (splitChar*)malloc(sizeof(splitChar));

	return p;
}

void initStruct(splitChar *p) // ����ü ������ �ʱ�ȭ
{
	int i;

	for (i = 0; i<4; i++)
	{
		p->user[i] = '\0';
		p->permission[i] = '\0';
	}
	p->command = '\0';
	p->structPresent = 1;
}

splitChar * splitPermString(char *perm) // split parameter member as of char ',' then return the string after ','
{
	static splitChar p;
	int i = 0;
	
	initStruct(&p);

	// save string that is before command('+', '-', '=') in p.user
	while(((*perm)!='+')&&((*perm)!='-')&&((*perm)!='='))
	{
		if(((*perm)!='u')&&((*perm)!='g')&&((*perm)!='o')&&((*perm)!='a'))
		{
			printf("wrong input\n");

			exit(1);
		}
		p.user[i++] = *perm++;
		pointArray++;

		if(i==4)
		{
			printf("wrong input\n");
			
			exit(1);
		}
	}
	p.user[i] = '\0';
	
	// save char that is '+', '-' or '=' in p.command
	if(((*perm)!='+')&&((*perm)!='-')&&((*perm)!='='))
	{
		printf("wrong input\n");

		exit(1);
	}
	p.command = *perm++;
	pointArray++;

	if((*perm)=='\0')
	{
		printf("wrong input\n");

		exit(1);
	}

	i = 0;

	// save string that is after command('+', '-', '=') in p.permission
	while((*perm)!='\0')
	{
		if(((*perm)!='r')&&((*perm)!='w')&&((*perm)!='x'))
		{
			printf("wrong input\n");

			exit(1);
		}
		p.permission[i++] = *perm++;
		pointArray++;

		if((*perm)==',')
		{
			p.structPresent = 1;
			p.permission[i] = '\0';

			pointArray++;

			return &p;
		}
		else if(i==4)
		{
			printf("wrong input\n");

			exit(1);
		}
	}
	p.permission[i] = '\0';
	p.structPresent = 0;

	return &p;
}

void routinePerm(splitChar *p, char name[], int i) 		// �۹̼� �ִ� ��
{
	int l;
	struct stat buf;

	lstat(name, &buf);

	if (p->command == '=')						//  = ��ɹ��� ���Ե��������, ������ �׷� �ٸ���� ��� ���� ����
	{
		buf.st_mode &= ~(auth[i][0]);				// ���� ���� ����
		buf.st_mode &= ~(auth[i][1]);				// �׷� ���� ����
		buf.st_mode &= ~(auth[i][2]);				// �ٸ� ��� ���� ����
		chmod(name, buf.st_mode);				// ���� ���� ��ɾ�
	}

	for (l = 0; l<4; l++)
	{
		if (p->permission[l] == '\0')				// ���� ������� �ݺ������� ��������
			break;

		if (p->permission[l] == 'r')				// r�� ���� �ϰ��
			changePermCharMode(name, p->command, i, 0);	// chmod.h ���ִ�changePermCharMode �Լ� ���, �б� ���� ����
		else if (p->permission[l] == 'w')				// w ���� �� ���
			changePermCharMode(name, p->command, i, 1);	// chmod.h ���ִ�changePermCharMode �Լ� ���, ���� ���� ����
		else if (p->permission[l] == 'x')				// x�� ���� �ϰ��
			changePermCharMode(name, p->command, i, 2);	// chmod.h ���ִ�changePermCharMode �Լ� ���, ���� ���� ����
	}
}
