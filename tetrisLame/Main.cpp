/*
 * Thomas Victor Cole
 * --The Prograndma--
 *
 * Written through a tutorial on how to make tetris.
 * https://www.youtube.com/watch?v=8OK8_tHeCIA&t=1883s
 * My particular favorite part is the rotation function.
 * Interesting math to treat a one dimensional array like a two dimensional array!
 */


#include <iostream>
#include <Windows.h>
#include <vector>
#include <thread>


using namespace std;

wstring TETROMINO[7];
int FIELD_WIDTH = 12;
int FIELD_HEIGHT = 16;
unsigned char *PLAYING_FIELD = nullptr;


int SCREEN_WIDTH = 80;
int SCREEN_HEIGHT = 30;
// 0 = empty space
// 1 = part of a shape
// 2 = a different shape
// 3 = a different shape
// ...
// 9 = boundary

int rotate(int x, int y, int r);
bool doesPieceFit(int tetromino, int roation, int xPos, int yPox);

int main()
{

	// create assests

	// line
	int i = 0;
	TETROMINO[i].append(L"..X."); // treat it like a 2d array but use
	TETROMINO[i].append(L"..X."); // some facy pantsy math
	TETROMINO[i].append(L"..X."); // to figure out what the index j
	TETROMINO[i].append(L"..X."); // ought to be. 

	//z
	i++;
	TETROMINO[i].append(L"..X."); // tetromino[1] = that lightning shape
	TETROMINO[i].append(L".XX.");
	TETROMINO[i].append(L".X..");
	TETROMINO[i].append(L"....");

	//Box
	i++;
	TETROMINO[i].append(L"....");
	TETROMINO[i].append(L".XX.");
	TETROMINO[i].append(L".XX.");
	TETROMINO[i].append(L"....");

	//T
	i++;
	TETROMINO[i].append(L"..X.");
	TETROMINO[i].append(L".XX.");
	TETROMINO[i].append(L"..X.");
	TETROMINO[i].append(L"....");

	//s
	i++;
	TETROMINO[i].append(L".X..");
	TETROMINO[i].append(L".XX.");
	TETROMINO[i].append(L"..X.");
	TETROMINO[i].append(L"....");

	//L
	i++;
	TETROMINO[i].append(L"..X.");
	TETROMINO[i].append(L"..X.");
	TETROMINO[i].append(L".XX.");
	TETROMINO[i].append(L"....");

	//J
	i++;
	TETROMINO[i].append(L"..X.");
	TETROMINO[i].append(L"..X.");
	TETROMINO[i].append(L"..XX");
	TETROMINO[i].append(L"....");


	// this is what the PLAYING_FIELD is initialized to be. 
	// treat it like a 2d array but it's not, it's just a facy 1d array
	// everything is initialzed to 0 except the borders, they're 9
	PLAYING_FIELD = new unsigned char[FIELD_WIDTH*FIELD_HEIGHT];
	for (int x = 0; x < FIELD_WIDTH; x++)
	{
		for (int y = 0; y < FIELD_HEIGHT; y++)
		{
			PLAYING_FIELD[y*FIELD_WIDTH + x] = (x == 0 || x == FIELD_WIDTH - 1 || y == FIELD_HEIGHT - 1) ? 9 : 0;

		}
	}


	// Game vars

	bool key[4];
	int currentPiece = 0;
	int currentRotation = 0;
	int currentX = FIELD_WIDTH / 2; // start in middle of the top.
	int currentY = 0;
	int speed = 20;
	int speedCount = 0;
	bool forceDown = false;
	bool rotateHold = true;
	int pieceCount = 0;
	int playerScore = 0;
	vector<int> lines;
	bool gameOver = false;

	// so here is the screen
	wchar_t *screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) screen[i] = L' ';
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD dwBytesWritten = 0;


	while (!gameOver)
	{
		this_thread::sleep_for(50ms); // Small Step = 1 Game Tick
		speedCount++;
		forceDown = (speedCount == speed);

		// Iput 
		for (int k = 0; k < 4; k++)								// R   L   D Z
			key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		// Game Logic

		// Handle player movement
		currentX += (key[0] && doesPieceFit(currentPiece, currentRotation, currentX + 1, currentY)) ? 1 : 0;
		currentX -= (key[1] && doesPieceFit(currentPiece, currentRotation, currentX - 1, currentY)) ? 1 : 0;
		currentY += (key[2] && doesPieceFit(currentPiece, currentRotation, currentX, currentY + 1)) ? 1 : 0;

		// Rotate, but latch to stop wild spinning
		if (key[3])
		{
			currentRotation += (rotateHold && doesPieceFit(currentPiece, currentRotation + 1, currentX, currentY)) ? 1 : 0;
			rotateHold = false;
		}
		else
		{
			rotateHold = true;
		}

		// Force the piece down the playfield if it's time
		if (forceDown)
		{
			// Update difficulty every 50 pieces
			speedCount = 0;
			pieceCount++;
			if (pieceCount % 50 == 0)
			{
				if (speed >= 10) 
				{
					speed--;
				}
			}

			// Test if piece can be moved down
			if (doesPieceFit(currentPiece, currentRotation, currentX, currentY + 1))
			{
				currentY++; // It can, so do it!
			}
			else
			{
				// It can't! Lock the piece in place
				for (int x = 0; x < 4; x++)
				{
					for (int y = 0; y < 4; y++)
					{
						if (TETROMINO[currentPiece][rotate(x, y, currentRotation)] != L'.')
						{
							PLAYING_FIELD[(currentY + y) * FIELD_WIDTH + (currentX + x)] = currentPiece + 1;
						}
					}
				}

				// Check for lines
				for (int y = 0; y < 4; y++)
					if (currentY + y < FIELD_HEIGHT - 1)
					{
						bool bLine = true;
						for (int x = 1; x < FIELD_WIDTH - 1; x++)
							bLine &= (PLAYING_FIELD[(currentY + y) * FIELD_WIDTH + x]) != 0;

						if (bLine)
						{
							// Remove Line, set to =
							for (int x = 1; x < FIELD_WIDTH - 1; x++)
								PLAYING_FIELD[(currentY + y) * FIELD_WIDTH + x] = 8;
							lines.push_back(currentY + y);
						}
					}

				playerScore += 25;
				if (!lines.empty())
				{
					playerScore += (1 << lines.size()) * 100;
				}

				// Pick New Piece
				currentX = FIELD_WIDTH / 2;
				currentY = 0;
				currentRotation = 0;
				currentPiece = rand() % 7;

				// If piece does not fit straight away, game over!
				gameOver = !doesPieceFit(currentPiece, currentRotation, currentX, currentY);
			}
		}

		// Display ======================

		// Draw Field
		for (int x = 0; x < FIELD_WIDTH; x++)
		{
			for (int y = 0; y < FIELD_HEIGHT; y++)
			{
				screen[(y + 2)*SCREEN_WIDTH + (x + 2)] = L" ABCDEFG=#"[PLAYING_FIELD[y*FIELD_WIDTH + x]];
			}
		}

		// Draw Current Piece
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				if (TETROMINO[currentPiece][rotate(x, y, currentRotation)] != L'.')
				{
					screen[(currentY + y + 2)*SCREEN_WIDTH + (currentX + x + 2)] = currentPiece + 65;
				}
			}
		}

		// Draw Score
		swprintf_s(&screen[2 * SCREEN_WIDTH + FIELD_WIDTH + 6], 16, L"SCORE: %8d", playerScore);

		// Animate Line Completion
		if (!lines.empty())
		{
			// Display Frame (cheekily to draw lines)
			WriteConsoleOutputCharacter(console, (LPCTSTR) screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); // Delay a bit

			for (auto &line : lines)
				for (int x = 1; x < FIELD_WIDTH - 1; x++)
				{
					for (int y = line; y > 0; y--)
						PLAYING_FIELD[y * FIELD_WIDTH + x] = PLAYING_FIELD[(y - 1) * FIELD_WIDTH + x];
					PLAYING_FIELD[x] = 0;
				}

			lines.clear();
		}

		// Display Frame
		WriteConsoleOutputCharacter(console, (LPCTSTR)screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwBytesWritten);
	}

	// Oh Dear
	CloseHandle(console);
	cout << "Game Over!! Score:" << playerScore << endl;
	system("pause");
	return 0;

}


int rotate(int x, int y, int r)
{
	// normal 
		// i = y*w + x
		// ex. 10= 2 * 4 + 2
	// a clockwise rotation
		// i = 12 + y - (x * 4)
	// a reflection
		// i = 15 - (y * 4) - x
	// a counter-clockwise rotation
		// i = 3 + y(x * 4)
	 
	switch (r % 4)
	{
		case 0: return y * 4 + x;
		case 1: return 12 + y - (x * 4);
		case 2: return 15 - (y * 4) - x;
		case 3: return 3 - y + (x * 4);
	}
	return 0;
}

bool doesPieceFit(int tetromino, int rotation, int xPos, int yPos)
{
	//iff cell > 0 -> cell is occupied
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			
			int index = rotate(x, y, rotation);

			int field = (yPos + y) * FIELD_WIDTH + (xPos + x);



			if (xPos + x >= 0 && xPos + x < FIELD_WIDTH)
			{
				if (yPos + y >= 0 && yPos + y < FIELD_HEIGHT) // then it's in bounds
				{

					if (TETROMINO[tetromino][index] != L'.' && PLAYING_FIELD[field] != 0)
					{
						return false; // first hit return false!
					}
				}
			}

		}
	}


	// if it never fails... it fits!

	return true;
}