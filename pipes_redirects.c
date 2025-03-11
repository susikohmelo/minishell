/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_redirects.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljylhank <ljylhank@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 16:46:06 by ljylhank          #+#    #+#             */
/*   Updated: 2025/01/08 11:54:42 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipes_redirects.h"

void	free_strarr(char **strarr)
{
	int	index;

	index = 0;
	while (strarr[index])
		free(strarr[index++]);
	free(strarr[index]);
	free(strarr);
}

static void	execute_without_pipe(t_executor *exe, t_cmd *cmd, char **envp)
{
	cmd->pid = fork();
	if (cmd->pid < 0)
		pipexit(exe, "forking failed", 0, 1);
	if (cmd->pid == 0)
	{
		close(exe->duped_stdin_fd);
		apply_redirects(cmd);
		execute_cmnd(exe, cmd->argv, envp, cmd->file_error);
	}
}

static void	execute_and_pipe(t_executor *exe, t_cmd *cmd, char **envp)
{
	int		pipe_fd[2];

	if (pipe(pipe_fd) < 0)
		pipexit(exe, "pipe creation failed", 0, 1);
	arr_push(&exe->garbage_fds, &pipe_fd[0]);
	arr_push(&exe->garbage_fds, &pipe_fd[1]);
	cmd->pid = fork();
	if (cmd->pid < 0)
		pipexit(exe, "forking failed", 0, 1);
	if (cmd->pid == 0)
	{
		close(exe->duped_stdin_fd);
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		apply_redirects(cmd);
		execute_cmnd(exe, cmd->argv, envp, cmd->file_error);
	}
	close(pipe_fd[1]);
	dup2(pipe_fd[0], STDIN_FILENO);
}

static bool	run_pipeline(t_executor *exe, t_cmd *cmds, size_t cmds_length)
{
	size_t	i;

	if (get_create_files(exe, cmds, cmds_length) == true)
	{
		if (g_signum == SIGINT)
		{
			exe->exit_status = 130;
			return (false);
		}
	}
	i = (size_t) - 1;
	while (++i < cmds_length)
	{
		if (cmds[i].type == '|')
			execute_and_pipe(exe, &cmds[i], (char **)exe->envp.ps);
		else
			execute_without_pipe(exe, &cmds[i], (char **)exe->envp.ps);
		apply_builtin(exe, cmds[i].argv);
	}
	i = (size_t) - 1;
	while (++i < cmds_length)
		if (cmds[i].pid != -1)
			waitpid(cmds[i].pid, &exe->exit_status, 0);
	exe->exit_status = WEXITSTATUS(exe->exit_status);
	return (true);
}

void	run_commands(t_executor *e)
{
	size_t	i;
	size_t	length;

	e->duped_stdin_fd = dup(STDIN_FILENO);
	i = 0;
	while (i < e->cmds_length)
	{
		length = 0;
		while (e->cmds[i + length].type != 0
			&& e->cmds[i + length].type != '|' << 2
			&& e->cmds[i + length].type != '&' << 2)
			++length;
		if (!e->skip_pipeline && !run_pipeline(e, e->cmds + i, length + 1))
		{
			g_signum = 0;
			break ;
		}
		if (e->cmds[i + length].type == '&' << 2 && e->exit_status != 0)
			break ;
		if (e->cmds[i + length].type == '&' << 2)
			e->skip_pipeline = false;
		else if (e->cmds[i + length].type == '|' << 2 && e->exit_status == 0)
			e->skip_pipeline = true;
		i += length + 1;
	}
}
