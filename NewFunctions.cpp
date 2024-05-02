#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate)
{
	// char variable to hold user input inside menuTwo
	char option;

	// char array to hold draw menu text
	// char menuText1[MAXCOLS] = { "<A>nimate: N / <F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu:" };
	char menuText1[MAXCOLS] = { "<A>nimate: N / <U>ndo: 0 / Red<O>: 0 / Cl<I>p: 0 / <P>lay\n" };
	char menuText2[MAXCOLS] = { "<F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: " };

	Point pt, startPoint, endPoint;
	pt.row = 0, pt.col = 0;

	int boxHeight, treeHeight, treeBranchAngle, treeTrunkAngle = 270;

	do
	{
		// erase main menu
		clearLine(MAXROWS + 1, 90);

		menuText1[11] = animate ? 'Y' : 'N';

		// alter values for undo, redo, and clip
		if (undoList.count >= 0)
		{
			snprintf(menuText1, MAXCOLS, "<A>nimate: N / <U>ndo: %d / Cl<I>p: %d / <P>lay\n", undoList.count, clips.count);

			if (redoList.count > 0)
			{
				snprintf(menuText1, MAXCOLS, "<A>nimate: N / <U>ndo: %d / Red<O>: %d Cl<I>p: %d / <P>lay\n", undoList.count, redoList.count, clips.count);
			}
		}

		// display draw menu
		printf("%s", menuText1);
		printf("%s", menuText2);

		cout << "                        ";
		gotoxy(MAXROWS + 2, 65);

		cin >> option;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
		}

		option = toupper(option);

		switch (option)
		{
			// animate menuTwo option
		case 'A':

			animate = !animate;

			break;

		case 'L':

			// clear menu and display start line mode
			clearLine(MAXROWS + 1, 90);
			clearLine(MAXROWS + 2, 90);
			gotoxy(MAXROWS + 1, 0);
			cout << "Type any letter to choose start point / ESC to cancel";

			// if user enters char, update startPoint
			if (getPoint(pt) != ESC)
			{
				startPoint.row = pt.row, startPoint.col = pt.col;

				// clear and display end line mode
				clearLine(MAXROWS + 1, 90);
				cout << "Type any letter to choose end point / ESC to cancel";

				// if user enter char, update endPoint
				if (getPoint(pt) != ESC)
				{
					//copyCanvas(backupCanvas, canvas);
					addUndoState(undoList, redoList, current);

					endPoint.row = pt.row, endPoint.col = pt.col;

					drawLine(current->item, startPoint, endPoint, animate);
					displayCanvas(current->item);

				}
				else
				{
					displayCanvas(current->item);
				}

				//reset pt for future use
				pt.row = 0, pt.col = 0;
			}

			break;

		case 'B':

			// get user input for box size
			clearLine(MAXROWS + 1, 90);
			clearLine(MAXROWS + 2, 90);
			gotoxy(MAXROWS + 1, 0);
			cout << "Enter size: ";
			cin >> boxHeight;

			//ensure valid input
			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			// get user input for box position
			clearLine(MAXROWS + 1, 90);
			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";

			switch (toupper(getPoint(pt)))
			{
			case 'C':

				// make a backup for undo
				//copyCanvas(backupCanvas, canvas);
				addUndoState(undoList, redoList, current);

				// clear input char
				displayCanvas(current->item);

				// print box to canvas at center
				startPoint.row = (MAXROWS / 2), startPoint.col = (MAXCOLS / 2);
				drawBox(current->item, startPoint, boxHeight, animate);
				displayCanvas(current->item);

				//reset pt for future use
				pt.row = 0, pt.col = 0;

				break;

			case ESC:

				//reset pt for future use
				pt.row = 0, pt.col = 0;

				break;

			default:

				// make a backup for undo
				// copyCanvas(backupCanvas, canvas);
				addUndoState(undoList, redoList, current);

				// clear canvas
				displayCanvas(current->item);

				// print box to canvas at pt
				startPoint.row = pt.row, startPoint.col = pt.col;
				drawBox(current->item, startPoint, boxHeight, animate);
				displayCanvas(current->item);

				//reset pt for future use
				pt.row = 0, pt.col = 0;

				break;
			}

			break;

		case 'T':

			// get user input for box size
			clearLine(MAXROWS + 1, 90);
			clearLine(MAXROWS + 2, 90);
			gotoxy(MAXROWS + 1, 0);
			cout << "Enter approximate tree height: ";
			cin >> treeHeight;

			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			clearLine(MAXROWS + 1, 90);
			cout << "Enter branch angle: ";
			cin >> treeBranchAngle;

			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			clearLine(MAXROWS + 1, 90);
			cout << "Type any letter to choose start point, or <C> for bottom center / <ESC> to cancel";

			switch (toupper(getPoint(pt)))
			{
			case 'C':

				// make a backup for undo
				// copyCanvas(backupCanvas, canvas);
				addUndoState(undoList, redoList, current);

				// clear input char
				displayCanvas(current->item);

				// print trunk of tree to canvas at bottom center
				startPoint.row = (MAXROWS - 1), startPoint.col = (MAXCOLS / 2);

				treeRecursive(current->item, startPoint, treeHeight, treeTrunkAngle, treeBranchAngle, animate);

				// update canvas with tree to user
				displayCanvas(current->item);

				//reset pt for future use
				pt.row = 0, pt.col = 0;

				break;

			case ESC:

				//reset pt for future use
				pt.row = 0, pt.col = 0;

				break;

			default:

				// make a backup for undo
				// copyCanvas(backupCanvas, canvas);
				addUndoState(undoList, redoList, current);

				// clear input char
				displayCanvas(current->item);

				// print trunk of tree to canvas at chosen point
				startPoint.row = pt.row, startPoint.col = pt.col;

				treeRecursive(current->item, startPoint, treeHeight, treeTrunkAngle, treeBranchAngle, animate);

				// update canvas with tree to user
				displayCanvas(current->item);

				//reset pt for future use
				pt.row = 0, pt.col = 0;

				break;
			}

			break;


		case 'F':

			clearLine(MAXROWS + 1, 90);
			clearLine(MAXROWS + 2, 90);
			gotoxy(MAXROWS + 1, 0);
			cout << "Enter character to fill with from current location / <ESC> to cancel";

			switch (char newCh = getPoint(pt))
			{

			case ESC:

				//reset pt for future use
				pt.row = 0, pt.col = 0;

				break;

			default:

				// make a backup for undo
				// copyCanvas(backupCanvas, canvas);
				addUndoState(undoList, redoList, current);

				fillRecursive(current->item, pt.row, pt.col, current->item[pt.row][pt.col], newCh, animate);

				displayCanvas(current->item);

				//reset pt for future use
				pt.row = 0, pt.col = 0;

				break;
			}

			break;

		case 'N':

			// get user input for box size
			clearLine(MAXROWS + 1, 90);
			clearLine(MAXROWS + 2, 90);
			gotoxy(MAXROWS + 1, 0);
			cout << "Enter size: ";
			cin >> boxHeight;

			//ensure valid input
			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			// get user input for box position
			clearLine(MAXROWS + 1, 90);
			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";

			switch (toupper(getPoint(pt)))
			{
			case 'C':

				// make a backup for undo
				// copyCanvas(backupCanvas, canvas);
				addUndoState(undoList, redoList, current);

				// clear input char
				displayCanvas(current->item);

				// print box to canvas at center
				startPoint.row = (MAXROWS / 2), startPoint.col = (MAXCOLS / 2);
				drawBoxesRecursive(current->item, startPoint, boxHeight, animate);
				displayCanvas(current->item);

				//reset pt for future use
				pt.row = 0, pt.col = 0;
				break;

			case ESC:

				//reset pt for future use
				pt.row = 0, pt.col = 0;

				break;

			default:

				// make a backup for undo
				// copyCanvas(backupCanvas, canvas);
				addUndoState(undoList, redoList, current);

				// clear canvas
				displayCanvas(current->item);

				// print box to canvas at pt
				startPoint.row = pt.row, startPoint.col = pt.col;
				drawBoxesRecursive(current->item, startPoint, boxHeight, animate);
				displayCanvas(current->item);

				//reset pt for future use
				pt.row = 0, pt.col = 0;

				break;
			}

			break;

		case 'U':

			// backup canvas and display previous canvas
			// copyCanvas(canvas, backupCanvas);

			// restore current from undoList
			restore(undoList, redoList, current);

			// display to user
			displayCanvas(current->item);

			break;

		case 'O':

			// restore current from redoList
			restore(redoList, undoList, current);

			//display to user
			displayCanvas(current->item);

			break;

		case 'I':

			addNode(clips, newCanvas());
			copyCanvas(clips.head->item, current->item);

			break;

		case 'P':

			if (clips.count < 2)
			{
				return;
			}

			clearLine(MAXROWS + 1, 90);
			clearLine(MAXROWS + 2, 90);
			gotoxy(MAXROWS + 1, 0);

			cout << "Hold ESC to stop     ";
			cout << "Clip: ";

			play(clips);

			break;

		}

	} while (option != 'M');

	return;
}


// Get a single point from screen, with character entered at that point
char getPoint(Point& pt)
{
	// variables to hold user input and keep track of cursor position
	char input;

	gotoxy(pt.row, pt.col);

	// takes user input during edit mode
	do
	{
		input = _getch();

		// first check if input is = SPECIAL
		if (input == SPECIAL)
		{

			// get input second char and check if that = one of the arrow keys, move cursor
			input = _getch();

			if (input == LEFTARROW && pt.col > 0)
			{
				--pt.col;
				gotoxy(pt.row, pt.col);
			}
			else if (input == UPARROW && pt.row > 0)
			{
				--pt.row;
				gotoxy(pt.row, pt.col);
			}
			else if (input == RIGHTARROW && pt.col < MAXCOLS - 1)
			{
				++pt.col;
				gotoxy(pt.row, pt.col);
			}
			else if (input == DOWNARROW && pt.row < MAXROWS - 1)
			{
				++pt.row;
				gotoxy(pt.row, pt.col);
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

			// display input to user
			printf("%c", input);

			return input;
		}

	} while (input != ESC);

	return ESC;
}


// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	const int TIME = 50;
	Point chosenPoint(row, col);

	// base cases:
	if (row < 0 || col < 0 || row >= MAXROWS || col >= MAXCOLS || canvas[row][col] != oldCh || oldCh == newCh)
		return;
	else
		drawHelper(canvas, chosenPoint, newCh, animate);

	//recursive cases:
	//check space above, to the left, below, and to the right
	if (canvas[row][col] == newCh)
	{
		fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
		fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
		fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
		fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
	}
}


// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	// end of the trunk
	DrawPoint end;

	// base cases:
	if (height < 3)
	{
		return;
	}

	if (start.row < 0 || start.row > MAXROWS - 1 || start.col < 0 || start.col > MAXCOLS - 1)
	{
		return;
	}

	startAngle = startAngle % 360;

	// find the end of the trunk
	end = findEndPoint(start, height / 3, startAngle);

	// draw trunk
	drawLine(canvas, start, end, animate);

	// find left and right angles
	int leftAngle = startAngle - branchAngle;
	int rightAngle = startAngle + branchAngle;


	// print left and right branches
	treeRecursive(canvas, end, height - 2, leftAngle, branchAngle, animate);
	treeRecursive(canvas, end, height - 2, rightAngle, branchAngle, animate);

}


// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	// Stop recursion if height is too small
	if (height < 2) {
		return;
	}

	// draw the box at the current level
	drawBox(canvas, center, height, animate);

	// calculate the size of the nested box
	int nestedHeight = height - 2;

	// recursively draw the nested box
	drawBoxesRecursive(canvas, center, nestedHeight, animate);
}
