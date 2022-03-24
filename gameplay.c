//Header file
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h> //mac에선 없는 헤더파일
#include<conio.h> //mac에선 없는 헤더파일

//Global Variables Declaration
char selection = 0;
int map[20][14];
int copymap[20][14];

// add block
int blockset[7][4][4][4];
int block[4][4]; //떨어지는 블록의 좌표
int block_x[4];
int block_y[4];
int blocktype = 0;
int blockphase = 0;

// 블럭의 회전에서 필요한 함수 >> 지역변수로 대체해야 함!
int originalpoint_x = 0;
int originalpoint_y = 0;
int tempblock_x[4];
int tempblock_y[4];

int templines[19][12]; //쓰지 않는 배열임
int clearlineposition[5];

int score = 0;

//Functions Declararion
void HideCursor();
void StartScreen();
void PressAnyKey();
void GamePlay();
void HelpScreen();

void GetKeyInput();

void Gotoxy(int x, int y);
void PrintMap();
void PrintBlock();
void EraseBlock();
void PrintStatus();

void CreateBlock();
void ChooseRandomBlock();
void AddBlocktoMap();
void BlockMoveDown();
void BlockMoveLeft();
void BlockMoveRight();
void RotateBlock();

int CheckBlockCollisionDown();
int CheckBlockCollisionLeft();
int CheckBlockCollisionRight();
int CheckBlockCollisionRotate();

void CheckGameover(); //아직 덜 만든 함수!
void CheckLineClear();
void LineClear();
void RemoveLine(int lineidx);
void MoveLinesDownward(int lineidx);

/***************************************************************************/
int main() {
	HideCursor();
	system("cls");
	system("mode con cols=80 lines=30");
	while (1) {
		StartScreen();
		PressAnyKey();
		selection = _getch();
		system("cls");
		switch (selection) {
		case '1': GamePlay(); return 0;
		case '2': HelpScreen(); break;
		}
	} //while
	return 0;
}

//Functions
void HideCursor() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
} //커서를 숨기는 함수

void StartScreen() {
	printf("####################\n");
	Sleep(100);
	printf("#    * G A M E *   #\n");
	printf("#  *** TETRIS ***  #\n");
	Sleep(100);
	printf("#                  #\n");
	printf("#                  #\n");
	Sleep(100);
	printf("#   * 1 : start *  #\n");
	printf("#                  #\n");
	Sleep(100);
	printf("#                  #\n");
	printf("#                  #\n");
	Sleep(100);
	printf("#   * 2 : help  *  #\n");
	printf("#                  #\n");
	Sleep(100);
	printf("#                  #\n");
	printf("#                  #\n");
	Sleep(100);
	printf("####################");
}

void HelpScreen() {
	printf("help me!");
}

void PressAnyKey() {
	while (1) {
		if (_kbhit()) {
			return;
		}
	}
}
void GamePlay() {
	//Init
	int framecount = 0;

	clock_t CurTime, OldTime, OldTimeBlockMove;
	OldTime = clock();
	OldTimeBlockMove = clock();

	CreateBlock();
	
	while (1) {
		//Data Update
		//Keyboard input
		GetKeyInput();

		//Implementation before print(BlockMoveDown, LineClear)
		CurTime = clock();
		if (CurTime - OldTimeBlockMove > 500) {
			OldTimeBlockMove = CurTime;
			//Implementation
			if (CheckBlockCollisionDown() == 0) {
				BlockMoveDown();
			}
			else {
				EraseBlock();
				AddBlocktoMap();
				CheckLineClear();
				LineClear();
				system("cls");
				CreateBlock();
				//Add Score

			}
		}

		//Print
		PrintMap();
		PrintBlock();
		PrintStatus();

		//Wait
		while (1) {
			CurTime = clock();
			if (CurTime - OldTime > 100) {
				OldTime = CurTime;
				break;
			}
		}
		CheckGameover();
		framecount++;
	}
}

/***************************************************************************/

void GetKeyInput() {
	int inp;
	if (_kbhit()) {
		inp = _getch();
		if (inp == 'c') {
			if (CheckBlockCollisionRotate()==0) {
				RotateBlock();
			}
		}
		switch (inp) {
		case 'j': {
			if (CheckBlockCollisionLeft()) { break; }
			BlockMoveLeft();
			break;
		}
		case 'J': {
			if (CheckBlockCollisionLeft()) { break; }
			BlockMoveLeft();
			break;
		}
		case 'l': {
			if (CheckBlockCollisionRight()) { break; }
			BlockMoveRight();
			break;
		}
		case 'L': {
			if (CheckBlockCollisionRight()) { break; }
			BlockMoveRight();
			break;
		}
		}
	}
}

/***************************************************************************/
void Gotoxy(int x, int y) {
	COORD CursorPosition = { 2*x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPosition);
}

void PrintMap() {
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 14; j++) {
			if (map[i][j] == 1) {
				Gotoxy(j, i);
				printf("X");
			}
			else {
				Gotoxy(j, i);
				printf("  ");
			}
		}
	}
}

void PrintBlock() {
	for (int i = 0; i < 4; i++) {
		Gotoxy(block_x[i], block_y[i]);
		printf("O");
	}
}

void EraseBlock() {
	for (int i = 0; i < 4; i++) {
		Gotoxy(block_x[i], block_y[i]);
		printf("%c", 0);
	}
}

void PrintStatus() {
	/*
	Gotoxy(15, 0);
	printf("score : %4d", score*100);
	for (int i = 0; i < 4; i++) {
		Gotoxy(15, 2 * i + 1);
		printf("block_x[%d] : %3d", i, block_x[i]);
		Gotoxy(15, 2 * i + 2);
		printf("block_y[%d] : %3d", i, block_y[i]);
	}
	*/
	Gotoxy(15, 0);
	printf("# Score : %4d", score * 100);
}

/***************************************************************************/

void CreateBlock() {
	//initialization
	ChooseRandomBlock();
	int startplace_x = 5, startplace_y = 0;
	int startpoint_x = 0, startpoint_y = 0;
	int endpoint_x = 3, endpoint_y = 3;
	int isfound = 0;
	int count = 0;
	//find startpoint
	for (int i = 0; i < 4; i++) { //x
		for (int j = 0; j < 4; j++) {
			if (block[j][i] == 1) { startpoint_x = i; isfound = 1; break; }
		}
		if (isfound == 1) { break; }
	}
	isfound = 0;

	for (int i = 0; i < 4; i++) { //y
		for (int j = 0; j < 4; j++) {
			if (block[i][j] == 1) { startpoint_y = i; isfound = 1; break; }
		}
		if (isfound == 1) { break; }
	}
	isfound = 0;

	//find endpoint
	for (int i = 3; i >= 0; i--) { //x
		for (int j = 0; j < 4; j++) {
			if (block[j][i] == 1) { endpoint_x = i; isfound = 1; break; }
		}
		if (isfound == 1) { break; }
	}
	isfound = 0;

	for (int i = 3; i >= 0; i--) { //y
		for (int j = 0; j < 4; j++) {
			if (block[i][j] == 1) { endpoint_y = i; isfound = 1; break; }
		}
		if (isfound == 1) { break; }
	}

	//allocate position to each block
	for (int i = startpoint_y; i <= endpoint_y; i++) {
		for (int j = startpoint_x; j <= endpoint_x; j++) {
			if (block[i][j] == 1) {
				block_x[count] = j - startpoint_x + startplace_x;
				block_y[count] = i - startpoint_y + startplace_y;
				count++;
			}
		}
	}

	//calculate originalpoint
	originalpoint_x = block_x[0] - startpoint_x;
	originalpoint_y = block_y[0] - startpoint_y;
}

void ChooseRandomBlock() {
	srand(time(NULL));
	blocktype = rand() % 7;
	blockphase = rand() % 4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			block[i][j] = blockset[blocktype][blockphase][i][j];
		}
	}
}

void AddBlocktoMap() {
	for (int i = 0; i < 4; i++) {
		map[block_y[i]][block_x[i]] = 1;
		block_x[i] = 0;
		block_y[i] = 0;
	}
}

void BlockMoveDown() {
	for (int i = 0; i < 4; i++) {
		Gotoxy(block_x[i], block_y[i]);
		printf("%c", 0);
		block_y[i]++;
	}
	originalpoint_y++;
}

void BlockMoveLeft() {
	EraseBlock();
	for (int i = 0; i < 4; i++) {
		block_x[i]--;
	}
	originalpoint_x--;
}

void BlockMoveRight() {
	EraseBlock();
	for (int i = 0; i < 4; i++) {
		block_x[i]++;
	}
	originalpoint_x++;
}

void RotateBlock() {
	EraseBlock();
	blockphase++;
	blockphase = blockphase % 4;

	int count = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockset[blocktype][blockphase][i][j] == 1) {
				block_x[count] = j + originalpoint_x;
				block_y[count] = i + originalpoint_y;
				count++;
			}
		}
	}
}



/***************************************************************************/
int CheckBlockCollisionDown() {
	for (int i = 0; i < 4; i++) {
		if (map[block_y[i] + 1][block_x[i]] == 1) { //움직이는 블록의 하단부에 맵이 있는지 확인
			return 1;
		}
	}
	return 0;
}

int CheckBlockCollisionLeft() {
	for (int i = 0; i < 4; i++) {
		if (map[block_y[i]][block_x[i] - 1] == 1) { //왼쪽에 맵이 있는지 확인
			return 1;
		}
	}
	return 0;
}

int CheckBlockCollisionRight() {
	for (int i = 0; i < 4; i++) {
		if (map[block_y[i]][block_x[i] + 1] == 1) { //오른쪽에 맵이 있는지 확인
			return 1;
		}
	}
	return 0;
}

int CheckBlockCollisionRotate() { // 블록의 회전이 가능한지 확인하는 함수
	//init
	int count = 0;
	blockphase++;
	blockphase = blockphase % 4; //회전값을 1증가시킴(3에서 4가 되는 경우는 0으로 되돌림)

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockset[blocktype][blockphase][i][j] == 1) {
				tempblock_x[count] = j + originalpoint_x;
				tempblock_y[count] = i + originalpoint_y;
				count++;
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		if (map[tempblock_y[i]][tempblock_x[i]] == 1) {
			//만약 회전할 위치에 벽 또는 블럭이 있다면 다시 원래 위치와 회전으로 되돌림
			blockphase += 3;
			blockphase = blockphase % 4;

			return 1;
		}
	}
	blockphase += 3;
	blockphase = blockphase % 4;
	return 0;
}

void CheckGameover(){ 
	int i=1;
	for (int j = 1; j < 13; j++) { //위에서 첫번째 줄에 놓여진 블럭이 있는지 체크
			if (map[i][j] == 1) { 
				system("cls"); //놓인 블럭이 있다면 전부 지우고 게임오버 화면 표시
				printf("####################\n");
				Sleep(100);
				printf("#      G A M E     #\n");
				printf("#                  #\n");
				Sleep(100);
				printf("#         o        #\n");
				printf("#         V        #\n");
				Sleep(200);
				printf("#         E        #\n");
				Sleep(300);
				printf("#         R        #\n");
				Sleep(100);
				printf("#                  #\n");
				printf("#                  #\n");
				Sleep(100);
				printf("#                  #\n");
				printf("#                  #\n");
				Sleep(100);
				printf("#                  #\n");
				printf("#                  #\n");
				Sleep(100);
				printf("####################");
				Sleep(1000);
				exit(0);
				}
		}
}


/***************************************************************************/
void CheckLineClear() { //라인이 클리어 되었는지 확인하는 함수
	//init
	int blockcount;
	int k;
	memset(clearlineposition, -1, sizeof(clearlineposition));

	//아래줄부터 위쪽줄로 검사
	for (int i = 18; i >= 0; i--) {
		blockcount = 0;
		for (int j = 1; j < 13; j++) {
			if (map[i][j] == 1) { blockcount++; }
		}
		if (blockcount == 12) { //클리어된 라인의 인덱스값을 clearlineposition[]에 전달
			k = 0;
			while (clearlineposition[k] != -1) {
				k++;
			}
			clearlineposition[k] = i;
		}
	}
}

void LineClear() { //라인이 클리어 되었을 때 실행되는 함수
	for (int i = 0; i < 5; i++) {
		if (clearlineposition[i] == -1) { return; }
		RemoveLine(clearlineposition[i]);
		MoveLinesDownward(clearlineposition[i]);
		score += 900;
	}
}

void RemoveLine(int lineidx) { //라인을 지우는 함수
	for (int i = 1; i < 13; i++) {
		map[lineidx][i] = 0;
	}
}

void MoveLinesDownward(int lineidx) { //밑에 남은 라인이 있는지 확인하고 없으면 내리는 함수
	for (int i = lineidx - 1; i >= 0; i--) {
		for (int j = 1; j < 13; j++) {
			if (map[i][j] == 1) {
				map[i][j] = 0;
				map[i + 1][j] = 1;
			}
		}
	}
}

/***************************************************************************/

//Arrays

int map[20][14] = {
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

int copymap[20][14] = {
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2
};


int testblock[4][4] = {
	0,0,1,0,
	1,1,1,0,
	0,0,0,0,
	0,0,0,0
};

int blockset[7][4][4][4] = {
		0,0,0,0,
		1,1,1,1,
		0,0,0,0,
		0,0,0,0,

		0,0,1,0,
		0,0,1,0,
		0,0,1,0,
		0,0,1,0,

		0,0,0,0,
		0,0,0,0,
		1,1,1,1,
		0,0,0,0,

		0,1,0,0,
		0,1,0,0,
		0,1,0,0,
		0,1,0,0,

		1,0,0,0,
		1,1,1,0,
		0,0,0,0,
		0,0,0,0,

		0,1,1,0,
		0,1,0,0,
		0,1,0,0,
		0,0,0,0,

		0,0,0,0,
		1,1,1,0,
		0,0,1,0,
		0,0,0,0,

		0,1,0,0,
		0,1,0,0,
		1,1,0,0,
		0,0,0,0,

		0,0,1,0,
		1,1,1,0,
		0,0,0,0,
		0,0,0,0,

		0,1,0,0,
		0,1,0,0,
		0,1,1,0,
		0,0,0,0,

		0,0,0,0,
		1,1,1,0,
		1,0,0,0,
		0,0,0,0,

		1,1,0,0,
		0,1,0,0,
		0,1,0,0,
		0,0,0,0,


		0,1,1,0,
		0,1,1,0,
		0,0,0,0,
		0,0,0,0,

		0,1,1,0,
		0,1,1,0,
		0,0,0,0,
		0,0,0,0,

		0,1,1,0,
		0,1,1,0,
		0,0,0,0,
		0,0,0,0,

		0,1,1,0,
		0,1,1,0,
		0,0,0,0,
		0,0,0,0,

		0,1,1,0,
		1,1,0,0,
		0,0,0,0,
		0,0,0,0,

		0,1,0,0,
		0,1,1,0,
		0,0,1,0,
		0,0,0,0,

		0,0,0,0,
		0,1,1,0,
		1,1,0,0,
		0,0,0,0,

		1,0,0,0,
		1,1,0,0,
		0,1,0,0,
		0,0,0,0,

		0,1,0,0,
		1,1,1,0,
		0,0,0,0,
		0,0,0,0,

		0,1,0,0,
		0,1,1,0,
		0,1,0,0,
		0,0,0,0,

		0,0,0,0,
		1,1,1,0,
		0,1,0,0,
		0,0,0,0,

		0,1,0,0,
		1,1,0,0,
		0,1,0,0,
		0,0,0,0,

		1,1,0,0,
		0,1,1,0,
		0,0,0,0,
		0,0,0,0,

		0,0,1,0,
		0,1,1,0,
		0,1,0,0,
		0,0,0,0,

		0,0,0,0,
		1,1,0,0,
		0,1,1,0,
		0,0,0,0,

		0,1,0,0,
		1,1,0,0,
		1,0,0,0,
		0,0,0,0
};