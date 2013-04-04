#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#if defined(_DEBUG)
#define dprintf(fmt, ...) printf("%s():%d "fmt,__func__,__LINE__,##__VA_ARGS__)
#else
#define dprintf(fmt, ...)
#endif

void print_board(char **board, int size)
{
	int i, j;
	printf("\n");
	for(i=0; i<size; i++)
	{
		for(j=0; j<size; j++)
		{
			printf("%c ", board[i][j]);
		}
		printf("\n");
	}
}

int safe(int *column, int k)
{
	int x=0;
	while(x<k)
	{
		if(column[x]==column[k] /* same column */ || 
				(k-column[k])==(x-column[x]) /* same left-up-right-down line */ || 
				(x+column[x])==(k+column[k]) /* same right-up-left-down line */ )
		{
			return 0;
		}
		x++;
	}
	return 1;
}

int main(int argc, char** argv)
{
	char buf[1024]={0};
	int nBoardSize;
	int nPresetChess;
	dprintf("board size --> ");
	fgets(buf, sizeof(buf), stdin);
	sscanf(buf, "%d", &nBoardSize);	
	dprintf("creating %d x %d board\n", nBoardSize, nBoardSize);

	char **theBoard = NULL;
	int *column = NULL;
	int i,j;
	column = malloc(sizeof(int)*nBoardSize);

	/*
	 * initialize theBoard
	 */
	theBoard = malloc(sizeof(char *) *nBoardSize);
	for(i=0; i<nBoardSize; i++)
	{
		theBoard[i] = malloc(sizeof(char)*nBoardSize);
		for(j=0; j<nBoardSize; j++)
		{
			theBoard[i][j]='X';
		}
	}

	int k,q;
	int backward=0;
	int pass=0;
	int count=0;

	while(k<nBoardSize)
	{
			for(q=backward; q<nBoardSize; q++)
			{
				column[k]=q;
				if(safe(column, k))
				{
					theBoard[k][column[k]] = 'Q';
					pass=1;
					break;
				}
			}

		backward=0;

		if(pass)
		{
			k++;
			pass=0;
			if(k==nBoardSize)
			{
				count++;
				print_board(theBoard, nBoardSize);
			}
			else
				continue;
		}

		k--;
		if(k<0)
			break;
		theBoard[k][column[k]]='X';
		column[k]+=1;
		backward=column[k];

	}

	printf("solution=%d\n", count);
	for(i=0; i<nBoardSize; i++)
	{
		free(theBoard[i]);
	}
	free(theBoard);
	free(column);

	return 0;

}

