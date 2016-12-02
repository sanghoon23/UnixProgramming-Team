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

typedef struct splitChar  //기본 구조체
{
	char user[4]; // 0번쨰 user,group,all,other 이냐?,
	char command; // command('+', '-', '=')
	char permission[4]; // r, w, x
	int structPresent; // 구조체상태
} splitChar;

char * accessAuth(int mode); // 권항
splitChar * makeStruct(); // 구조체 선언
void initStruct(splitChar *p); // 해당 구조체 0으로 초기화

int main(int argc, char *argv[])
{
	int k;
	struct stat buf;
	splitChar *p;
	char *mask;
	int mask_int[3];

	if (argc != 3)
	{
		printf("usage : %s <접근권한지정> <접근권한변경파일명>\n", argv[0]);

		exit(1);
	}

	lstat(argv[2], &buf); // 해당 파일의 정보를 받아온다. 파일명 -> buf에 저장
	printf("Before change authority : %s\n", accessAuth(buf.st_mode)); // 바뀌기전 권한

	p = makeStruct(p);									// 구조체 생성 / 포인터 변수 연결
	p->structPresent = 1;									// 구조체 상태를 1으로

	chmod(argv[1], buf.st_mode);

	lstat(argv[2], &buf); // 해당 파일의 정보를 받아온다. buf에 저장
	printf("After change authority : %s\n", accessAuth(buf.st_mode)); // 바뀐후 권한

	return 0;
}

char * accessAuth(int mode) //  해당파일의 "rw_r__r__"이러한 형식으로 권한을 알아오는 함수
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

splitChar * makeStruct() // 구조채 생성 후 리턴해줌
{
	splitChar *p;

	p = (splitChar*)malloc(sizeof(splitChar));

	return p;
}

void initStruct(splitChar *p) // 구조체 변수들 초기화
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

void routinePerm(splitChar *p, char name[], int i) 		// 퍼미션 주는 것
{
	int l;
	struct stat buf;

	lstat(name, &buf);

	if (p->command == '=')						//  = 명령문이 포함되있을경우, 유저와 그룹 다른사람 모두 권한 삭제
	{
		buf.st_mode &= ~(auth[i][0]);				// 유저 권한 삭제
		buf.st_mode &= ~(auth[i][1]);				// 그룹 권한 삭제
		buf.st_mode &= ~(auth[i][2]);				// 다른 사람 권한 삭제
		chmod(name, buf.st_mode);				// 권한 변경 명령어
	}

	for (l = 0; l<4; l++)
	{
		if (p->permission[l] == '\0')				// 문자 없을경우 반복문에서 빠져나감
			break;

		if (p->permission[l] == 'r')				// r의 권한 일경우
			changePermCharMode(name, p->command, i, 0);	// chmod.h 에있는changePermCharMode 함수 사용, 읽기 권한 변경
		else if (p->permission[l] == 'w')				// w 권한 일 경우
			changePermCharMode(name, p->command, i, 1);	// chmod.h 에있는changePermCharMode 함수 사용, 쓰기 권한 변경
		else if (p->permission[l] == 'x')				// x의 권한 일경우
			changePermCharMode(name, p->command, i, 2);	// chmod.h 에있는changePermCharMode 함수 사용, 접근 권한 변경
	}
}