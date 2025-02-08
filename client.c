#include "global.h"
#include "board.h"
#include "move.h"
#include "comm.h"
#include "minimax.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

/**********************************************************/
Position gamePosition;		// Position we are going to use

Move moveReceived;			// temporary move to retrieve opponent's choice
Move myMove;				// move to save our choice and send it to the server

char myColor;				// to store our color
int mySocket;				// our socket
char msg;					// used to store the received message

char * agentName = "SAMATA";		//default name.. change it! keep in mind MAX_NAME_LENGTH

char * ip = "127.0.0.1";	// default ip (local machine)

char * agent = "alphabeta"; 	// agent algorithm	
/**********************************************************/


int main( int argc, char ** argv )
{
	int c;
	opterr = 0;

	while( ( c = getopt ( argc, argv, "i:p:hrma" ) ) != -1 )
		switch( c )
		{
			case 'h':
				printf( "[-i ip] [-p port] [-r random] [-m minimax] [-a alphabeta pruning (minimax)] \n" );
				return 0;
			case 'm': 
				agent = "minimax";
				agentName = "minimax!";
				break;
			case 'a':
				agent = "alphabeta";
				agentName = "alphabet";
				break;
			case 'r': 
				agent = "random";
				agentName = "randomAg";
				break;
			case 'i':
				ip = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case '?':
				if( optopt == 'i' || optopt == 'p' )
					printf( "Option -%c requires an argument.\n", ( char ) optopt );
				else if( isprint( optopt ) )
					printf( "Unknown option -%c\n", ( char ) optopt );
				else
					printf( "Unknown option character -%c\n", ( char ) optopt );
				return 1;
			default:
			return 1;
		}

	connectToTarget( port, ip, &mySocket );

/**********************************************************
// used in random
	srand( time( NULL ) );
	int i, j;
**********************************************************/

	while( 1 )
	{

		msg = recvMsg( mySocket );

		switch ( msg )
		{
			case NM_REQUEST_NAME:		//server asks for our name
				sendName( agentName, mySocket );
				break;

			case NM_NEW_POSITION:		//server is trying to send us a new position
				getPosition( &gamePosition, mySocket );
				printPosition( &gamePosition );
				break;

			case NM_COLOR_W:			//server informs us that we have WHITE color
				myColor = WHITE;
				break;

			case NM_COLOR_B:			//server informs us that we have BLACK color
				myColor = BLACK;
				break;

			case NM_PREPARE_TO_RECEIVE_MOVE:	//server informs us that he will now send us opponent's move
				getMove( &moveReceived, mySocket );
				moveReceived.color = getOtherSide( myColor );
				doMove( &gamePosition, &moveReceived );		//play opponent's move on our position
				printPosition( &gamePosition );
				break;

			case NM_REQUEST_MOVE:		//server requests our move
				myMove.color = myColor;


				if( !canMove( &gamePosition, myColor ) )
				{
					myMove.tile[ 0 ] = NULL_MOVE;		// we have no move ..so send null move
				}
				else
				{


/***********************************************************
					if (strcmp(agent, "random") == 0){
						// random player - not the most efficient implementation
						while( 1 )
						{
							i = rand() % ARRAY_BOARD_SIZE;
							j = rand() % ARRAY_BOARD_SIZE;

							if( gamePosition.board[ i ][ j ] == EMPTY )
							{
								myMove.tile[ 0 ] = i;
								myMove.tile[ 1 ] = j;
								if( isLegalMove( &gamePosition, &myMove ) )
									break;
							}
						}
		
					

// end of random
**********************************************************/
					if (strcmp(agent, "minimax") == 0){
						myMove = getBestMove(gamePosition, myColor, FALSE);
						//if (myMove.tile[0] == -1 && myMove[1] == -1) myMove = NULL_MOVE;
					}else if (strcmp(agent, "alphabeta") == 0){
						myMove = getBestMove(gamePosition, myColor, TRUE);
					}
				}

				sendMove( &myMove, mySocket );			//send our move
				doMove( &gamePosition, &myMove );		//play our move on our position
				printPosition( &gamePosition );
				break;

			case NM_QUIT:			//server wants us to quit...we shall obey
				close( mySocket );
				return 0;
		}

	} 

	return 0;
}






