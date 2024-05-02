#include <iostream>
#include <Windows.h>
#include "Definitions.h"
#include <fstream>
using namespace std;


Node* newCanvas()
{
	// create a new node and set it to NULL
	Node* current = new Node;
	current->next = NULL;

	// initialize item of new node as blank spaces
	initCanvas(current->item);

	return current;
}


Node* newCanvas(Node* oldNode)
{
	// create a new node and set it to NULL
	Node* newNode = new Node;
	newNode->next = NULL;

	// initialize item of new node as item of oldNode
	copyCanvas(newNode->item, oldNode->item);

	return newNode;
}


void play(List& clips)
{
	// Check if there are at least 2 clips in the animation
	if (clips.count < 2)
	{
		return;
	}

	// Start playing the animation using recursive function
	clearLine(MAXROWS + 1, 90);  // clear the bottom line of the screen
	clearLine(MAXROWS + 2, 90);
	gotoxy(MAXROWS + 1, 0);

	cout << "Hold <ESC> to stop\t" << "Clip: ";

	// Wait for the user to press the Escape key
	while (!(GetKeyState(VK_ESCAPE) & 0x8000)) {
		playRecursive(clips.head, clips.count);
		// Sleep(100);  // pause for 100 milliseconds
	}
}


void playRecursive(Node* head, int count)
{
	// Base case: if the head pointer is null, return
	if (head == nullptr)
	{
		return;
	}

	// recurse to the next node
	playRecursive(head->next, count - 1);

	// display the current clip
	displayCanvas(head->item);

	// go to the bottom-left corner of the screen
	gotoxy(MAXROWS + 1, 32);
	cout << "           ";
	gotoxy(MAXROWS + 1, 32);

	// print the current clip number
	cout << count;
	gotoxy(MAXROWS + 1, 40);

	// pause for 100 milliseconds
	Sleep(100);

}


void addUndoState(List& undoList, List& redoList, Node*& current)
{
	// create a new node and copy to it from current
	Node* undoNode = new Node;
	copyCanvas(undoNode->item, current->item);

	// add the new node to undoList
	addNode(undoList, undoNode);

	// delete redoList
	deleteList(redoList);
}


void restore(List& undoList, List& redoList, Node*& current)
{
	// if nothing to restore, return
	if (undoList.count == 0)
		return;

	// otherwise, add current to the redoList
	addNode(redoList, current);

	// update current to removeNode
	current = removeNode(undoList);
}


void addNode(List& list, Node* nodeToAdd)
{
	//set the next pointer of the new node to the current head of the list
	nodeToAdd->next = list.head;

	//set the head of the list to the new node
	list.head = nodeToAdd;

	// increment the count of list
	list.count++;
}


Node* removeNode(List& list)
{
	// if no nodes in list, return
	if (list.head == NULL)
		return NULL;

	// save a pointer to the first node in the list
	Node* removedNode = list.head;

	// set the head of the list to the next node
	list.head = removedNode->next;

	// decrement the count of items in the list
	list.count--;

	return removedNode;
}


void deleteList(List& list)
{
	// while nodes in list, delete them
	while (list.head != NULL)
	{
		Node* temp = list.head->next;
		delete list.head;
		list.head = temp;
	}

	// update count to 0
	list.count = 0;
}


bool loadClips(List& clips, char filename[])
{
	// char array to hold the full filepath
	char filePath[FILENAMESIZE];

	// delete clips list
	deleteList(clips);

	// keep track of # of files
	int num = 1;

	// attach the file path to the beginning of the file name entered by user
	snprintf(filePath, FILENAMESIZE, "SavedFiles\\%s", filename);

	// a copy of filename that will have edits done
	char fileNameCopy[FILENAMESIZE];
	snprintf(fileNameCopy, FILENAMESIZE, filePath);
	snprintf(fileNameCopy, FILENAMESIZE, "%s-%d.txt", filePath, num);

	// make current an empty canvas
	Node* current = newCanvas();

	// while files left to load, pass them to loadCanvas
	while (loadCanvas(current->item, fileNameCopy))
	{
		addNode(clips, current);
		current = newCanvas();

		snprintf(fileNameCopy, FILENAMESIZE, " ");
		num++;
		snprintf(fileNameCopy, FILENAMESIZE, "SavedFiles\\%s-%d.txt", filename, num);

	}

	// delete current
	while (current != NULL)
	{
		Node* temp = current;
		current = current->next;
		delete temp;
	}

	// if no file was opened return false
	if (clips.head == NULL)
	{
		return false;
	}

	// update user
	cout << "Clips Loaded!\n";
	system("pause");

	clearLine(MAXROWS + 1, 90);
	clearLine(MAXROWS + 2, 90);
	clearLine(MAXROWS + 3, 90);

	return true;
}


bool saveClips(List& clips, char filename[])
{
	// char array to hold the full filepath
	char fileNameCopy[FILENAMESIZE];

	// keep track of # of files
	int num = 1;

	// copy of list of clips
	List copy = clips;

	Node* currentClip = copy.head;
	Node* next1 = NULL;
	Node* previousNode = NULL;

	// put copied list in reverse order
	while (currentClip != NULL)
	{
		next1 = currentClip->next;
		currentClip->next = previousNode;
		previousNode = currentClip;
		currentClip = next1;
	}

	copy.head = previousNode;

	// check if clips left to save
	if (copy.head == NULL)
	{
		return false;
	}

	Node* current = copy.head;
	Node* next2 = current->next;

	while (current != NULL)
	{
		// create full file name
		snprintf(fileNameCopy, FILENAMESIZE, "%s-%d.txt", filename, num);

		if (saveCanvas(current->item, fileNameCopy))
		{
			next2 = current->next;
			current = next2;
		}
		else
		{
			return false;
		}

		// reset copy of file name
		snprintf(fileNameCopy, FILENAMESIZE, " ");
		num++;
	}

	deleteList(copy);
	return true;
}
