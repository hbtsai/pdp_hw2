#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>
#include <errno.h>

#if defined(_DEBUG)
#define dprintf(fmt, ...) printf("%s():%d "fmt,__func__,__LINE__,##__VA_ARGS__)
#else
#define dprintf(fmt, ...)
#endif

const int MAX_NUM_THREADS=48;
char **theBoard = NULL;
pthread_mutex_t col_lock = PTHREAD_MUTEX_INITIALIZER; // column lock
int nBoardSize = 0;
unsigned long long result=0;

struct queen_data
{
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


	pthread_mutex_lock(&col_lock);
	result+=count;
	pthread_mutex_unlock(&col_lock);
	
	free(column);
}

int main(int argc, char** argv)
{


	pthread_attr_t pattr;
	size_t stacksize;
	pthread_attr_init(&pattr);
	pthread_attr_getstacksize(&pattr, &stacksize);
	dprintf("checkpoint stacksize=%u\n", stacksize );
	pthread_attr_setstacksize(&pattr, PTHREAD_STACK_MIN*1024);

	char buf[1024]={0};
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


	int k=0,q=0,p=0;
	struct queen_data *qd;
	int numThreads=0;

	qd = malloc(sizeof(struct queen_data)*nBoardSize*nBoardSize);

	pthread_t *th_a=malloc(sizeof(pthread_t)*nBoardSize*nBoardSize);

	for(p=0; p<nBoardSize*nBoardSize;p++)
	{
		qd[p].preset = malloc(sizeof(int)*nBoardSize);
		memcpy(qd[p].preset, presetChesses, sizeof(int)*nBoardSize);
	}

	int b1=-1, b2=-1;
	int ret=0;

	for(q=0; q<nBoardSize; q++)
	{
		if(presetChesses[q]<0)
		{
			if(b1>=0)
				b2=q;
			else
				b1=q;
		}
		if(b1>=0 && b2>=0)
			break;
	}

	for(p=0; p<nBoardSize; p++)
	{
		if(b2<0)
		{
			pthread_create(&th_a[k+p], NULL, &nqueen, &qd[k+p]);
			numThreads++;
			break;
		}

		for(k=0; k<nBoardSize; k++)
		{
			if(b1<0)
			{
				pthread_create(&th_a[k+p], NULL, &nqueen, &qd[k+p]);
				numThreads++;
				break;
			}
			else
			{
				qd[k+p*nBoardSize].preset[b1]=p;
				qd[k+p*nBoardSize].preset[b2]=k;
				ret = pthread_create(&(th_a[k+p*nBoardSize]), NULL, &nqueen, &qd[k+p*nBoardSize]);
				if(ret!=0)
				{
					while(numThreads>0)
					{
						pthread_join(th_a[(k+p*nBoardSize)-numThreads], NULL);
						numThreads--;
					}
				pthread_create(&(th_a[k+p*nBoardSize]), NULL, &nqueen, &qd[k+p*nBoardSize]);
				}

				numThreads++;
			}
		}
	}

	for(k=0; k<numThreads; k++)
	{
		pthread_join(th_a[nBoardSize*nBoardSize-k-1], NULL);
	}

	printf("solution=%llu\n", result);

	for(p=0; p<nBoardSize*nBoardSize; p++)
	{
		free(qd[p].preset);
	}
	free(qd);
	for(i=0; i<nBoardSize; i++)
	{
		free(theBoard[i]);
	}
	free(theBoard);
	free(presetChesses);

	return 0;

}

