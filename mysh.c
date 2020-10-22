#include "mysh.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int redir_fd[2]; /* redir_fd[0] input, redir_fd[1] output */
    int pipe_fd[2], pipe_pos;
    int cmd_siz;
    int i;
    char *cmd, **args;

    while(1)
    {
        prompt(*argv);
        cmd = read_cmd();
        if(cmd == NULL)
            continue;

        cmd_siz = cnt_tok(cmd);
        if(!cmd_siz)
        {
            free(cmd);
            continue;
        }
        args = tok_cmd(cmd, cmd_siz);

        if(!create_child())
        {
            pipe_pos = chk_pipe(args);
            /* Have piping */
            if(pipe_pos)
            {
                if(pipe(pipe_fd) == -1)
                {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
                printf("pipe_fd[0] = %d\npipe_fd[1] = %d\n", pipe_fd[0], pipe_fd[1]);

                if(!create_child())
                {
                    close(pipe_fd[0]);
                    redir_fd[1] = 1;
                    do_redir(args, pipe_pos, redir_fd);
                    dup2(pipe_fd[1], redir_fd[1]);
                    close(pipe_fd[1]);
                    exec_cmd(split_cmd(args, pipe_pos));
                }

                close(pipe_fd[1]);
                redir_fd[0] = 0;
                do_redir(args+(pipe_pos+1), cmd_siz-pipe_pos-1, redir_fd);
                dup2(pipe_fd[0], redir_fd[0]);
                close(pipe_fd[0]);
                exec_cmd(split_cmd(args+(pipe_pos+1), cmd_siz-pipe_pos-1));
            }
            /* No piping */
            else
            {
                /* No redirection */
                if(!do_redir(args, cmd_siz, redir_fd))
                    exec_cmd(args);
                /* Have redirection */
                else
                    exec_cmd(split_cmd(args, cmd_siz));
            }
        }

        free(cmd);
        for(i = 0; i < cmd_siz; i++)
            free(args[i]);
        free(args);
    }
    return 0;
}
