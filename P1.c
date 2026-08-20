#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
int main()
{
   pid_t pid = fork();
   if(pid == 0)
   {
      printf("P2 : PID =%d parent = %d \n",getpid(),getppid());
   }
   if(pid>0)
   {
      printf("P1:PID =%d parent = %d \n",getpid(),getppid());
   }
   return 0;
}

[24bcs170@mepcolinux ex1]$cc p1.c
[24bcs170@mepcolinux ex1]$./a.out
P1:PID =78294 parent = 78118 
P2 : PID =78295 parent = 78294 
