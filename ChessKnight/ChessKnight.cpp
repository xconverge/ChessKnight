/// Program to work with a Knight from a chessboard in various ways

#include "stdafx.h"
#include <vector>
#include <iostream>

using namespace std;


/// Basic struct used for representing an x, y coordinate
struct Point
{
	int x;
	int y;
};


/// Function to display current state of the chess board
/** Inputs: size = size of board
  *		    start = starting position
  *		    end = ending position
  *		    current = current position of the knight */
void DisplayBoard(int sizex, int sizey, Point start, Point end, Point current)
{	
	for (int i = 0; i < sizey; i++)
	{
		for (int j = 0; j < sizex; j++)
		{
			if (current.x == j && current.y == i)
				cout << " K";
			else if (start.x == j && start.y == i)
				cout << " S";
			else if (end.x == j && end.y == i)
				cout << " E";
			else
				cout << " .";
		}
		// Create a new row
		cout << endl;
	}
	cout << endl;
}


/// Function to determine if a knight's move is valid based on the standard rules of chess
bool IsValidMove(int sizex, int sizey, Point start, Point end)
{
	// Move is outside the bounds of the board
	if (start.x < 0 || start.x > sizex || start.y < 0 || start.y > sizey || end.x < 0 || end.x > sizex || end.y < 0 || end.y > sizey)
		return false;

	if ((abs(end.x-start.x) == 1 && abs(end.y-start.y) == 2) || (abs(end.x - start.x) == 2 && abs(end.y - start.y) == 1))
		return true;
	else
		return false;
}


/// Function to determine if a sequence of moves for the knight is valid
bool IsValidSequence(Point moves[], int numMoves, bool displayMoves, int sizex, int sizey, Point start, Point end)
{
	for (int i = 1; i < numMoves; i++)
	{
		// Display move as an option
		if(displayMoves)
			DisplayBoard(sizex, sizey, start, end, moves[i]);

		if (!IsValidMove(sizex, sizey, moves[i - 1], moves[i]))
		{
			//Invalid move for the knight!
			return false;
		}
	}
	return true;
}


/// Basic unit test for testing a sequence and validation
void TestSequence()
{
	Point test[5];
	test[0] = Point{ 1,2 };
	test[1] = Point{ 3,3 };
	test[2] = Point{ 4,5 };
	test[3] = Point{ 3,3 };
	test[4] = Point{ 1,2 };

	Point startPos = Point{ 1,2 };
	Point endPos = Point{ 5,4 };
	Point currentPos = Point{ 0,0 };

	if (IsValidSequence(test, 5, true, 8, 8, startPos, endPos))
		cout << "Valid" << endl;
	else
		cout << "Invalid!" << endl;
}


/// Node structure to be used in graph data structure (array of edges)
struct Node
{
	int x;
	int y;

	vector<Point> localMoves;
};


/// Generate the available moves for each point (don't check for on the board yet)
vector<Point> GenerateLocalMoves(Point pt, int sizex, int sizey)
{
	vector<Point> localMoves(8);

	localMoves[0] = { pt.x + 1, pt.y + 2 };
	localMoves[1] = { pt.x + 1, pt.y - 2 };
	localMoves[2] = { pt.x - 1, pt.y + 2 };
	localMoves[3] = { pt.x - 1, pt.y - 2 };
	localMoves[4] = { pt.x + 2, pt.y + 1 };
	localMoves[5] = { pt.x + 2, pt.y - 1 };
	localMoves[6] = { pt.x - 2, pt.y + 1 };
	localMoves[7] = { pt.x - 2, pt.y - 1 };

	return localMoves;
}


/// Function to find the least number of moves to get from start point to end point
void ShortestPath(int sizex, int sizey, Point start, Point end, vector<int>& dist)
{
	int startNode;
	int endNode;

	// Create graph data structure
	Node **graph = new Node*[sizex * sizey];

	// Add each square of the board to the graph
	int idx = 0;
	for (int i = 0; i < sizex; i++)
	{
		for (int j = 0; j < sizey; j++)
		{
			if (start.x == i && start.y == j)
				startNode = idx;
			if (end.x == i && end.y == j)
				endNode = idx;

			Node *addNode = new Node;
			addNode->x = i;
			addNode->y = j;
			// Generate the 8 possible moves for each square
			addNode->localMoves = GenerateLocalMoves(Point{ i,j }, sizex ,sizey);
			graph[idx] = addNode;
			idx++;
		}
	}


	//// TODO
	///////////////////////////////////////////////////////////////////////////////////////////////Generated neighbors to each square on the board. Now need to compute the shortest path sequenece from the starting node to all other nodes

	/*
	//Create a flag for whether or not a node has been visited and initialize to false
	vector<bool> visited(sizex * sizey, false);

	visited[startNode] = true;
	dist[startNode] = 0;

	// Iterate over all nodes to calculate shortest path between them and startNode (1)
	for (int i = 0; i < (sizex * sizey); i++)
	{
		int node2, length = 1000000;

		// Calculate shortest path by adding all edges in a BFS manner
		for (int i = 1; i <= (sizex * sizey); i++)
		{
			if (visited[i])
			{
				Node *tmp = graph[i];
				while (tmp != NULL)
				{
					// Calculate path to see if shorter
					if (!visited[tmp->node2] && (dist[i] + tmp->length)<length)
					{
						node2 = tmp->node2;
						length = (dist[i] + tmp->length);
					}
					tmp = tmp->nextEdge;
				}
			}
		}
		visited[node2] = true;
		dist[node2] = length;
	}

	*/	
}


int main()
{
	// Initialize size of the board
	int sizex = 8;
	int sizey = 8;

	TestSequence();

	// Create structure to hold the number of moves data for each instance of node 1 to node X
	vector<int> dist(sizex * sizey);

	ShortestPath(sizex, sizey, Point{ 0,0 }, Point{1,2}, dist);

	getchar();
    return 0;
}

