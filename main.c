 /* 
 * File:   main.c
 * Author: Rybin Vladislav
 *
 * Created on September 23, 2015, 7:05 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_LENGTH 255
/*
 * 
 */
int main(int argc, char** argv) 
{
    int pipefd[2];
    pid_t chpid;
    char* word = argv[2];
    
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: %s <filepath> <string>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    if (pipe(pipefd) == -1) 
    {
        perror("pipe");
        return EXIT_FAILURE;
    }
    
    chpid = fork();
    if (chpid == -1) 
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    
    if (chpid == 0) 
    { //Child
        close(pipefd[1]);
        size_t buflen = 0;
        while (read(pipefd[0], &buflen, sizeof(size_t)) > 0) {
            char* buf = (char*)malloc(sizeof(char) * MAX_LENGTH);
            read(pipefd[0], buf, buflen);
            if (strstr(buf, word) != NULL)
            {
                write(STDOUT_FILENO, buf, strlen(buf));
            }
        }
            
        close(pipefd[0]);
        return EXIT_SUCCESS;

    } 
    else 
    {          //Parent
        close(pipefd[0]);
        
        FILE* fp;
        char* line = NULL;
        size_t len;
        ssize_t read;
        
        fp = fopen(argv[1], "r");
        if (fp == NULL) 
        {
            printf("pzdc \n");
            return EXIT_FAILURE;
        }
        
        while ((read = getline(&line, &len, fp)) != -1) 
        {
            size_t lenlen = strlen(line);
            write(pipefd[1], &lenlen, sizeof(size_t));
            write(pipefd[1], line, lenlen);
        }
        
        //kill(chpid, SIGKILL);
        fclose(fp);
        close(pipefd[1]);
        wait(NULL);
        
        
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
