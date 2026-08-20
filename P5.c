#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
int main()
{
   int n;
   printf("enter a number:");
   scanf("%d",&n);
   pid_t pid=fork();
   if(pid<0)
   {
      perror("Fork failed");
      return 1;
   }
   if(pid==0)
   {
      int sum=0;
      int i; 
      
      for(i=1;i<=n;i++) 
      {
	 sum+=i;
      }
      printf("\n---child process---\n");
      printf("Sum of first %d natural number=%d\n", n, sum);
   }
   else
   {
      wait(NULL);
      printf("\n---parent process---\n");
      printf("collatz sequence: ");
	 while(n!=1)
	 {
	    printf("%d ", n); 
	    if(n%2==0){
	       n=n/2;
	    }
	    else
	    {
	       n=3*n+1;
	    }
	 }
      printf("%d\n", 1);
   }
   return 0;
}

[24bcs170@mepcolinux ex1]$cc p4.c
[24bcs170@mepcolinux ex1]$./a.out
enter a number:5

---child process---
Sum of first 5 natural number=15

---parent process---
collatz sequence: 5 16 8 4 2 1
[24bcs170@mepcolinux ex1]$exit
exit
