/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 14:13:49 by lfiestas          #+#    #+#             */
/*   Updated: 2025/01/08 10:53:23 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft/libft.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <signal.h>
#include <unistd.h>

static void	set_minishell_path(t_executor *exe, char *arg0)
{
	char	*cwd;

	if (arg0[0] == '/')
	{
		exe->minishell_path = ft_strdup(arg0);
		return ;
	}
	cwd = getcwd(NULL, 0);
	exe->minishell_path = ft_malloc(ft_strlen(cwd) + ft_strlen(arg0) + 2);
	ft_memcpy(exe->minishell_path, cwd, ft_strlen(cwd));
	exe->minishell_path[ft_strlen(cwd)] = '/';
	ft_memcpy(exe->minishell_path + 1 + ft_strlen(cwd), arg0, ft_strlen(arg0));
	exe->minishell_path[ft_strlen(cwd) + 1 + ft_strlen(arg0)] = '\0';
	free(cwd);
}

void	executor_init(t_executor *exe, char *arg0, char **envp)
{
	size_t				i;
	char				*env_var;
	struct sigaction	act;

	act = (struct sigaction){.sa_handler = SIG_IGN};
	sigaction(SIGQUIT, &act, NULL);
	signal(SIGINT, handle_signals);
	rl_event_hook = redisplay_prompt;
	*exe = (t_executor){};
	exe->token_strs = arr_new(sizeof(char *), 16, &exe->arena);
	exe->garbage_fds = arr_new(sizeof(int), 8, &exe->arena);
	exe->envp = arr_new(sizeof(char *), 128, NULL);
	i = 0;
	while (envp[i] != NULL)
	{
		env_var = ft_strdup(envp[i++]);
		arr_push(&exe->envp, &env_var);
	}
	set_minishell_path(exe, arg0);
}

void	executor_reset(t_executor *exe)
{
	size_t	i;

	i = (size_t) - 1;
	while (++i < exe->garbage_fds.length)
		close(exe->garbage_fds.is[i]);
	free(exe->line);
	ft_arena_rewind(&exe->arena, exe->token_strs.ps);
	*exe = (t_executor){
		.arena = exe->arena,
		.envp = exe->envp,
		.exit_status = exe->exit_status,
		.minishell_path = exe->minishell_path,
		.script_fd = exe->script_fd,
	};
	exe->token_strs = arr_new(sizeof(char *), 16, &exe->arena);
	exe->garbage_fds = arr_new(sizeof(int), 8, &exe->arena);
	if (g_signum == SIGINT)
		ft_putchar_fd('\n', 1);
	g_signum = 0;
}

void	executor_delete(t_executor *exe)
{
	size_t	i;

	i = (size_t) - 1;
	while (++i < exe->garbage_fds.length)
		close(exe->garbage_fds.is[i]);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	if (exe->script_fd != -1)
		close(exe->script_fd);
	i = 0;
	while (i < exe->envp.length)
		free(exe->envp.ps[i++]);
	arr_delete(&exe->envp);
	ft_arena_clear(&exe->arena);
	free(exe->line);
	free(exe->minishell_path);
	rl_clear_history();
}

void	executor_exit(t_executor *exe, int exit_status)
{
	executor_delete(exe);
	exit(exit_status);
}
