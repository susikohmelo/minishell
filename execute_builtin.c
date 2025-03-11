/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 18:19:57 by lfiestas          #+#    #+#             */
/*   Updated: 2025/01/07 14:34:44 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	find_env_var(char **envp, const char *var)
{
	const size_t	length = ft_strlen(var);
	size_t			i;

	i = 0;
	while (envp[i] != NULL
		&& (ft_strncmp(envp[i], var, length) != 0
			|| envp[i][length] != '='))
		++i;
	if (envp[i] == NULL)
		return ((size_t) - 1);
	return (i);
}

void	execute_builtin(t_executor *exe, char **argv, char **envp)
{
	if (ft_strcmp(argv[0], "echo") == 0)
		echo(exe, argv);
	else if (ft_strcmp(argv[0], "cd") == 0)
		cd(exe, argv);
	else if (ft_strcmp(argv[0], "pwd") == 0)
		pwd(exe);
	else if (ft_strcmp(argv[0], "export") == 0)
		export(exe, argv, envp);
	else if (ft_strcmp(argv[0], "unset") == 0)
		executor_exit(exe, 0);
	else if (ft_strcmp(argv[0], "env") == 0)
		env(exe, envp);
	else if (ft_strcmp(argv[0], "exit") == 0)
		executor_exit(exe, 0);
}

bool	check_export_identifier(char *arg)
{
	if (!ft_isalpha(*arg) && *arg != '_')
		return (false);
	++arg;
	while (*arg != '=' && *arg != '+' && *arg != '\0')
	{
		if (!ft_isalnum(*arg) && *arg != '_')
			return (false);
		++arg;
	}
	if (arg[0] == '+' && arg[1] != '=')
		return (false);
	return (true);
}
