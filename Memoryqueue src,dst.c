[24bcs170@mepcolinux MQ]$cat producer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>

struct message_buffer
{
    long message_type;
    char message_text[1024];
};

int main()
{
    key_t key;
    int message_queue_id;
    char src_filename[256];
    
    printf("Enter source filename: ");
    scanf("%s", src_filename);
    
    key = ftok(".", 66);
    message_queue_id = msgget(key, 0666 | IPC_CREAT);
    
    struct message_buffer message;
    FILE *file;
    
    message.message_type = 1;
    
    // Check if file exists
    if (access(src_filename, F_OK) == -1)
    {
        printf("Error: Source file does not exist.\n");
        strcpy(message.message_text, "ERROR_FILE_NOT_FOUND");
        msgsnd(message_queue_id, &message, strlen(message.message_text) + 1, 0);
        return 1;
    }
    
    // Check if file is readable
    if (access(src_filename, R_OK) == -1)
    {
        printf("Error: Source file is not readable.\n");
        strcpy(message.message_text, "ERROR_FILE_NOT_READABLE");
        msgsnd(message_queue_id, &message, strlen(message.message_text) + 1, 0);
        return 1;
    }
    
    file = fopen(src_filename, "r");
    if (file == NULL)
    {
        printf("Error: Cannot open source file.\n");
        strcpy(message.message_text, "ERROR_FILE_OPEN");
        msgsnd(message_queue_id, &message, strlen(message.message_text) + 1, 0);
        return 1;
    }
    
    // Read and send data line by line (Only dynamic size sent)
    while (fgets(message.message_text, sizeof(message.message_text), file) != NULL)
    {
        msgsnd(message_queue_id, &message, strlen(message.message_text) + 1, 0);
    }
    
    // Send EOF signal
    strcpy(message.message_text, "EOF");
    msgsnd(message_queue_id, &message, strlen(message.message_text) + 1, 0);
    
    fclose(file);
    printf("Producer sent file data.\n");
    
    return 0;
}

[24bcs170@mepcolinux MQ]$cat consumer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

struct message_buffer
{
    long message_type;
    char message_text[1024];
};

int main()
{
    key_t key;
    int message_queue_id;
    char dest_filename[256];
    
    printf("Enter destination filename: ");
    scanf("%s", dest_filename);
    
    key = ftok(".", 66);
    message_queue_id = msgget(key, 0666 | IPC_CREAT);
    
    struct message_buffer message;
    FILE *file;
    
    file = fopen(dest_filename, "w");
    if (file == NULL)
    {
        printf("Consumer Error: Cannot create destination file.\n");
        return 1;
    }
    
    printf("Waiting for producer to send data...\n");
    
    while (1)
    {
        // Dynamic reading using full buffer limit
        msgrcv(message_queue_id, &message, sizeof(message.message_text), 1, 0);
        
        // Error handling from producer side
        if (strcmp(message.message_text, "ERROR_FILE_NOT_FOUND") == 0 ||
            strcmp(message.message_text, "ERROR_FILE_NOT_READABLE") == 0 ||
            strcmp(message.message_text, "ERROR_FILE_OPEN") == 0)
        {
            printf("Consumer stopping: Producer encountered an error.\n");
            fclose(file);
            remove(dest_filename);
            msgctl(message_queue_id, IPC_RMID, NULL);
            return 1;
        }
        
        // Stop if End of File reached
        if (strcmp(message.message_text, "EOF") == 0)
        {
            break;
        }
        
        fputs(message.message_text, file);
    }
    
    fclose(file);
    printf("Consumer saved data to %s.\n", dest_filename);
    
    // Delete queue from system
    msgctl(message_queue_id, IPC_RMID, NULL);
    
    return 0;
}

[24bcs170@mepcolinux MQ]$./producer
Enter source filename: srcfile.txt
Producer sent file data.
[24bcs170@mepcolinux MQ]$./consumer
Enter destination filename: dstfile.txt
Waiting for producer to send data...
Consumer saved data to dstfile.txt.
[24bcs170@mepcolinux MQ]$cat dstfile.txt
"Hi Harish, Message Queue working perfectly!"
