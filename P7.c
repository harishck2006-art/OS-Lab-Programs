#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 100

int main()
{
    setvbuf(stdin, NULL, _IONBF, 0);

    int pipe1[2];   
    int pipe2[2];   

    if (pipe(pipe1) == -1)
    {
        perror("pipe1 failed");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipe2) == -1)
    {
        perror("pipe2 failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        printf("[P1] PID = %d, PPID = %d\n", getpid(), getppid());

        close(pipe1[0]);
        close(pipe2[1]);

        int choice;
        printf("--- MENU ---\n1. Sum of Digits\n2. Reverse the String\nEnter choice (1 or 2): ");
        while (scanf("%d", &choice) != 1)
        {
            printf("Invalid input! Enter 1 or 2: ");
            while (getchar() != '\n');
        }
        getchar();

        write(pipe1[1], &choice, sizeof(int));

        if (choice == 1)
        {
            char p1_digit_string[BUFFER_SIZE];
            printf("[P1] Enter a number to find its sum of digits: ");
            if (fgets(p1_digit_string, BUFFER_SIZE, stdin) != NULL)
            {
                p1_digit_string[strcspn(p1_digit_string, "\n")] = '\0';
            }

            printf("[P1] Sending question to P2: \"Find the sum of digits of %s\"\n", p1_digit_string);
            write(pipe1[1], p1_digit_string, BUFFER_SIZE);

            int sum;
            read(pipe2[0], &sum, sizeof(int));
            printf("[P1] Received response from P2: Sum of digits = %d\n", sum);
        }
        else if (choice == 2)
        {
            char p1_extracted_string[BUFFER_SIZE];
            char p1_inverted_output[BUFFER_SIZE];
            int length, rev_idx, idx;

            read(pipe2[0], p1_extracted_string, BUFFER_SIZE);
            printf("[P1] Received question from P2: \"Reverse the string %s\"\n", p1_extracted_string);

            length = strlen(p1_extracted_string);
            rev_idx = 0;
            for (idx = length - 1; idx >= 0; idx--)
            {
                p1_inverted_output[rev_idx++] = p1_extracted_string[idx];
            }
            p1_inverted_output[rev_idx] = '\0';

            printf("[P1] Calculated reversed string = %s\n", p1_inverted_output);
            printf("[P1] Sending answer back to P2 through pipe1\n");
            write(pipe1[1], p1_inverted_output, BUFFER_SIZE);
        }

        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL);
    }
    else
    {
        printf("[P2] PID = %d, PPID = %d\n", getpid(), getppid());

        close(pipe1[1]);
        close(pipe2[0]);

        int choice;
        read(pipe1[0], &choice, sizeof(int));

        if (choice == 1)
        {
            char p2_digit_string[BUFFER_SIZE];
            int sum = 0;
            int idx;

            read(pipe1[0], p2_digit_string, BUFFER_SIZE);
            printf("[P2] Received question from P1: Find sum of digits of %s\n", p2_digit_string);

            for (idx = 0; p2_digit_string[idx] != '\0'; idx++)
            {
                if (p2_digit_string[idx] >= '0' && p2_digit_string[idx] <= '9')
                {
                    sum += (p2_digit_string[idx] - '0');
                }
            }

            printf("[P2] Calculated sum = %d\n", sum);
            printf("[P2] Sending result back to P1 through pipe2\n");
            write(pipe2[1], &sum, sizeof(int));
        }
        else if (choice == 2)
        {
            char p2_input_string[BUFFER_SIZE];
            char p2_received_answer[BUFFER_SIZE];

            printf("[P2] Enter a string to reverse: ");
            if (fgets(p2_input_string, BUFFER_SIZE, stdin) != NULL)
            {
                p2_input_string[strcspn(p2_input_string, "\n")] = '\0';
            }

            printf("[P2] Sending question to P1: \"Reverse the string %s\"\n", p2_input_string);
            write(pipe2[1], p2_input_string, BUFFER_SIZE);

            read(pipe1[0], p2_received_answer, BUFFER_SIZE);
            printf("[P2] Received response from P1: Reversed string = %s\n", p2_received_answer);
        }

        close(pipe1[0]);
        close(pipe2[1]);
    }

    return 0;
}

[24bcs170@mepcolinux ex2]$cc P2.c
[24bcs170@mepcolinux ex2]$./a.out
[P1] PID = 214401, PPID = 213726
[P2] PID = 214402, PPID = 214401
--- MENU ---
1. Sum of Digits
2. Reverse the String
Enter choice (1 or 2): 1
[P1] Enter a number to find its sum of digits: 1
[P1] Sending question to P2: "Find the sum of digits of 1"
[P2] Received question from P1: Find sum of digits of 1
[P2] Calculated sum = 1
[P2] Sending result back to P1 through pipe2
[P1] Received response from P2: Sum of digits = 1
[24bcs170@mepcolinux ex2]$/a.out
[P1] PID = 214500, PPID = 213726
--- MENU ---
1. Sum of Digits
2. Reverse the String
[P2] PID = 214501, PPID = 214500
Enter choice (1 or 2): 1
[P1] Enter a number to find its sum of digits: 1 2 3 4 5
[P1] Sending question to P2: "Find the sum of digits of 1 2 3 4 5"
[P2] Received question from P1: Find sum of digits of 1 2 3 4 5
[P2] Calculated sum = 15
[P2] Sending result back to P1 through pipe2
[P1] Received response from P2: Sum of digits = 15
[24bcs170@mepcolinux ex2]$./a.out
[P1] PID = 214589, PPID = 213726
--- MENU ---
1. Sum of Digits
2. Reverse the String
Enter choice (1 or 2): [P2] PID = 214590, PPID = 214589
2
[P2] Enter a string to reverse: harish
[P2] Sending question to P1: "Reverse the string harish"
[P1] Received question from P2: "Reverse the string harish"
[P1] Calculated reversed string = hsirah
[P1] Sending answer back to P2 through pipe1
[P2] Received response from P1: Reversed string = hsirah
[24bcs170@mepcolinux ex2]$exit
