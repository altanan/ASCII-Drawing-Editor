/*
* This program allows display and editing of text art (also called ASCII art).
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;

const char* SAVE_PATH = "SavedFiles/";

int main()
{
	// system("cls");

	Node* current;
	current = newCanvas();

	List undoList;
	List redoList;
	List clips;

	// char array to hold file name entered by user
	char fileName[FILENAMESIZE];
	// char array to hold the full filepath
	char filePath[FILENAMESIZE];

	//init filename with spaces
	for (int x = 0; x < FILENAMESIZE; x++)
	{
		fileName[x] = ' ';
	}

	// char canvas[MAXROWS][MAXCOLS];
	// char backup[MAXROWS][MAXCOLS];

	// bool variable to hold animation state
	bool animate = false;

	// initialize the canvas with spaces before its displayed
	// unnecessary now initCanvas(canvas);

	// variable to hold user input from menu
	char choice = 'Z';
	// variables to hold user input from replace function
	char oldCh, newCh;
	// variables to hold user input from move function
	int movex, movey;

	do
	{
		// display the canvas and menu
		displayCanvas(current->item);

		char menuText1[MAXCOLS] = { "<A>nimate: N / <U>ndo: 0 / Cl<I>p: 0 / <P>lay\n" };

		if (undoList.count >= 0)
		{
			snprintf(menuText1, MAXCOLS, "<A>nimate: N / <U>ndo: %d / Cl<I>p: %d / <P>lay\n", undoList.count, clips.count);

			if (redoList.count > 0)
			{
				snprintf(menuText1, MAXCOLS, "<A>nimate: N / <U>ndo: %d / Red<O>: %d / Cl<I>p: %d / <P>lay\n", undoList.count, redoList.count, clips.count);
			}
		}

		menuText1[11] = animate ? 'Y' : 'N';

		printf("%s", menuText1);

		cout << "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <L>oad / <S>ave / <Q>uit: ";

		cin.clear();
		cin >> choice;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
		}

		// convert lowercase input to uppercase
		choice = toupper(choice);

		cin.ignore((numeric_limits<streamsize>::max)(), '\n');

		// clear user input if menu option not chosen
		gotoxy(23, 83);
		cout << "                 ";
		gotoxy(23, 83);


		switch (choice)
		{
			// Edit menu option
		case 'E':

			// backup canvas and enter edit mode
			// copyCanvas(backup, current->item);
			addUndoState(undoList, redoList, current);

			editCanvas(current->item);

			break;

			// Move menu option
		case 'M':

			// backup canvas and clear menu
			// copyCanvas(backup, current->item);
			addUndoState(undoList, redoList, current);

			clearLine(MAXROWS + 1, 90);
			clearLine(MAXROWS + 2, 90);
			gotoxy(MAXROWS + 1, 0);

			// get user input
			cout << "Enter column units to move: ";
			cin >> movey;
			cout << "Enter row units to move: ";
			cin >> movex;

			clearLine(MAXROWS + 2, 90);

			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			moveCanvas(current->item, movex, movey);

			break;

			// Replace menu option
		case 'R':

			// backup canvas and clear menu
			// copyCanvas(backup, current->item);
			addUndoState(undoList, redoList, current);

			clearLine(MAXROWS + 1, 90);
			clearLine(MAXROWS + 2, 90);
			gotoxy(MAXROWS + 1, 0);

			// get user input
			cout << "Enter character to replace: ";
			oldCh = cin.get();


			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}


			// account for blank spaces being entered by user
			while (cin.get() != '\n');
			{
				if (oldCh == '\n')
				{
					oldCh = ' ';
				}
			}

			cout << "Enter character to replace with: ";
			newCh = cin.get();

			clearLine(MAXROWS + 2, 90);


			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}


			while (cin.get() != '\n');

			if (newCh == '\n')
			{
				newCh = ' ';
			}


			replace(current->item, oldCh, newCh);

			break;

			// Undo menu option
		case 'U':

			// backup canvas and display previous canvas
			// copyCanvas(current->item, backup);

			restore(undoList, redoList, current);

			// copyCanvas(current->item, undoList.head->item);
			// removeNode(undoList);

			// addUndoState(undoList, redoList, current);
			// copyCanvas(undoList.head->item, current->item);

			break;

			// Load menu option
		case 'L':

			char option;

			// backup canvas and replace all old values with spaces 
			// so that previous characters are not left behind, clear menu
			// copyCanvas(backup, current->item);
			// addUndoState(undoList, redoList, current);

			clearLine(MAXROWS + 1, 90);
			clearLine(MAXROWS + 2, 90);
			gotoxy(MAXROWS + 1, 0);

			cout << "<C>anvas or <A>nimation ? ";
			cin >> option;

			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			option = toupper(option);

			switch (option)
			{
			case 'A':

				clearLine(MAXROWS + 1, 90);
				gotoxy(MAXROWS + 1, 0);

				// user input file name
				cout << "Enter the file name (don't enter 'txt'): ";
				cin.getline(fileName, FILENAMESIZE - 15);

				if (loadClips(clips, fileName) == FALSE)
				{
					cerr << "ERROR: File can not be read.\n";
					system("pause");

					clearLine(MAXROWS + 2, 90);
					clearLine(MAXROWS + 3, 90);
				}

				option = '\n';

				break;

			case 'C':

				clearLine(MAXROWS + 1, 90);
				gotoxy(MAXROWS + 1, 0);

				// user input file name
				cout << "Enter the file name (don't enter 'txt'): ";
				cin.getline(fileName, FILENAMESIZE - 15);

				// int variable to hold length of str entered by user for file name
				int len = strlen(fileName);

				// attach .txt to the end of the file name entered by user
				snprintf(fileName + len, FILENAMESIZE - len, ".txt");
				// attach the file path to the beginning of the file name entered by user
				snprintf(filePath, FILENAMESIZE, "SavedFiles\\%s", fileName);

				if (loadCanvas(current->item, filePath) == FALSE)
				{
					cerr << "ERROR: File can not be read.\n";
					system("pause");

					clearLine(MAXROWS + 2, 90);
					clearLine(MAXROWS + 3, 90);
				}

				option = '\n';

				break;
			}


			break;

			// Save menu option
		case 'S':

			//clear menu
			clearLine(MAXROWS + 1, 90);
			clearLine(MAXROWS + 2, 90);
			gotoxy(MAXROWS + 1, 0);

			cout << "<C>anvas or <A>nimation ? ";
			cin >> option;

			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			option = toupper(option);

			switch (option)
			{
			case 'A':

				// clear option menu
				clearLine(MAXROWS + 1, 90);
				gotoxy(MAXROWS + 1, 0);

				// let user input file name
				cout << "Enter the file name (don't enter 'txt'): ";
				cin.getline(fileName, FILENAMESIZE - 15);

				if (cin.fail())
				{
					cin.clear();
					cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				}

				// attach the file path to the beginning of the file name entered by user
				snprintf(filePath, FILENAMESIZE, "SavedFiles\\%s", fileName);

				saveClips(clips, filePath);

				break;

			case 'C':

				// clear option menu
				clearLine(MAXROWS + 1, 90);
				gotoxy(MAXROWS + 1, 0);

				// let user input file name
				cout << "Enter the file name (don't enter 'txt'): ";
				cin.getline(fileName, FILENAMESIZE - 15);

				if (cin.fail())
				{
					cin.clear();
					cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				}

				int len = strlen(fileName);

				// attach .txt to the end of the file name entered by user
				snprintf(fileName + len, FILENAMESIZE - len, ".txt");
				// attach the file path to the beginning of the file name entered by user
				snprintf(filePath, FILENAMESIZE, "SavedFiles\\%s", fileName);

				if (saveCanvas(current->item, filePath) == FALSE)
				{
					cerr << "ERROR: File can not be written.\n";

					system("pause");

					clearLine(MAXROWS + 2, 90);
					clearLine(MAXROWS + 3, 90);
				}
				else
				{
					cout << "File saved!\n";
					system("pause");
					clearLine(MAXROWS + 2, 90);
					clearLine(MAXROWS + 3, 90);
				}

				break;
			}

			break;

		case 'C':

			addUndoState(undoList, redoList, current);

			//clear the canvas by initializing it with spaces
			initCanvas(current->item);

			break;

		case 'D':

			// display the secondary menu for draw options
			menuTwo(current, undoList, redoList, clips, animate);

			break;

		case 'A':

			animate = !animate;

			break;

		case 'O':

			restore(redoList, undoList, current);

			break;

		case 'I':

			addNode(clips, newCanvas());
			copyCanvas(clips.head->item, current->item);

			break;

		case 'P':

			play(clips);

			break;

		}

		clearLine(MAXROWS + 1, 90);
		clearLine(MAXROWS + 2, 90);
		gotoxy(MAXROWS + 1, 0);

	} while (choice != 'Q');
	// clear menu when user quits
	/*
	gotoxy(23, 83);
	cout << "                 ";
	*/
	gotoxy(MAXROWS, MAXCOLS + 1);

	deleteList(undoList);
	deleteList(redoList);
	deleteList(clips);

	delete current;

	return 0;
}


void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
//msdn.microsoft.com and look up SetConsoleCursorPosition if you want to understand how this function works


void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}


void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	// for loops to check each array position to see if they are equivalent to oldCh
	// if true, replace that position with newCh
	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			if (canvas[x][y] == oldCh)
			{
				canvas[x][y] = newCh;
			}
		}
	}
}


void editCanvas(char canvas[][MAXCOLS])
{
	// variables to hold user input and keep track of cursor position
	char input;
	int row = 0, col = 0;

	// clear menu, display edit mode, move cursor
	clearLine(MAXROWS + 1, 90);
	clearLine(MAXROWS + 2, 90);
	gotoxy(MAXROWS + 1, 0);
	cout << "Press <ESC> to stop editing";
	gotoxy(row, col);

	// takes user input during edit mode
	do
	{
		input = _getch();

		// first check if input is = SPECIAL
		if (input == SPECIAL)
		{
			// get input second char and check if that = one of the arrow keys, move cursor
			input = _getch();

			if (input == LEFTARROW && col > 0)
			{
				--col;
				gotoxy(row, col);
			}
			else if (input == UPARROW && row > 0)
			{
				--row;
				gotoxy(row, col);
			}
			else if (input == RIGHTARROW && col < MAXCOLS - 1)
			{
				++col;
				gotoxy(row, col);
			}
			else if (input == DOWNARROW && row < MAXROWS - 1)
			{
				++row;
				gotoxy(row, col);
			}
		}

		// account for f1-12 inputs, do nothing with them
		else if (input == '\0')
		{
			input = _getch();
		}

		// ensure character input is valid
		else if (input >= 32 && input <= 126)
		{
			// alter contents of canvas with ascii input
			canvas[row][col] = input;
			// display input to user
			printf("%c", canvas[row][col]);
			// return cursor to position
			gotoxy(row, col);
		}

	} while (input != ESC);
}

// easiest way is to use a temporary second array, make a copy into second canvas, then copy everything in 2nd canvas to the original canvas in their new position
// or copy and move them into new positions, then copy back to original
void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	// array to hold copy of canvas when moved
	char temp[MAXROWS][MAXCOLS];

	// initialize temp with all spaces
	initCanvas(temp);

	// copy contents of canvas into temp at new positions
	for (int a = 0; a < MAXROWS; a++)
	{
		for (int b = 0; b < MAXCOLS; b++)
		{

			// ensure new positions are valid
			if (a - rowValue >= 0 && a - rowValue < MAXROWS && b - colValue >= 0 && b - colValue < MAXCOLS)
			{
				temp[a][b] = canvas[a - rowValue][b - colValue];
			}
		}
	}

	// copy contents of temp back into canvas
	copyCanvas(canvas, temp);
}


void initCanvas(char canvas[][MAXCOLS])
{
	// change each position in canvas to a space
	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			canvas[x][y] = ' ';
		}
	}
}


void displayCanvas(char canvas[][MAXCOLS])
{
	/*
	// Clear the screen
	for (int a = 0; a < MAXROWS; a++)
	{
		clearLine(a, MAXCOLS - 1);
	}
	*/

	// move cursor, display each value of canvas with 
	// pipe characters at the end of the columns
	gotoxy(0, 0);
	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			printf("%c", canvas[x][y]);
		}

		cout << "|" << endl;
	}

	// display line of dashes at the end of the rows
	for (int z = 0; z < MAXCOLS; z++)
	{
		printf("%s", "-");
	}
	cout << endl;
}


void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	// copy each array position of from[] to to[]
	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			to[x][y] = from[x][y];
		}
	}
}


bool saveCanvas(char canvas[][MAXCOLS], char filename[])
{
	// char array to hold the full filepath
	// char filePath[FILENAMESIZE];

	/*
	// let user input file name
	cout << "Enter the file name (don't enter 'txt'): ";
	cin.getline(fileName, FILENAMESIZE - 15);

	if (cin.fail())
	{
		cin.clear();
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	}
	*/

	//ensure file opens
	ofstream file(filename);
	if (!file.is_open())
	{
		/*
		cerr << "ERROR: File can not be written.\n";

		system("pause");

		clearLine(MAXROWS + 2, 90);
		clearLine(MAXROWS + 3, 90);
		*/

		return false;
	}

	// copy chars in canvas to the file, with \n at the end of the line
	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			file.put(canvas[row][col]);
		}

		file.put('\n');
	}

	// close file, let user know file was saved
	file.close();
	/*
	cout << "File saved!\n";
	system("pause");
	clearLine(MAXROWS + 2, 90);
	clearLine(MAXROWS + 3, 90);
	*/

	return true;
}


bool loadCanvas(char canvas[][MAXCOLS], char filename[])
{
	/*
	// char array to hold the full filepath
	char filePath[FILENAMESIZE];
	*/


	// int variable to hold length of str entered by user for file name
	int len = strlen(filename);


	/*
	// attach .txt to the end of the file name entered by user
	snprintf(filename + len, FILENAMESIZE - len, ".txt");
	// attach the file path to the beginning of the file name entered by user
	snprintf(filePath, FILENAMESIZE, "SavedFiles\\%s", filename);
	*/

	// attempt to open file, give error message if not
	ifstream file(filename);

	if (!file.is_open())
	{
		/*
		cerr << "ERROR: File can not be read.\n";
		system("pause");

		clearLine(MAXROWS + 2, 90);
		clearLine(MAXROWS + 3, 90);
		*/

		return false;
	}

	initCanvas(canvas);

	// otherwise copy each row of the file into line array, then line array into canvas
	// loop until end of rows or until reaching the end of the file
	// int variable to hold the current row being processed
	int row = 0;
	while (row < MAXROWS && file.good())
	{

		// char array to store each line of the file, get each line
		char line[MAXCOLS + 1];
		file.getline(line, MAXCOLS + 1);

		// check to see if file stream is in an error state, 
		// clear error state and ignore characters outside canvas
		while (file.fail() && !file.eof())
		{
			file.clear();
			file.ignore((numeric_limits<streamsize>::max)(), '\n');
		}

		// get the length of each line being put into canvas
		len = strlen(line);

		// place each character into canvas from line
		memcpy(canvas[row], line, len);

		row++;
	}

	file.close();

	return true;
}
