#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// malloc, free, write, close, fork, waitpid, signal, kill, exit, chdir, execve, dup, dup2, pipe, strcmp, strncmp

void exec(char **argv, int start, int nullify, char **env, int argc)
{
    char **args = malloc(sizeof(args) * argc);
    int i = start;
    int j = 0;

    argv[nullify] = NULL;
    argv[0] = NULL;
    while (argv[i])
    {
        args[j] = argv[i];
        // printf("args: %s\n", args[j]);
        j++;
        i++;
    }
    args[j] = NULL;
    // printf("args: %s\n", args[i]);
    execve(argv[start], args, env);
    // write(2, "error could not execute\n", 25);
    exit (1);
}
//   STDIN_FILENO 0
//   STDOUT_FILENO 1
int main(int argc, char **argv, char **env)
{
    int i = 1;
    int start = 1;
    int fd[2];

    while (argv[i])
    {
        if (i > argc)
            break;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
		{
			if (strcmp(argv[i], "cd") == 0)
			{
				i++;
				if (!argv[i] || (argv[i+1] && (strcmp(argv[i], ";") == 0 || strcmp(argv[i], "|") == 0)))
				{
					write(2, "cd: bad arguments\n", 19);
					break ;
				}
				else if (chdir(argv[i]) == -1)
				{
					write(2, "cd: could not change direcoty to ", 34);
					write(2, argv[i], strlen(argv[i])); // I assume you can code strlen yourself
					write(2, "\n", 2);
				}
			}
            i++;
		}
        if (i != 1 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
        {
            if(fork() == 0)
                exec(argv, start, i, env, argc);
            i++;
            start = i;
            while (waitpid(-1, NULL, 0) != -1)
                ;
        }
        else if (i != 1 && strcmp(argv[i], "|") == 0)
        {
            pipe(fd);
            if (fork() == 0)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]);
                exec(argv, start, i, env, argc);
            }
            dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
            close(fd[0]);
            i++;
            start = i;
        }
        // printf("argv[%d]: %s\n", i, argv[i]);
    }
}