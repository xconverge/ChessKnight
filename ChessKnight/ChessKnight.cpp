/// Program to work with a Knight from a chessboard in various ways

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <map>
#include <stack>

using namespace std;


/// Basic struct used for representing an x, y coordinate
struct Point
{
	int x;
	int y;
};

// Operator for using the Point struct with the results path map
bool operator <(Point const& p1, Point const& p2)
{
	return p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y);
}


/// Function to display current state of the chess board (Level 1)
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
	if (start.x < 0 || start.x >= sizex || start.y < 0 || start.y >= sizey || end.x < 0 || end.x >= sizex || end.y < 0 || end.y >= sizey)
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
	int numMoves;

	vector<Point> localMoves;
};


/// Generate the available moves for each point (don't check for on the board yet)
vector<Point> GenerateLocalMoves(Point pt, int sizex, int sizey)
{
	vector<Point> localMoves;

	// Create potential local points that it can move to from this square
	Point localPoint[8];

	localPoint[0] = { pt.x + 1, pt.y + 2 };
	localPoint[1] = { pt.x + 1, pt.y - 2 };
	localPoint[2] = { pt.x - 1, pt.y + 2 };
	localPoint[3] = { pt.x - 1, pt.y - 2 };
	localPoint[4] = { pt.x + 2, pt.y + 1 };
	localPoint[5] = { pt.x + 2, pt.y - 1 };
	localPoint[6] = { pt.x - 2, pt.y + 1 };
	localPoint[7] = { pt.x - 2, pt.y - 1 };

	// For each potential local point, verify that it is a valid move before adding it to the list of local moves
	for (int i = 0; i < 8; i++)
	{
		if (IsValidMove(sizex, sizey, pt, localPoint[i]))
			localMoves.push_back(localPoint[i]);
	}

	return localMoves;
}

bool DFS(Point start, vector<vector<bool>> &explored, vector<vector<Node *>> &graph, map<Point, Point> &moveMap)
{
	// Current node is now explored
	explored[start.x][start.y] = true;

	bool arrived = false;
	
	// Recursive iterations for all connected nodes
	for (int i = 0; i < graph[start.x][start.y]->localMoves.size(); i++)
	{
		if (arrived)
			break;

		int nodex = graph[start.x][start.y]->localMoves[i].x;
		int nodey = graph[start.x][start.y]->localMoves[i].y;

		if (!explored[nodex][nodey])
		{
			DFS(graph[nodex][nodey]->localMoves[i], explored,graph,moveMap);
		}
	}

	return arrived;
}

/// Function to find a series of moves to get from the start point to the end point (Level 2)
stack<Point> AnyPath(int sizex, int sizey, Point start, Point end)
{
	// Create map for getting the move sequence at the end (how each node got to each other node)
	map<Point, Point> moveMap;

	// Create graph data structure
	vector<vector<Node *>> graph(sizex, vector<Node *>(sizey));

	// Add each square of the board to the graph
	for (int i = 0; i < sizex; i++)
	{
		for (int j = 0; j < sizey; j++)
		{
			Node *addNode = new Node;
			addNode->x = i;
			addNode->y = j;
			addNode->numMoves = 1;

			// Generate the possible moves for each square (maximum of 8)
			addNode->localMoves = GenerateLocalMoves(Point{ i,j }, sizex, sizey);

			// Add the Node to the graph
			graph[i][j] = addNode;
		}
	}

	// Initialize the graph to completely unexplored
	vector<vector<bool>> explored(sizex, vector<bool>(sizey, false));

	DFS(start, explored, graph, moveMap);

	// Stack used for collecting the order of moves it took to get from the start to the end point
	stack<Point> sequence;

	// Work backwards from the end point along the map of moves that were made to get to it to create the sequence
	Point curr = { end.x , end.y };
	while ((start.x != curr.x) && (start.y != curr.y))
	{
		sequence.push(curr);
		curr = moveMap[curr];
	}

	return sequence;
}


int main()
{
	// Initialize size of the board
	int sizex = 8;
	int sizey = 8;

	TestSequence();

	stack<Point> anyPathSequence = AnyPath(sizex, sizey, Point{ 0,0 }, Point{1,2});
	cout << "Basic path took " << size(anyPathSequence) << " moves." << endl;
	while(size(anyPathSequence) > 0)
	{
		cout << "( " << anyPathSequence.top().x << " , " << anyPathSequence.top().y << " )" << endl;
		anyPathSequence.pop();
	}

	getchar();
    return 0;
}

