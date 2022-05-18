/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnemor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 13:50:32 by lnemor            #+#    #+#             */
/*   Updated: 2022/05/12 14:10:41 by lnemor           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
void ft_putstr(char *s)
{
	int i = 0;

	while (s[i])
	{
		write(2, &s[i], 1);
		i++;
	}
}

void exec(char **av, char **env, int in, int out)
{
	if (av[0] && strcmp(av[0], "cd"))
	{
		int pid = fork();
		if (pid == 0)
		{
			if (in != 0)
			{
				dup2(in, 0);
				close(out);
			}
			if (out != 1)
			{
				dup2(out, 1);
				close(out);
			}
			execve(av[0], &av[0], env);
			ft_putstr("error: cannot execute ");
			ft_putstr(av[0]);
			ft_putstr("\n");
			exit (-1);
		}
	}
	else if (av[0])
	{
		if (!av[1] || (av[1] && av[2]))
			ft_putstr("error: cd: bad arguments\n");
		else if (chdir(av[1]) == -1)
		{	
			ft_putstr("error: cd: cannot change directory to ");
			ft_putstr(av[1]);
			ft_putstr("\n");
		}
	}
}

int main(int argc, char **argv, char **env)
{
	int i = 0;
	int j = 0;
	int fdsave = 0;
	int fd[2];

	argv = &argv[1];
	while (argv[i])
	{
		if (!strcmp(argv[i], "|"))
		{
			argv[i] = NULL;
			pipe(fd);
			exec(&argv[j], env, fdsave, fd[1]);
			close (fd[1]);
			if (fdsave)
				close (fdsave);
			fdsave = fd[0];
			i++;
			j = i;
		}
		else if (!strcmp(argv[i], ";"))
		{
			argv[i] = NULL;
			exec(&argv[j], env, fdsave, 1);
			while (waitpid(-1, NULL, 0) != -1)
				;
			if (fdsave)
				close(fdsave);
			fdsave = 0;
			i++;
			j = i;
		}
		else
			i++;
	}
	if (i > 0 && argv[i - 1] != NULL)
		exec(&argv[j], env, fdsave, 1);
	if (fdsave)
		close(fdsave);
	while (waitpid(-1, NULL, 0) != -1)
		;
	return (0);

}
