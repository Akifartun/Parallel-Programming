#include"icb_gui.h"
#include"time.h"
#include <Windows.h>
#include <stdio.h>

int FRM;
int FRM2;
int FRM3;
int keypressed;

ICBYTES space;
ICBYTES start;
ICBYTES start_button;
ICBYTES gameover;
ICBYTES exp;
ICBYTES ast;
ICBYTES bllt;
ICBYTES spaceship;
ICBYTES bullet;
ICBYTES back_ast;

ICBYTES asteroid[3];
ICBYTES explosion[7];

ICBYTES back_text1;
ICBYTES back_text2;

ICBYTES coordinat_ast{ {8, 5, 77, 71}, {88, 7, 56, 49}, {148, 6, 36, 30} };
ICBYTES coordinat_exp{ {26, 20, 55, 73}, {100, 20, 78, 73}, {183, 20, 78, 73}, {275, 20, 78, 73}, {367, 20, 78, 73}, {458, 20, 78, 73}, {544, 20, 78, 71} };

HANDLE asteroid_mutex;

void Info();
void GameStart();
void GameProcess();
void GameOver();

int shipX = 3;
int shipY = 650;
int asteroidX;
int asteroidY;

int xcor, ycor;
int left;
int state_asteroid;
int coord_asteroid;
int finish = 1;

int score = 0;
int health = 10;

char a[10] = "SCORE: ";
char combined[20];
char c[15] = "HEALTH: ";
char combined3[30];

void ICGUI_Create()
{
	ICG_MWSize(850, 860);
	ICG_MWTitle("ASTEROID GAME");
}

void* makeExplosion(PVOID lpParam) // AKÝF ARTUN 190101018 - HATÝCE KÜBRA DURAN 190101001
{
	ICBYTES back;
	ICBYTES first;
	PlaySound("./files/Crash_Spaceship.wav", NULL, SND_ASYNC);
	Copy(space, xcor, ycor, 150, 150, first);
	for (int i = 1; i <= coordinat_exp.Y(); i++)
	{
		Copy(space, xcor, ycor, 150, 150, back);
		Copy(exp, coordinat_exp.I(1, i), coordinat_exp.I(2, i), coordinat_exp.I(3, i), coordinat_exp.I(4, i), explosion[i - 1]);  // LOAD EXPLOSION STATUS
		PasteNon0(explosion[i - 1], xcor, ycor, space);
		//DisplayImage(FRM, space);
#ifdef _DEBUG
		Sleep(15);
#else
		Sleep(20); 
#endif
		Paste(back, xcor, ycor, space);
	}
	Paste(first, xcor, ycor, space);
	DisplayImage(FRM, space);

	return(NULL);
}

void* fallAsteroid(PVOID lpParam) // AKÝF ARTUN 190101018 - HATÝCE KÜBRA DURAN 190101001
{
	finish = 1;
	DWORD dw;
	time_t t1;
	DWORD dwWaitResult;

	srand((unsigned)time(&t1));

	asteroidX = 5 + (rand() % 720);	
	asteroidY = 15;
	state_asteroid = rand() % 3;
	coord_asteroid = state_asteroid + 1;

	if (asteroidX <= 400) {
		left = 0;
	}
	else if (asteroidX > 400) {
		left = 1;
	}
	while (1) {
		dwWaitResult = WaitForSingleObject(asteroid_mutex, INFINITE);
		if (finish == 0) break;
		if (asteroidY >= 700) {
			asteroidX = 5 + (rand() % 720);	//78
			asteroidY = 15;
			state_asteroid = rand() % 3;
			coord_asteroid = state_asteroid + 1;
		}
		Copy(space, asteroidX, asteroidY, coordinat_ast.I(3, coord_asteroid) + 1, coordinat_ast.I(4, coord_asteroid) + 1, back_ast);
		Copy(ast, coordinat_ast.I(1, coord_asteroid), coordinat_ast.I(2, coord_asteroid), coordinat_ast.I(3, coord_asteroid), coordinat_ast.I(4, coord_asteroid), asteroid[state_asteroid]);
		PasteNon0(asteroid[state_asteroid], asteroidX, asteroidY, space);
		DisplayImage(FRM, space);
		Sleep(10);
		Paste(back_ast, asteroidX, asteroidY, space);
		if (left == 0) {
			asteroidX += 1;
			asteroidY += 2;
		}
		else if (left == 1) {
			asteroidX -= 1;
			asteroidY += 2;
		}

		ReleaseMutex(asteroid_mutex);
	}

	return(NULL);
}
void* shoot(PVOID lpParam) // AKÝF ARTUN 190101018 - HATÝCE KÜBRA DURAN 190101001
{
	PlaySound("./files/Laser_Gun.wav", NULL, SND_ASYNC);
	time_t t1;
	DWORD dw;
	DWORD dwWaitResult;
	ICBYTES back_bull1, back_bull2, back_explosion;

	srand((unsigned)time(&t1));

	int bulletX1 = shipX + 8;
	int bulletY1 = shipY - 25;
	int bulletX2 = shipX + 44;
	int bulletY2 = shipY - 25;

	int width_1 = coordinat_ast.I(3, coord_asteroid);
	int height_1 = coordinat_ast.I(4, coord_asteroid);
	int height_2 = coordinat_ast.I(4, coord_asteroid) - 5;

	while (bulletY1 > 3 && bulletY2 > 3) {
		if ((bulletX1 < asteroidX + width_1 && asteroidX <= bulletX1 && asteroidY + height_1 >= bulletY1 && asteroidY + height_2 < bulletY1 && finish == 1) || (bulletX2 < asteroidX + width_1 && asteroidX <= bulletX2 && asteroidY + height_1 >= bulletY2 && asteroidY + height_2 < bulletY2 && finish == 1)) {
			dwWaitResult = WaitForSingleObject(asteroid_mutex, INFINITE);
			Sleep(50);

			xcor = asteroidX;
			ycor = asteroidY;

			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)makeExplosion, NULL, 0, &dw);

			score += 1;
			Paste(back_text2, 10, 10, space);
			sprintf_s(combined, sizeof(a), "%s%d", a, score);
			Impress12x20(space, 10, 10, combined, 0xffffff);
			Sleep(400);
			if (state_asteroid < 2) {
				state_asteroid += 1;
				coord_asteroid = state_asteroid + 1;
			}
			else if (state_asteroid == 2) {
				finish = 0;
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fallAsteroid, NULL, 0, &dw);
			}

			ReleaseMutex(asteroid_mutex);

			if (asteroidX <= 400) {
				left = 0;
			}
			else if (asteroidX > 400) {
				left = 1;
			}
			break;
		}
		Copy(space, bulletX1, bulletY1, 13, 32, back_bull1);
		Copy(space, bulletX2, bulletY2, 13, 32, back_bull2);
		Copy(bullet, 15, 5, 13, 32, bllt);
		PasteNon0(bllt, bulletX1, bulletY1, space);
		PasteNon0(bllt, bulletX2, bulletY2, space);
		//DisplayImage(FRM, space);
		Sleep(15);
		Paste(back_bull1, bulletX1, bulletY1, space);
		Paste(back_bull2, bulletX2, bulletY2, space);
		bulletY1 -= 2;
		bulletY2 -= 2;

	}

	return(NULL);
}

void* spaceship_movement(PVOID lpParam) // AKÝF ARTUN 190101018 - HATÝCE KÜBRA DURAN 190101001
{
	DWORD dw;
	DWORD dwWaitResult;
	ICBYTES back_ship;
	ICBYTES back_last;
	state_asteroid = rand() % 3;
	coord_asteroid = state_asteroid + 1;

	Copy(space, 650, 10, 130, 20, back_text1);
	Copy(space, 200, 10, 150, 20, back_text2);


	sprintf_s(combined, sizeof(a), "%s%d", a, score);
	sprintf_s(combined3, sizeof(c), "%s%d", c, health);
	Impress12x20(space, 10, 10, combined, 0xffffff);
	Impress12x20(space, 650, 10, combined3, 0xffffff);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fallAsteroid, NULL, 0, &dw);

	while (1) {
		int width = coordinat_ast.I(3, coord_asteroid);
		int height = coordinat_ast.I(4, coord_asteroid);
		if (asteroidX + width >= shipX && asteroidX <= shipX + 62 &&
			asteroidY + height >= shipY && asteroidY <= shipY + 73) {
			xcor = shipX;
			ycor = shipY;

			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)makeExplosion, NULL, 0, &dw);
			if (state_asteroid == 0) {
				health -= 5;
			}
			else if (state_asteroid == 1) {
				health -= 3;
			}
			else if (state_asteroid == 2) {
				health -= 1;
			}
			if (health <= 0) {
				Sleep(500);
				GameOver();
			}
			Paste(back_text1, 650, 10, space);
			sprintf_s(combined3, sizeof(c), "%s%d", c, health);
			Impress12x20(space, 650, 10, combined3, 0xffffff);
			Sleep(1000);
		}
		Copy(space, shipX, shipY, 63, 74, back_ship);
		PasteNon0(spaceship, shipX, shipY, space);
		DisplayImage(FRM, space);
		Sleep(10);
		Paste(back_ship, shipX, shipY, space);
		if (keypressed == 32) {
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)shoot, NULL, 0, &dw);
			keypressed = 0;
		}
		if (keypressed == 37) shipX -= 2;
		if (keypressed == 39) shipX += 2;
	}

	return(NULL);
}

void WhenKeyPressed(int k)
{
	keypressed = k;
}

void Info() // AKÝF ARTUN 190101018 
{
	MessageBox(ICG_GetMainWindow(), "ASTEROID OYUNUNA HOSGELDIN!!!\nTOPLAMDA 10 ADET CANA SAHIPSIN!!!\nBUYUK ASTEROIDE CARPARSAN 5 CAN,\nORTA ASTEROIDE CARPARSAN 3 CAN VE\nKUCUK ASTEROIDE CARPARSAN 1 CAN GIDER!!!\nVURDUGUN HER ASTEROID ICIN +1 SKOR KAZANIRSIN.\nBOL SANS.", "BILGILENDIRME", MB_OK);
	GameProcess();
}

void GameStart() { // HATÝCE KÜBRA DURAN 190101001
	PlaySound("./files/intro.wav", NULL, SND_ASYNC);
	int BTN1;
	FRM2 = ICG_FrameSunken(5, 5, 800, 800);
	ReadImage("./background/Space Background (1).jpg", start);
	BTN1 = ICG_BitmapButton(300, 600, 250, 112, Info);
	ReadImage("./files/start.bmp", start_button);
	SetButtonBitmap(BTN1, start_button);
	DisplayImage(FRM2, start);
}

void GameProcess() { // AKÝF ARTUN 190101018 
	DWORD dw;
	srand(time(NULL));

	asteroid_mutex = CreateMutex(NULL, FALSE, NULL);

	FRM = ICG_FrameSunken(5, 5, 800, 800);
	ReadImage("./background/Space Background (6).jpg", space);
	ReadImage("./files/explosion_reduced.bmp", exp);
	ReadImage("./files/new1.bmp", ast);
	ReadImage("./files/spaceship2-3.bmp", spaceship);
	ReadImage("./files/bullet1.bmp", bullet);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)spaceship_movement, NULL, 0, &dw);

	ICG_SetOnKeyPressed(WhenKeyPressed);
	SetFocus(ICG_GetMainWindow());
	DisplayImage(FRM, space);
}

void GameOver() { // HATÝCE KÜBRA DURAN 190101001
	FRM3 = ICG_FrameSunken(5, 5, 800, 800);
	ReadImage("./files/gameover.bmp", space);
	DisplayImage(FRM3, space);
	Sleep(2000);
	exit(0);
}

void ICGUI_main()
{
	GameStart();
}