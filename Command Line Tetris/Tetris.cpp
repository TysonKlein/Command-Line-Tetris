#include <iostream>

#include <Windows.h>
#include <vector>

int playWidth = 12;
int playHeight = 18;

int screenHeight = 30;
int screenWidth = 120;

int indexAfterRotateion(int x, int y, int r)
{
	//r = 0 | i = yw + x
	//r = 1 | i = 12 + y - xw
	//r = 2 | i = 15 - x - yw
	//r = 3 | i = 3 - y + xw
	switch (r)
	{
	case 0:return y * 4 + x;
	case 1:return 12 + y - x * 4;
	case 2:return 15 - x - 4 * y;
	case 3:return 3 - y + 4 * x;
	}
	return 0;
}

bool doesPieceFit(std::wstring* tetromino, int tetrominoID, int r, unsigned char* field, int pX, int pY)
{
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++){
			int tI = indexAfterRotateion(x, y, r);
			int fI = (pY + y) * playWidth + pX + x;
				if (pX + x >= 0 && pY + y >= 0 && pX + x < playWidth && pY + y < playHeight)
				{
					if (tetromino[tetrominoID][tI] == L'|' && field[fI] != 0)
						return false;
				}
			}

	return true;
}

int main()
{
	//Holds the seven tetrominos in their base orientation
	std::wstring tetromino[7];

	unsigned char *playField = nullptr;
	
	//Construct the assets
	tetromino[0].append(L"..|.");
	tetromino[0].append(L"..|.");
	tetromino[0].append(L"..|.");
	tetromino[0].append(L"..|.");

	tetromino[1].append(L"..|.");
	tetromino[1].append(L"..|.");
	tetromino[1].append(L".||.");
	tetromino[1].append(L"....");

	tetromino[2].append(L".|..");
	tetromino[2].append(L".|..");
	tetromino[2].append(L".||.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".||.");
	tetromino[3].append(L".||.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..|.");
	tetromino[4].append(L".||.");
	tetromino[4].append(L".|..");
	tetromino[4].append(L"....");

	tetromino[5].append(L".|..");
	tetromino[5].append(L".||.");
	tetromino[5].append(L"..|.");
	tetromino[5].append(L"....");

	tetromino[6].append(L"..|.");
	tetromino[6].append(L".||.");
	tetromino[6].append(L"..|.");
	tetromino[6].append(L"....");

	std::vector<int> lines;

	//Construct the play field
	playField = new unsigned char[playHeight * playWidth];

	//Set to 0 (empty) unless on the sides or on the bottom
	for (int y = 0; y < playHeight; y++)
		for (int x = 0; x < playWidth; x++)
			playField[y * playWidth + x] = (y == playHeight - 1 || x == 0 || x == playWidth - 1) ? 9 : 0;

	//Create the rendering environment (command prompt)

	wchar_t* screen = new wchar_t[screenWidth * screenHeight];
	for (int i = 0; i < screenWidth * screenHeight; i++) screen[i] = L' ';
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD dwBytesWritten = 0;

	//LOOP
	bool exit = false;
	int cPiece = 0;
	int cR = 0;
	int cX = playWidth / 2;
	int cY = 0;
	int cTick = 0;
	int Speed = 20;
	int piecesDelivered = 0;
	int Score = 0;

	bool forceDown = false;

	bool KEY[4];
	bool rotateLatch = false;

	while (!exit)
	{
		//Game Timing ===============================
		Sleep(50);

		//Input =====================================
		for (int i = 0; i < 4; i++)
			KEY[i] = (0x8000 & GetAsyncKeyState((unsigned char)("DASW"[i]))) != 0;

		//Logic =====================================
		cX += (KEY[0] && doesPieceFit(tetromino, cPiece, cR, playField, cX + 1, cY)) ? 1 : 0;
		cX -= (KEY[1] && doesPieceFit(tetromino, cPiece, cR, playField, cX - 1, cY)) ? 1 : 0;
		cY += (KEY[2] && doesPieceFit(tetromino, cPiece, cR, playField, cX, cY + 1)) ? 1 : 0;
		//cR = (!rotateLatch && KEY[3] && doesPieceFit(tetromino, cPiece, (cR + 1) % 4, playField, cX, cY)) ? (cR + 1) % 4 : cR;
		if (KEY[3] && !rotateLatch)
		{
			if (doesPieceFit(tetromino, cPiece, (cR + 1) % 4, playField, cX, cY)) { cR = (cR + 1) % 4; cTick--; }
			else if (cY > 0 && doesPieceFit(tetromino, cPiece, (cR + 1) % 4, playField, cX, cY - 1)) { cR = (cR + 1) % 4; cY = cY - 1; cTick--;}
			else if (cX > 0 && doesPieceFit(tetromino, cPiece, (cR + 1) % 4, playField, cX - 1, cY)) { cR = (cR + 1) % 4; cX = cX - 1; cTick--; }
			else if (cX < playWidth - 1 && doesPieceFit(tetromino, cPiece, (cR + 1) % 4, playField, cX + 1, cY)) { cR = (cR + 1) % 4; cX = cX + 1; cTick--; }
			else { forceDown = true; }
		}
		rotateLatch = KEY[3];

		cTick++;

		if (cTick == Speed)
		{
			cTick = 0;
			if (doesPieceFit(tetromino, cPiece, cR, playField, cX, cY+1))
			{
				//Move one line down if it can
				cY++;
			}
			else if(!KEY[3] || forceDown)
			{
				//Lock the piece into place
				for (int y = 0; y < 4; y++)
					for (int x = 0; x < 4; x++)
						if (tetromino[cPiece][indexAfterRotateion(x, y, cR)] == L'|')
							playField[(cY + y) * playWidth + (cX + x)] = cPiece+1;

				//Check for lines
				for (int y = cY; y < cY + 4; y++)
				{
					if (y < playHeight - 1) {
						bool line = true;
						for (int x = 1; x < playWidth - 1; x++)
						{
							line &= (playField[y * playWidth + x]) != 0;
						}

						if (line)
						{
							lines.push_back(y);
							for (int x = 1; x < playWidth - 1; x++)
								playField[y * playWidth + x] = 8;
						}
					}
				}
				Score += 25;
				if (!lines.empty()) Score += lines.size() * lines.size() * 100;

				//Give the player a new piece
				cX = playWidth / 2;
				cY = 0;
				cR = 0;
				cPiece = rand() % 7;
				piecesDelivered++;

				if (piecesDelivered >(21 - Speed) * 10 && Speed > 1)
				{
					Speed--;
				}

				//If new piece can't exist, game over
				if (!doesPieceFit(tetromino, cPiece, cR, playField, cX, cY))
				{
					exit = true;
				}
			}
		}
			
		//Display ===================================

			//Draw the play area to the screen
			for (int y = 0; y < playHeight; y++)
				for (int x = 0; x < playWidth; x++)
					screen[(y + 2) * screenWidth + (x + 2)] = L" ABCDEFG=#"[playField[y * playWidth + x]];

			//Draw the current tetromino to the screen
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
					if (tetromino[cPiece][indexAfterRotateion(x, y, cR)] == L'|') 
						screen[(cY + y + 2) * screenWidth + (cX + x + 2)] = L" ABCDEFG=#"[cPiece+1];

			swprintf_s(&screen[2 * screenWidth + playWidth + 6], 16, L"SCORE: %8d", Score);


			//If there is a line, do some work
			if (!lines.empty())
			{
				WriteConsoleOutputCharacter(console, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
				Sleep(400);

				for (auto &v : lines)
				{
					for (int x = 1; x < playWidth - 1; x++)
					{
						for (int y = v; y > 0; y--)
						{
							playField[(y)*playWidth + x] = playField[(y - 1) * playWidth + x];
						}
						playField[x] = 0;
					}
				}

				lines.clear();
			}

			//Display Frame
			WriteConsoleOutputCharacter(console, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
	}

	CloseHandle(console);
	std::cout << "Game Over. Score: " << Score << std::endl;
	system("pause");

	return 0;
}