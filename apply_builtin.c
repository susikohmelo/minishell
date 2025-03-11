/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_builtin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 18:18:28 by lfiestas          #+#    #+#             */
/*   Updated: 2025/01/07 14:10:39 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>

// [ FUNCTIONS IN THIS FILE ARe THE ACTUAL APPLICATION OF BUILTIN COMMANDS ]

static void	apply_unset(t_executor *exe, char **argv)
{
	size_t	i;

	while (*++argv != NULL)
	{
		if (!ft_isalpha((*argv)[0]) && (*argv)[0] != '_')
			continue ;
		i = 0;
		while ((*argv)[++i] != '\0')
			if (!ft_isalnum((*argv)[0]) && (*argv)[0] != '_')
				continue ;
		i = find_env_var((char **)exe->envp.ps, *argv);
		if (i == (size_t) - 1)
			continue ;
		free(exe->envp.ps[i]);
		ft_memmove(exe->envp.ps + i, exe->envp.ps + i + 1, \
			sizeof exe->envp.ps[0] * (exe->envp.length - i));
		exe->envp.length--;
	}
}

void	apply_export(t_executor *e, char **argv)
{
	size_t	i;
	size_t	eq;
	bool	append;
	t_array	a;

	while (*++argv != NULL)
	{
		if (!check_export_identifier(*argv) || !ft_strchr(*argv, '='))
			continue ;
		eq = ft_strchr(*argv, '=') - *argv;
		append = (*argv)[eq - 1] == '+';
		a = arr_new(sizeof(char), ft_strlen(*argv) + sizeof"", NULL);
		arr_concat(&a, *argv, eq - append);
		i = find_env_var((char **)e->envp.ps, a.cs);
		arr_push(&a, "=");
		if (append && i != (size_t) - 1)
			arr_concat(&a, e->envp.ps[i] + eq, ft_strlen(e->envp.ps[i]) - eq);
		arr_concat(&a, *argv + eq + 1, ft_strlen(*argv) - (eq + 1));
		if (i != (size_t) - 1)
			(free(e->envp.ps[i]), e->envp.ps[i] = a.cs);
		else
			arr_push(&e->envp, &a.cs);
	}
}

static void	apply_cd(t_executor *ex, char **argv)
{
	char	*old_cwd;
	char	*new_cwd;
	char	*buf;

	if ((!argv[1] && find_env_var((char **)ex->envp.ps, "HOME") == (size_t) - 1)
		|| argv[2] != NULL)
		return ;
	if (argv[1] == NULL)
		argv[1] = ex->envp.ps[find_env_var((char **)ex->envp.ps, "HOME")] + 5;
	old_cwd = getcwd((char [PATH_MAX]){}, PATH_MAX);
	if (old_cwd == NULL)
		old_cwd = ex->envp.ps[find_env_var((char **)ex->envp.ps, "PWD")];
	if (old_cwd == NULL || chdir(argv[1]) == -1)
		return ;
	new_cwd = getcwd(NULL, 0);
	if (new_cwd == NULL)
		return ;
	buf = ft_arena_alloc(\
		&ex->arena, sizeof"OLDPWD=" + ft_strlen(old_cwd) + ft_strlen(new_cwd));
	ft_memcpy(buf, "OLDPWD=", sizeof"OLDPWD=");
	ft_memcpy(buf + ft_strlen("OLDPWD="), old_cwd, ft_strlen(old_cwd) + 1);
	apply_export(ex, (char *[3]){NULL, buf});
	ft_memcpy(buf + ft_strlen("OLDPWD="), new_cwd, ft_strlen(new_cwd) + 1);
	apply_export(ex, (char *[3]){NULL, buf + ft_strlen("OLD")});
	free(new_cwd);
}

static void	apply_exit(t_executor *exe, char **argv)
{
	size_t	i;

	i = 0;
	while (i < exe->cmds_length && exe->cmds[i].pid != -1)
		kill(SIGKILL, exe->cmds[i++].pid);
	ft_putendl_fd("exit", STDOUT_FILENO);
	if (argv[1] == NULL || argv[2] == NULL)
	{
		close(exe->duped_stdin_fd);
		i = (size_t) - 1;
		while (argv[1] != NULL && argv[1][++i])
		{
			if (argv[1][i] != ' ' && !ft_isdigit(argv[1][i]))
			{
				ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
				ft_putstr_fd(argv[1], STDERR_FILENO);
				ft_putendl_fd(": numeric argument required", STDERR_FILENO);
				executor_exit(exe, 2);
			}
		}
		if (argv[1] != NULL)
			executor_exit(exe, builtin_exit_atoi(argv[1]));
		executor_exit(exe, 0);
	}
	ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
}

void	apply_builtin(t_executor *exe, char **argv)
{
	if (argv[0] == 0)
		return ;
	if (ft_strcmp(argv[0], "cd") == 0)
		apply_cd(exe, argv);
	else if (ft_strcmp(argv[0], "export") == 0)
		apply_export(exe, argv);
	else if (ft_strcmp(argv[0], "unset") == 0)
		apply_unset(exe, argv);
	else if (ft_strcmp(argv[0], "exit") == 0)
		apply_exit(exe, argv);
}
