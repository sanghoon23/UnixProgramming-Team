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

	chmod(argv[1], buf.st_mode);

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