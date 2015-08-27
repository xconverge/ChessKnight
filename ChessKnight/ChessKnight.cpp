/// Program to work with a Knight from a chessboard in various ways

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <map>
#include <stack>
#include <queue>
#include <fstream>
#include <sstream>

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

/// Node structure to be used in graph data structure (array of edges)
struct Node
{
	int x;
	int y;
	int numMoves;
	char type;

	vector<Point> localMoves;
};


/// Function to display current state of the chess board
//  Level 1
/** Inputs: size = size of board
  *		    start = starting position
  *		    end = ending position
  *		    current = current position of the knight */
void DisplayBoard(int sizex, int sizey, Point start, Point end, Point current, vector<vector<Node *>> &graph)
{	
	for (int i = 0; i < sizey; i++)
	{
		for (int j = 0; j < sizex; j++)
		{
			if (current.x == j && current.y == i)
				cout << "K ";
			else if (start.x == j && start.y == i)
				cout << "S ";
			else if (end.x == j && end.y == i)
				cout << "E ";
			else
				cout << graph[j][i]->type << " ";
		}
		// Create a new row
		cout << endl;
	}
	cout << endl;
}


bool IsPathClear(Point start, Point end, vector<vector<Node *>> &graph)
{

	int xinc, yinc;
	if ((end.x - start.x) > 0)
		xinc = 1;
	else
		xinc = -1;

	if ((end.y - start.y) > 0)
		yinc = 1;
	else
		yinc = -1;

	for (int x = start.x; x != end.x + xinc; x += xinc)
	{
		for (int y = start.y; y != end.y + yinc; y += yinc)
		{
			if (graph[x][y]->type == 'B')
			{
				return false;
			}

		}
	}

	return true;
}

/// Function to determine if a knight's move is valid based on the standard rules of chess
bool IsValidMove(int sizex, int sizey, Point start, Point end, vector<vector<Node *>> &graph, bool teleportersEnabled)
{
	// Move is outside the bounds of the board
	if (start.x < 0 || start.x >= sizex || start.y < 0 || start.y >= sizey || end.x < 0 || end.x >= sizex || end.y < 0 || end.y >= sizey)
		return false;

	if ((abs(end.x - start.x) == 1 && abs(end.y - start.y) == 2) || (abs(end.x - start.x) == 2 && abs(end.y - start.y) == 1))
	{
		return true;
	}
	else if (teleportersEnabled)
	{
		cout << "Teleporting from: " << start.x << "," << start.y << " to: " << end.x << ", " << end.y;
	}
	else
	{
		return false;
	}
}


/// Function to determine if a sequence of moves for the knight is valid
bool IsValidSequence(stack<Point> moves, bool displayMoves, int sizex, int sizey, Point start, Point end, vector<vector<Node *>> &graph, bool teleportersEnabled)
{
	Point current = start;

	while (size(moves) > 0)
	{

		if (!IsValidMove(sizex, sizey, current, moves.top(), graph, teleportersEnabled))
		{
			//Invalid move for the knight!
			return false;
		}

		// Display move as an option
		if(displayMoves)
			DisplayBoard(sizex, sizey, start , end, moves.top(), graph);

		current = moves.top();

		moves.pop();
	}
	return true;
}


/// Generate the available moves for each point (don't check for on the board yet)
vector<Point> GenerateLocalMoves(Point pt, int sizex, int sizey, vector<vector<Node *>> &graph)
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
		if (IsValidMove(sizex, sizey, pt, localPoint[i], graph, true))
			localMoves.push_back(localPoint[i]);
	}

	return localMoves;
}

/// Recursive function to perform depth first search (not necessarily the shortest path)
//  Level 2
void DFS(Point start, vector<vector<bool>> &explored, vector<vector<Node *>> &graph, map<Point, Point> &moveMap)
{
	// Current node is now explored
	explored[start.x][start.y] = true;
	
	// Recursive iterations for all connected nodes
	for (int i = 0; i < graph[start.x][start.y]->localMoves.size(); i++)
	{
		// Determine which local nearby node to move to next
		int nodex = graph[start.x][start.y]->localMoves[i].x;
		int nodey = graph[start.x][start.y]->localMoves[i].y;

		// If it is not already explored, move to it
		if (!explored[nodex][nodey])
		{
			// Set the parent node of the local node so that the map stays in sync with how the knight arrived there
			moveMap[graph[start.x][start.y]->localMoves[i]] = start;

			// Recursively move from the local neighbor next as the start point
			DFS(graph[start.x][start.y]->localMoves[i], explored, graph, moveMap);
		}
	}
}


/// Use Dijkstra's shortest path algorithm to find the least number of moves necessary
//  Level 3 and 4
void Dijkstra(Point start, vector<vector<bool>> &explored, vector<vector<Node *>> &graph, map<Point, Point> &moveMap, int sizex, int sizey)
{
	// Data structure to hold the distances needed to move to each square
	vector<vector<int>> dist(sizex, vector<int>(sizey, 99999));

	// Current node is now explored
	explored[start.x][start.y] = true;
	dist[start.x][start.y] = 0;

	// Create a queue for the nodes to be processed
	queue <Node*> nodes;

	// Add the start node as the first item in the queue so that it starts searching from there
	nodes.push(graph[start.x][start.y]);

	// Add the rest of the nodes to the queue
	for (int i = 0; i < sizex; i++)
	{
		for (int j = 0; j < sizey; j++)
		{
			if (start.x == i && start.y == j)
				continue;
			else
				nodes.push(graph[i][j]);
		}
	}
	Node* curr;

	// While the queue has nodes, continue processing the number of moves necessary to reach the square
	while(!nodes.empty())
	{
		curr = nodes.front();
		nodes.pop();

		// Iterate for all connected nodes to find the shortest distances in a BFS way
		for (int k = 0; k < curr->localMoves.size(); k++)
		{
			// Select the neighbor
			int nodex = curr->localMoves[k].x;
			int nodey = curr->localMoves[k].y;

			// Calculate the potential number of moves to this neighbor
			int newDist = dist[curr->x][curr->y] + graph[nodex][nodey]->numMoves;

			// Check if there are any barriers in between the start and end point
			if (!IsPathClear({ curr->x,curr->y }, curr->localMoves[k], graph))
				continue;

			if (!explored[nodex][nodey] && (newDist < dist[nodex][nodey]))
			{
				// Set the parent node of the local node so that the map stays in sync with how the knight arrived there
				moveMap[curr->localMoves[k]] = { curr->x , curr->y };

				//Adjust the distance by adding the new number of moves to the original amount to get to that square
				dist[nodex][nodey] = newDist;
				explored[nodex][nodey] = true;
			}
		}
	}
}


/// Function to find a series of moves to get from the start point to the end point
/** Inputs: size = size of board
*		    start = starting position
*		    end = ending position
*		    pathType = 1 for any path, 2 for shortest path */
stack<Point> FindPath(int sizex, int sizey, Point start, Point end, int pathType, vector<vector<Node *>> graph)
{
	// Create map for getting the move sequence at the end (how each node got to each other node)
	map<Point, Point> moveMap;

	// Initialize the graph to completely unexplored
	vector<vector<bool>> explored(sizex, vector<bool>(sizey, false));

	switch (pathType)
	{
	case 1:
		// Compute paths from start to all possible end points with DFS
		DFS(start, explored, graph, moveMap);
		break;

	case 2:
		// Compute paths from start to all possible end points with dijkstra's algorithm
		Dijkstra(start, explored, graph, moveMap, sizex, sizey);
		break;
	}

	// Stack used for collecting the order of moves it took to get from the start to the end point
	stack<Point> sequence;

	// Work backwards from the end point along the map of moves that were made to get to it to create the sequence
	Point curr = { end.x , end.y };
	while ((start.x != curr.x) || (start.y != curr.y) && (size(sequence) > 0))
	{
		sequence.push(curr);
		curr = moveMap[curr];
	}

	// Return a stack of the moves necessary to get to the end point
	return sequence;
}


//Function used to split a string into a vector
vector<std::string> &split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


//Function used to split a string into a vector
vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}


vector<vector<Node *>> CreateGraph(int sizex, int sizey, bool fromFile)
{
	if (fromFile)
	{
		ifstream file("C:\\Users\\sean\\Downloads\\chessboard.txt", ios::in);
		string tmp;

		// Create graph data structure
		vector<vector<Node *>> graph(sizex, vector<Node *>(sizey));

		// Index of y component
		int y = 0;
		bool teleportFound = false;
		Point teleporter;

		//Read file into graph structure
		while (!file.eof())
		{
			getline(file, tmp);
			vector<string> splitLine = split(tmp, ' ');
			if (splitLine.size() == 0)
				continue;

			for (int i = 0; i < size(splitLine); i++)
			{
				Node *addNode = new Node;
				addNode->x = i;
				addNode->y = y;

				addNode->type = splitLine[i][0];

				switch (splitLine[i][0])
				{
				case '.':
					addNode->numMoves = 1;
					break;
				case 'W':
					addNode->numMoves = 2;
					break;
				case 'L':
					addNode->numMoves = 5;
					break;
				case 'R':
					addNode->numMoves = 999999;
					break;
				case 'B':
					addNode->numMoves = 999999;
					break;
				case 'T':
					addNode->numMoves = 1;
					break;
				}

				// Generate the possible moves for each square (maximum of 8)
				addNode->localMoves = GenerateLocalMoves(Point{ i,y }, sizex, sizey, graph);

				if (addNode->type == 'T' && !teleportFound)
				{
					teleporter = { i, y };
					teleportFound = true;
				}
				else if (addNode->type == 'T')
				{
					addNode->localMoves = { teleporter };
				}

				// Add the Node to the graph
				graph[i][y] = addNode;
			}

			y++;
		}
		file.close();

		return graph;
	}
	else
	{
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
				addNode->localMoves = GenerateLocalMoves(Point{ i,j }, sizex, sizey, graph);

				// Add the Node to the graph
				graph[i][j] = addNode;
			}
		}

		return graph;
	}
}



/// Main function used to run the Knight algorithms
int main()
{
	// Initialize size of the board
	int sizex = 8;
	int sizey = 8;

	Point start = { 0,0 };
	Point end = { 3,3 };

	vector<vector<Node *>> graph = CreateGraph(sizex, sizey, false);

	// Perform DFS to find any path between the start point and end point
	stack<Point> anyPathSequence = FindPath(sizex, sizey, start, end, 1, graph);
	
	if (size(anyPathSequence) && IsValidSequence(anyPathSequence, false, sizex, sizey, start, end, graph, false))
	{
		cout << "Basic path took " << size(anyPathSequence) << " moves." << endl;
		while (size(anyPathSequence) > 0)
		{
			cout << "( " << anyPathSequence.top().x << " , " << anyPathSequence.top().y << " )" << endl;
			anyPathSequence.pop();
		}
	}
	else
	{
		cout << "No valid path found." << endl;
	}

	cout << endl;

	// Perform shortest path algorithm for the knight
	stack<Point> shortestPathSequence = FindPath(sizex, sizey, start, end, 2, graph);

	if (size(shortestPathSequence) && IsValidSequence(shortestPathSequence, false, sizex, sizey, start, end, graph, false))
	{
		cout << "Shortest path took " << size(shortestPathSequence) << " moves." << endl;
		while (size(shortestPathSequence) > 0)
		{
			cout << "( " << shortestPathSequence.top().x << " , " << shortestPathSequence.top().y << " )" << endl;
			shortestPathSequence.pop();
		}
	}
	else
	{
		cout << "No valid path found." << endl;
	}

	sizex = 32;
	sizey = 32;

	start = { 6,0 };
	end = { 10,0 };

	vector<vector<Node *>> fileGraph = CreateGraph(sizex, sizey, true);

	// Perform shortest path algorithm for the knight on the complex board
	stack<Point> complexShortestPath = FindPath(sizex, sizey, start, end, 2, fileGraph);

	if (size(complexShortestPath) && IsValidSequence(complexShortestPath, true, sizex, sizey, start, end, fileGraph, true))
	{
		cout << "Shortest path on complex board took " << size(complexShortestPath) << " moves." << endl;
		while (size(complexShortestPath) > 0)
		{
			cout << "( " << complexShortestPath.top().x << " , " << complexShortestPath.top().y << " )" << endl;
			complexShortestPath.pop();
		}
	}
	else
	{
		cout << "No valid path found." << endl;
	}

	getchar();
    return 0;
}

