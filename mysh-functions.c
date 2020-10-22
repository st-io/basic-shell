#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define BUFSIZE 256

#define DEBUG 0

const char* DEL = " \t";

void prompt(char *args)
{
    printf("%s>", args);
}

char *read_cmd(void)
{
    int c, pos;
    char *cmd_buffer;
    
    cmd_buffer = malloc(BUFSIZE);
    if(cmd_buffer == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    pos = 0;
    while(c = getchar())
    {
        if(pos > (BUFSIZE - 1))
        {
            printf("Error: Input larger than %d characters\n", BUFSIZE-1);
            while(getchar() != '\n');
            free(cmd_buffer);
            return NULL;
        }

        if(c == '\n')
        {
            cmd_buffer[pos] = '\0';
            break;
        }

        if(c == EOF)
        {
            printf("\n");
            free(cmd_buffer);
            exit(EXIT_SUCCESS);
        }

        cmd_buffer[pos++] = c;
    }

    return cmd_buffer;
}

int create_child(void)
{
    pid_t pid, wait_pid;
    int status;

    pid = fork();

    if(pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(!pid)
        return pid;
    else
    {
        if((wait_pid = wait(&status)) == -1)
        {
            perror("wait");
            exit(EXIT_FAILURE);
        }

        return pid;
    }
}

void exec_cmd(char **cmd)
{
    if(execvp(*cmd, cmd) == -1)
    {
        perror(*cmd);
        exit(EXIT_FAILURE);
    }
}

int cnt_tok(char *cmd)
{
    int tokens;
    char buffer[strlen(cmd)+1];

    strcpy(buffer, cmd);

    if(strtok(buffer, DEL) == NULL)
        return 0;

    tokens = 1;
    while(strtok(NULL, DEL) != NULL)
        tokens++;

    return tokens;
}

char **tok_cmd(char *cmd, int tokens)
{
    int i;
    char *tmp_tok;
    char **args_buffer;

    tmp_tok = strtok(cmd, DEL);
    if(tmp_tok == NULL)
        return NULL;

    args_buffer = malloc( sizeof(char*) * (tokens + 1) );
    if(args_buffer == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < tokens; i++)
    {
        args_buffer[i] = malloc( (int)strlen(tmp_tok) + 1 );
        if(args_buffer[i] == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strcpy(args_buffer[i], tmp_tok);
        tmp_tok = strtok(NULL, DEL);
    }
    args_buffer[i] = NULL;

    /* DEBUG */
    if(DEBUG)
    {
        printf("--------------------\n");
        printf("in tok_cmd\nargs: %d\n", tokens);
        for(i = 0; i < tokens; i++)
            printf("%d - %s\n", i, args_buffer[i]);
        printf("--------------------\n");
    }
    /* DEBUG */

    return args_buffer;
}

char **split_cmd(char **args, int len)
{
    int i;
    char **args_buffer;

    args_buffer = malloc(sizeof(char*) * (len + 1));
    if(args_buffer == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < len; i++)
    {
        if(*args[i] == '>' || *args[i] == '<' || *args[i] == '|')
            args_buffer[i] = NULL;
        else
        {
            args_buffer[i] = malloc(strlen(args[i]) + 1);
            if(args_buffer[i] == NULL)
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            strcpy(args_buffer[i], args[i]);
        }
    }
    args_buffer[i] = NULL;

    /* DEBUG */
    if(DEBUG)
    {
        printf("--------------------\n");
        printf("in split_args\nargs: %d\n", len);
        for(i = 0; i < (len+1); i++)
            printf("%d - %s\n", i, args_buffer[i]);
        printf("--------------------\n");
    }
    /* DEBUG */

    return args_buffer;
}

int do_redir(char **args, int len, int *fd)
{
    int i, flag;
    flag = 0;
    for(i = 0; i < len; i++)
    {
        /* stdin */
        if(!strcmp(args[i], "<"))
        {
            fd[0]  = open(args[i + 1], O_RDONLY);
            printf("fd[0] = %d\n", fd[0]);
            dup2(fd[0], 0);
            flag = 1;
        }
        /* stdout */
        else if(!strcmp(args[i], ">") || !strcmp(args[i], ">>"))
        {
            /* append */
            if(args[i][1] == '>')
                fd[1] = open(args[i + 1], O_CREAT|O_WRONLY|O_APPEND,
                        S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
            /* truncate */
            else
                fd[1] = creat(args[i + 1], S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
            printf("fd[1] = %d\n", fd[1]);
            dup2(fd[1], 1);
            flag = 1;
        }
    }
    return flag;
}

int chk_pipe(char **args)
{
    int i;

    for(i = 0; args[i] != NULL; i++)
        if(!strcmp(args[i], "|"))
            return i;
    return 0;
}

