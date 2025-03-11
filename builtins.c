/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 17:27:12 by lfiestas          #+#    #+#             */
/*   Updated: 2025/01/07 14:37:08 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>

// [ THE FUNCTIONS IN THIS FILE PRINT THE BUILTIN CMD'S OUTPUT ]

void	echo(t_executor *exe, char **argv)
{
	bool	print_newline;
	bool	skip;
	size_t	i;
	size_t	j;

	print_newline = true;
	skip = false;
	i = 0;
	while (argv[++i] != NULL)
	{
		j = 1;
		while (!skip && argv[i][0] == '-' && argv[i][j] == 'n')
			++j;
		if (!skip && argv[i][0] == '-' && argv[i][j] == '\0')
		{
			print_newline = false;
			continue ;
		}
		skip = true;
		ft_putstr_fd(argv[i], STDOUT_FILENO);
		write(STDOUT_FILENO, " ", argv[i + 1] != NULL);
	}
	write(STDOUT_FILENO, "\n", print_newline);
	executor_exit(exe, 0);
}

void	cd(t_executor *exe, char **argv)
{
	if (argv[1] == NULL)
		executor_exit(exe, 0);
	if (argv[2] != NULL)
	{
		ft_putendl_fd("minishell: cd: too many arguments", STDERR_FILENO);
		executor_exit(exe, 1);
	}
	if (chdir(argv[1]) == -1)
	{
		ft_putstr_fd("cd: ", STDERR_FILENO);
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd(argv[1], STDERR_FILENO);
		executor_exit(exe, 1);
	}
	executor_exit(exe, 0);
}

void	pwd(t_executor *exe)
{
	char	*buf;

	buf = getcwd(NULL, 0);
	if (buf == NULL)
	{
		perror("pwd");
		executor_exit(exe, 1);
	}
	ft_putendl_fd(buf, STDOUT_FILENO);
	free(buf);
	executor_exit(exe, 0);
}

void	export(t_executor *exe, char **argv, char **envp)
{
	int	exit_status;

	if (argv[1] == NULL)
		env(exe, envp);
	exit_status = 0;
	while (*++argv != NULL)
	{
		if (!check_export_identifier(*argv))
		{
			ft_putstr_fd("export: `", STDERR_FILENO);
			ft_putstr_fd(*argv, STDERR_FILENO);
			ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
			exit_status = 1;
		}
	}
	executor_exit(exe, exit_status);
}

void	env(t_executor *exe, char **envp)
{
	while (*envp != NULL)
		ft_putendl_fd(*envp++, STDOUT_FILENO);
	executor_exit(exe, 0);
}
