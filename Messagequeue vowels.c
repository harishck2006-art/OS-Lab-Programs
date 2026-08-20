[?1034h[24bcs170@mepcolinux MQ]$cat P1.c
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct message_buffer
{
    long message_type;
    char message_text[1024];
};

int main()
{
    key_t key;
    int message_queue_id;
    pid_t pid;
    
    key = ftok(".", 65);
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    message_queue_id = msgget(key, 0666 | IPC_CREAT);
    if (message_queue_id == -1) {
        perror("msgget failed");
        exit(1);
    }
    
    pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }
    else if (pid > 0) 
    {
        struct message_buffer message;
        message.message_type = 1;
        
        printf("Enter message: ");
        fgets(message.message_text, sizeof(message.message_text), stdin);
        
        if (msgsnd(message_queue_id, &message, strlen(message.message_text) + 1, 0) == -1) {
            perror("msgsnd failed");
            exit(1);
        }
        printf("Producer sent: %s", message.message_text);
        
        wait(NULL);
        msgctl(message_queue_id, IPC_RMID, NULL);
    }
    else if (pid == 0) 
    {
        struct message_buffer message;
        int words = 0;
        int i = 0;
        int in_word = 0;
        
        if (msgrcv(message_queue_id, &message, sizeof(message.message_text), 1, 0) == -1) {
            perror("msgrcv failed");
            exit(1);
        }
        
        while (message.message_text[i] != '\0')
        {
            if (message.message_text[i] == ' ' || message.message_text[i] == '\n' || message.message_text[i] == '\t')
            {
                in_word = 0;
            }
            else if (in_word == 0)
            {
                in_word = 1;
                words++;
            }
            i++;
        }
        
        printf("Consumer calculated words: %d\n", words);
        exit(0);
    }
    
    return 0;
}

[24bcs170@mepcolinux MQ]$gcc P1.c
[24bcs170@mepcolinux MQ]$./P1
Enter message: hello i am hearish
Producer sent: hello i am hearish
Consumer calculated words: 4
