#include <stdio.h>

#define XMAX 500
#define YMAX 500

#define VISITED 'v'
#define UNVISITED 'u'

#define INFINITY 100000

#define WALL '#'
#define ENTRY 'E'
#define EXIT 'X'
#define WAY '0'
#define PATH 'P'
#define NEWLINE '\n'

//structure to hold co-ordinates in a map
struct point
{	int x, y;
}entry, xit, boundry;
typedef struct point point;

//structure to hold the "wire-frame" of the map. I can associate additional data such as weight, etc to each node on the map and then it can be manipulated easily.
struct node
{	point child;		//coordinates of the child pointed by this node. it can be NULL also! (NULL is represented by negative values for x and y in a coordinate)
	point self;			//coordinates of the node itself.
	int weight;			//path length of the node to the EXIT point. a low value of weight is preferable.
	char status;		// indicates wether the node has been already visited or unvisited. u - unvisited; v - visited;
	char symbol;		// symbol on the map at this coordinate. it may be WALL or ENTRY or PATH or _exit or ...
}map[YMAX][XMAX];		//wire-frame of the map described in map.txt
typedef struct node node;

//===========================================================
//function to open and read map.txt file and to create its wire-frame for analysis
int readMap()
{	int x=0, y=0;	//x and y to parse map
	char ch;		//ch: holds character stored in the map
	char filename[] = "map.txt" ;	//can be used get file name from user also..
	FILE *fp;						
	boundry.x = 0;	//setting the boundries of the map to be minimum by default
	boundry.y = 0;
	
	fp = fopen(filename, "r");
	if(fp == NULL)
	{	printf("\nerror opening input maze file..!");
		return -1;
	}

	while(1)	
	{	//read character from file
		ch = getc(fp);
		//put ch into wireframe
		map[y][x].symbol = ch;
		//set self coordinates in wireframe 
		map[y][x].self.x = x;
		map[y][x].self.y = y;
		//set wireframe node as unvisited
		map[y][x].status = UNVISITED;
		//set node's weight as very high.. as a lower weight is preferred while deriving the path from a node to the exit point
		map[y][x].weight = INFINITY;

		//if end-of-file is encountered, stop reading the file and finalise the wireframed map
		if(ch == EOF )
		{	break;
		}
		
		//if character is a newline character, increment the value of y in the wireframed map
		if(ch == NEWLINE)
		{   x++;
			y++;
			//y-boundary of the map is 1 less than the current value of y, if so, increment it..
			boundry.y++;
			if(x>boundry.x)
	 		    boundry.x = x;


			//as its a new line, x coordinate becomes 0
			x = 0;
		}					
		else
		{	//see if x-boundary of the map is lower than current value of x, if so, make them equal
		 	if(x >boundry.x)
	 		    boundry.x = x;

			//if the character is exit point (X in our case), mark it.
            if(ch == EXIT)
			{	xit.x = x;
				xit.y = y;
			}
			else
			{	//if the character is entry point (E in our case), mark it.
				if(ch == ENTRY)
				{	entry.x = x;
					entry.y = y;
				}
			}
			//now move ahead to explore the next character world!! :-p
			x++;
		}
    }//end of while

    //closing file
	fclose(fp);
	return 0;
}

//function to write the parsed, maniputed and pathed map to the file (map.txt in our case).
int writeMap()
{	//x, y used to move into wireframed map. newlineFlag indicates a newline so insert a newline character into the file
	int x=0, y=0;
	char ch;
	FILE *fp;
	
	fp = fopen("map.txt", "w");
	if(!fp)
	{	printf("\nerror opening output maze file..!");
		return -1;
	}
	
//	printing wire-framed map in the output file
	for(y=0; y<=boundry.y; y++)
	{	for(x=0; x<=boundry.x; x++)
		{	if(map[y][x].symbol == EOF || map[y][x].symbol == NULL)		//NULL so as to make it work even for non-rectangular maze B-)
			{   break;
			}
			putc(map[y][x].symbol, fp);
		}
	}
	fclose(fp);
 	return 0;
}

// function to display the wire-framed map on the output screen
int displayMap()
{	int x=0, y=0;
	char ch;
	printf("\n\n--------map---------:\n\n");
//	printing wire-framed map on the screen
	for(y=0; y<=boundry.y; y++)
	{	for(x=0; x<=boundry.x; x++)
		{	if(map[y][x].symbol == EOF || map[y][x].symbol == NULL)		//NULL so as to make it work even for non-rectangular maze B-)
			{   break;
			}
			putc(map[y][x].symbol, stdout);
		}
	}
	return 0;
}

//function to analyse wire-frame and discover the parent of each node and set its properties accordingly.
int parseMap()
{	int x, y, i, j;				//variables to move around in the wire-frame
	int rear, front;            //integers to mark front index and rear index of the parsing queue
	node *parsingQ[XMAX*YMAX];  //queue to hold discovered neighbours of nodes which have been visited but their neighbours have not been discovered yet
	node dummy;                 //to hold the node currently under analysis to find neighbours
	
	//initialising front and rear points
	front = rear = 0;
	
	//initialising our search from the exit point
	x = xit.x;
	y = xit.y;
	
	//marking characteristics of exit point manually
	map[y][x].weight = 0;
	map[y][x].status = VISITED;
	map[y][x].child.x = x;      //it has no further child; it may be the child (or (great)^n grand child of any other node ;-)
	map[y][x].child.y = y;

	//enquing the exit point first of all so as to discover its neighbours
	parsingQ[rear] = &map[y][x];
	rear++;
	
	while(front<rear)	//if front >= rear, this means there is no more element left to search any further
	{	dummy = *parsingQ[front];	//extracting the front element of the queue into dummy to parse it
		front++;
		x = dummy.self.x;           	//marking the coordinates of dummy
		y = dummy.self.y;
		map[y][x].status = VISITED;		//mark the node as visited now
		
		// if you hav reached the entry point, stop parsing the map (and print the path)
		if(map[y][x].self.x == entry.x && map[y][x].self.y == entry.y)
		{	//got the entry point. no need to parse any further..
			return 0;
		}
		
		//printf("\nmoving around node (%d, %d) weight:%d", x, y, map[y][x].weight);
		//checking all 8 neighbouring points to find possible parents and enquing unvisited to parsingQ[]
		for(i = -1; i<2; i++)			//loop---(1)
		{	for(j = -1; j<2; j++)		//loop---(2)
			{   //skip self check (already checked)
				if(i==j && j==0)
				{		continue;
				}

				//checking "unvisited" & "movable" neighbour of the point
				if( (map[y+i][x+j].symbol == WAY || map[y+i][x+j].symbol == ENTRY ) && ( map[y+i][x+j].status == UNVISITED ))
				{		//found a new parent.. set its attributes and enque him
						parsingQ[rear] = &map[y+i][x+j];
						rear++;

						//if the previous weight of the node is more than weight possible now, then remap the node
						if(map[y+i][x+j].weight > map[y][x].weight+1)
						{	map[y+i][x+j].weight = map[y][x].weight+1;
							map[y+i][x+j].child.x = x;
							map[y+i][x+j].child.y = y;
							map[y+i][x+j].status = VISITED;
						}
				}
			}//end of for loop--(2)
		}//end of for loop--(1)
	}//end of while loop
	
	return 0;
}

//function to change the symbols(from 0 to P) of the nodes forming the path
int locatePath()
{	int i=0, steps=0;   				// steps-> no of P's
	node *current;      				// denoting the current node
	//map[xit.y][xit.x].child.x=xit.x;    //making the exit points parents of itself
	//map[xit.y][xit.x].child.y=xit.y;
	
	//intializing the starting point from the ENTRY point so as to trace the path between entry and exit only
	current = &map[entry.y][entry.x];

	while(1)
	{	if((*current).symbol == WAY)
  		{	(*current).symbol = PATH;
  			steps++;					//increment the no. of steps
  		}

  		//making the new current point as the child of the current point (just like in linked list, we do node = node->next :-)
		current = &map[(*current).child.y][(*current).child.x];
		
		//if there is no further path, we can come out (either that point is an exit point or a dead end X-/
		if((*current).symbol != WAY )
		{   //if the symbol is not of exit point, dude! we are in a mess. path is broken!!
			if((*current).symbol != EXIT )
	 		{    //BROKEN PATH!;
	 			 steps = -1;
			}
			break;
		}
	}
	return steps;   // we are actually returning number of P's plotted in between E and X
}

int main()
{	int i, j, steps=0;
	int flag;           //flag to recieve errors returned by functions

	//lets read the given map
	flag = readMap();
	if(flag == -1)
	{	printf("\nerror reading map..\naborting..\n");
		getc(stdin);	//just to hold the screen
		return -1;
	}
	
	//parse the map to mark the nodes and fill in their structure in wire-frame ( map[][] );
	flag = parseMap();
	
	//based on child relations made by parseMap(), turn the symbols of shortest-path nodes to PATH symbol('P') to write in the file
	steps = locatePath();
	if(steps <= 0)
	{	printf("\npath broken!!\nnothing to write in the file..\n");
	}
	else
	{	printf("\nnumber of P's between E and X = %d\n", steps);
	
		//now we can write the found path in the file
		flag = writeMap();
		if(flag == -1)
		{	printf("\nerror writing file..\naborting..\n");
			getc(stdin);
			return -1;
		}
		
		//display the map to the screen
		displayMap();
	}
	
	printf("\n\npress a key to exit..\n\n");
	//just to hold the output screen
	getc(stdin);
	return 0;
}
