[?1034h[24bcs170@mepcolinux EX4]$cat P2.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define row 2
#define col 2
#define SHM_size (sizeof(int) * row * col * 3)

int main()
{
    int shmid;
    int *shm;
    key_t key = 5678;
    pid_t pid;
    int i;

    shmid = shmget(key, SHM_size, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget failed");
        exit(1);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("Fork failed");
        exit(1);
    }
    else if (pid > 0) // Parent Process (P1)
    {
        shm = (int *)shmat(shmid, NULL, 0);
        
       
        shm[row * col * 2] = 0; 

        printf("Enter elements of Matrix 1 (%dx%d):\n", row, col);
        for (i = 0; i < row * col; i++)
        {
            scanf("%d", &shm[i]);
        }

        printf("Enter elements of Matrix 2 (%dx%d):\n", row, col);
        for (i = 0; i < row * col; i++)
        {
            scanf("%d", &shm[row * col + i]);
        }

       
        shm[row * col * 2] = 1; 

        wait(NULL);
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
    }
    
    {
        shm = (int *)shmat(shmid, NULL, 0);

      
        while (shm[row * col * 2] != 1)
        {
            usleep(100000);
        }

        printf("\nSum of the matrices:\n");
        for (i = 0; i < row * col; i++)
        {
            int sum = shm[i] + shm[row * col + i];
            printf("%d ", sum);
            
          
            if ((i + 1) % col == 0)
            {
                printf("\n");
            }
        }

        shmdt(shm);
        exit(0);
    }
    return 0;
}
