#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#if defined(_DEBUG)
#define dprintf(fmt, ...) printf("%s():%d "fmt,__func__,__LINE__,##__VA_ARGS__)
#else
#define dprintf(fmt, ...)
#endif

const int MAX_NUM_THREADS=48;
char **theBoard = NULL;
pthread_mutex_t col_lock = PTHREAD_MUTEX_INITIALIZER; // column lock

struct queen_data
{
	int nBoardSize;
	int *preset;
};

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

void *nqueen(void *qdp)//int nBoardSize, int *preset, int *column)
{
	unsigned long count=0;
	int q=0, k=0;
	int pass = 0;
	int backward = 0;
	struct queen_data *qd=(struct queen_data *)qdp;

	int nBoardSize = qd->nBoardSize;
	int *column = NULL;
	column = malloc(sizeof(int)*nBoardSize);
	int *preset = qd->preset;

	while(k<nBoardSize)
	{
		if(preset[k]>=0)
		{
			column[k]=preset[k];
			if(safe(column,  k))
				pass = 1 ;
		}
		else
		{
			for(q=backward; q<nBoardSize; q++)
			{
				column[k]=q;
				if(safe(column, k))
				{
					pass=1;
					break;
				}
			}
		}
		backward=0;
		if(pass)
		{
			k++;
			pass=0;
			if(k==nBoardSize)
				count++;
			else
				continue;
		}

		do {
			k--;
		} while(k>=0&&preset[k]>=0);
		if(k<0)
			break;
		column[k]+=1;
		backward=column[k];

	}

	free(column);
	return (void *)count;
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

	dprintf("# of preset chess --> ");
	fgets(buf, sizeof(buf), stdin);
	sscanf(buf, "%d", &nPresetChess);	
	dprintf("putting %d chesses\n", nPresetChess);

	int *column = NULL;
	int *presetChesses = NULL;
	int i,j;
	column = malloc(sizeof(int)*nBoardSize);
	presetChesses = malloc(sizeof(int)*nBoardSize);
	for(i=0; i<nBoardSize; i++)
	{
		presetChesses[i]=-1;
	}

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

	int x, y;
	while(nPresetChess>0)
	{
		dprintf("set coordinates --> ");
		fgets(buf, sizeof(buf), stdin);
		x = atoi(strtok(buf, " "));
		y = atoi(strtok(NULL, " "));
		dprintf("row=%d, column=%d\n", x+1, y+1);
		if(x<0 || y<0 || x >= nBoardSize || y >= nBoardSize || presetChesses[x]>0)
		{
			printf("invalid input\n");
			exit(-1);
		}
		presetChesses[x]=y;
		nPresetChess--;
	}


	int k,q;
	unsigned long long count=0;
	unsigned long long result=0;
	struct queen_data *qd;
	int numThreads=0;

	qd = malloc(sizeof(struct queen_data)*nBoardSize);

	pthread_t th_a[nBoardSize];
	for(k=0; k<nBoardSize; k++)
	{
		qd[k].nBoardSize = nBoardSize;
		qd[k].preset = malloc(sizeof(int)*nBoardSize);
		memcpy(qd[k].preset, presetChesses, sizeof(int)*nBoardSize);
		for(q=0; q<nBoardSize; q++)
		{
			if(presetChesses[q]<0)
				break;
		}

		if(q==nBoardSize)
		{
			pthread_create(&th_a[k], NULL, &nqueen, &qd[k]);
			numThreads++;
			break;
		}
		else
		{
			qd[k].preset[q]=k;
			pthread_create(&th_a[k], NULL, &nqueen, &qd[k]);
			numThreads++;
		}

	}

	for(k=0; k<numThreads; k++)
	{
		pthread_join(th_a[k], (void **)&count);
		result += count;
		free(qd[k].preset);
	}
	free(qd);


//	count = nqueen(nBoardSize, preset, column);

	dprintf("solution=%llu\n", result);
	for(i=0; i<nBoardSize; i++)
	{
		free(theBoard[i]);
	}
	free(theBoard);
	free(column);
	free(presetChesses);

	return 0;

}

