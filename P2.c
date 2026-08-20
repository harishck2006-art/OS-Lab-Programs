#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
int main()
{
   pid_t pid1,pid2;
   pid1 = fork();
   if(pid1<0)
   {
      perror("Fork failed");
      return 1;
   }
   if(pid1==0)
   {
      printf("p2:PID=%d Parent =%d \n",getpid(),getppid());
      pid2=fork();
      if(pid2<0)
      {
	 perror("Fork failed");
	    return 1;
      }
      if(pid2 == 0)
      {
	 printf("p3:PID=%d Parent =%d \n",getpid(),getppid());
      }
      else
      {
	 wait(NULL);
      }
   }
   else
   {
      wait(NULL);
	 printf("P1:PID=%d \n",getpid());
   }
   return 0;
}

[24bcs170@mepcolinux ex1]$cc p2.c
[24bcs170@mepcolinux ex1]$./a.out
p2:PID=78404 Parent =78403 
p3:PID=78405 Parent =78404 
P1:PID=78403 
