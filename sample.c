//#include<conio.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include<stdio.h>
/* For printing the Grid */
void print_grid(int n,int x[])
{
    char arr[20][20];
    int i,j;
    for(i=1;i<=n;i++)
    {
        for(j=1; j<=n; j++)
        {
            arr[i][j]='-';
        }
    }
 
    for(i=1;i<=n;i++)
    {
        arr[i][x[i]]='Q';
    }
 
    for( i=1;i<=n;i++)
    {
        for(j=1;j<=n;j++)
        {
            printf("\t%c",arr[i][j]);
        }
            printf("\n");
    }
 
}
 
/* For checking Queens placement is safe or not */
int safetoplace(int x[],int k)
{       int i;
    for(i=1;i<k;i++)
    {
        if(x[i]==x[k]||i-x[i]==k-x[k]||i+x[i]==k+x[k])
        {
            return 0;  /* False*/
        }
    }
    return 1; /*true*/
}
/* For printing the Queens and Placing them in Grid */
void nqueens(int n)
{
    int x[20];
    int count=0;
    int k=1;
 
    x[k]=0;
 
    while(k!=0)
    {
 
        x[k]=x[k]+1;
        while((x[k]<=n)&&(!safetoplace(x,k)))
        {
 
            x[k]=x[k]+1;
        }
        if(x[k]<=n)
        {
            if(k==n)
            {
                count++;
                printf("\n\tPlacement %d  is : \n\n\n",count);
                print_grid(n,x);
                getch();
 
            }
            else
            {
                k++;
                x[k]=0;
                 }
        }
        else
        {
            k--;
        }
    }
    return;
}
 
/* Main Function */
int main()
{
    int n;
    printf("----------------------------------------------------\n");
    printf("-------------Made by C codechamp--------------------\n");
    printf("----------------------------------------------------\n\n");
    printf("\t C PROGRAM OF N-QUEEN PROBLEM\n\n");
    printf("\nEnter the no. of Queens : ");
    scanf("%d",&n);
    printf("\n\n\tUSING %d QUEEN'S STRATEGY \n\n",n);
    nqueens(n);
    system("pause");
    getch();
}


