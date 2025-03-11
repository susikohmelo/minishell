/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljylhank <ljylhank@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 20:39:39 by ljylhank          #+#    #+#             */
/*   Updated: 2025/01/07 15:08:49 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "pipes_redirects.h"

void	pipexit(t_executor *exe, char *str, char *str2, int exit_status)
{
	int	i;

	if (exit_status < 0)
		perror(str);
	else
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(str, STDERR_FILENO);
		if (str2)
		{
			ft_putstr_fd(str2, STDERR_FILENO);
			i = ft_strlen(str2);
			if (i != 0)
				if (str2[i - 1] != '\n')
					ft_putchar_fd('\n', STDERR_FILENO);
			if (exit_status < 0)
				exit_status = -exit_status;
			executor_exit(exe, exit_status);
		}
		i = ft_strlen(str);
		if (i != 0)
			if (str[i - 1] != '\n')
				ft_putchar_fd('\n', STDERR_FILENO);
	}
	executor_exit(exe, (1 - 2 * (exit_status < 0)) * exit_status);
}

static char	*append_paths(t_executor *exe, char *path1, char *path2)
{
	char	*new_path;
	int		i;
	int		i2;

	i = ft_strlen(path1);
	new_path = malloc((i + ft_strlen(path2) + 2) * sizeof(char));
	if (!new_path)
		pipexit(exe, "path appending failed", 0, 1);
	i = 0;
	while (path1[i])
	{
		new_path[i] = path1[i];
		i++;
	}
	new_path[i++] = '/';
	i2 = 0;
	while (path2[i2])
		new_path[i++] = path2[i2++];
	new_path[i] = '\0';
	return (new_path);
}

static char	**get_all_paths(t_executor *exe, char **envp, char *keyword)
{
	char	**all_paths;
	int		found;
	int		index;
	int		i;

	index = 0;
	found = 0;
	while (envp[index] && !found)
	{
		i = 0;
		while (envp[index][i] == keyword[i] && envp[index][i] && keyword[i])
		{
			if (keyword[i + 1] == '\0')
				found = 1;
			i++;
		}
		if (!found)
			index++;
	}
	if (!found)
		return (NULL);
	all_paths = ft_split(envp[index] + i, ':');
	if (!all_paths)
		pipexit(exe, "separating env paths failed", 0, 1);
	return (all_paths);
}

static char	*get_cmnd_path(t_executor *exe, char *cmd, char **envp)
{
	char	**all_paths;
	char	*appended_path;
	int		index;

	index = 0;
	all_paths = get_all_paths(exe, envp, "PATH=");
	while (all_paths && all_paths[index])
	{
		appended_path = append_paths(exe, all_paths[index], cmd);
		if (access(appended_path, F_OK) == 0)
		{
			if (access(appended_path, X_OK) == 0)
			{
				free_strarr(all_paths);
				return (appended_path);
			}
			else
				pipexit(exe, "no permissions to execute command", 0, -126);
		}
		free(appended_path);
		index++;
	}
	pipexit(exe, "command not found: ", cmd, 127);
	return (0);
}

void	execute_cmnd(t_executor *exe, char **argv, char **envp, bool file_error)
{
	char	*command_path;
	char	*_;

	if (argv[0] == NULL || file_error)
		executor_exit(exe, file_error);
	execute_builtin(exe, argv, envp);
	if ((argv[0][0] == '/' || (argv[0][0] == '.' && argv[0][1] == '/'))
			&& access(argv[0], F_OK) == 0)
	{
		if (access(argv[0], X_OK) == 0)
			command_path = argv[0];
		else
			pipexit(exe, "no permissions to execute command", 0, -1);
	}
	else if (argv[0][0] != '/' && argv[0][0] != '.' && argv[0][1] != '/')
		command_path = get_cmnd_path(exe, argv[0], envp);
	else
		pipexit(exe, "command not found: ", argv[0], 127);
	_ = ft_strjoin("_=", argv[0]);
	if (!_)
		pipexit(exe, "malloc", 0, -1);
	apply_export(exe, (char *[3]){NULL, _});
	free(_);
	if (execve(command_path, argv, envp) < 0)
		pipexit(exe, "executing command failed", 0, 126);
}
