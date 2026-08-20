[24bcs170@mepcolinux EX4]$cat Producer.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_SIZE 1024

struct shared_buffer {
    int status_flag; // 0 = empty, 1 = filled, 2 = read complete
    char text[SHM_SIZE];
};

int main()
{
    int shmid;
    struct shared_buffer *shm;
    key_t key;

    // CHANGED: Using 'Z' to ensure a completely fresh key ID
    key = ftok(".", 'Z');
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    // Allocate memory matching the structure size
    shmid = shmget(key, sizeof(struct shared_buffer), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget failed");
        exit(1);
    }

    // Attach to the structure segment
    shm = (struct shared_buffer *)shmat(shmid, NULL, 0);
    if (shm == (struct shared_buffer *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Initialize status to Empty
    shm->status_flag = 0; 

    printf("Enter a string: ");
    fgets(shm->text, SHM_SIZE, stdin);

    // Set status flag to Filled
    shm->status_flag = 1; 

    printf("Data written successfully. Waiting for Consumer to read...\n");
    
    // Safely wait until the consumer finishes reading
    while (shm->status_flag != 2)
    {
        usleep(100000); 
    }

    printf("Consumer read completed. Cleaning up memory.\n");
    
    shmdt(shm);
    return 0;
}


[24bcs170@mepcolinux EX4]$cat consumer.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#define SHM_SIZE 1024

struct shared_buffer {
    int status_flag;
    char text[SHM_SIZE];
};

int main()
{
    int shmid;
    struct shared_buffer *shm;
    int i;
    key_t key;

    // CHANGED: Using 'Z' to match the Producer's fresh key ID
    key = ftok(".", 'Z');
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    // Access the shared segment
    shmid = shmget(key, sizeof(struct shared_buffer), 0666);
    if (shmid < 0)
    {
        perror("shmget failed. Is the producer running?");
        exit(1);
    }

    // Attach to the segment
    shm = (struct shared_buffer *)shmat(shmid, NULL, 0);
    if (shm == (struct shared_buffer *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Wait until the status flag shifts to 1
    printf("Waiting for producer to enter data...\n");
    while (shm->status_flag != 1) 
    {
        usleep(100000); 
    }
    
    printf("Consumer get the string from the producer: %s", shm->text);
    printf("Vowels in the string: ");
    
    for (i = 0; shm->text[i] != '\0'; i++)
    {
        char ch = tolower(shm->text[i]);
        if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
        {
            printf("%c ", shm->text[i]);
        }
    }
    printf("\n");
    
    // Signal back to producer that processing is finished
    shm->status_flag = 2;

    // Detach and remove the segment from the system completely
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

[24bcs170@mepcolinux EX4]$./producer
Enter a string: harish
Data written successfully. Waiting for Consumer to read...
^C
[24bcs170@mepcolinux EX4]$./consumer
Waiting for producer to enter data...
Consumer got the string from the producer: harish
Vowels in the string: a i 
[24bcs170@mepcolinux EX4]$exit
exit
