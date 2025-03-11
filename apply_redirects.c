/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_redirects.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljylhank <ljylhank@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 17:48:56 by ljylhank          #+#    #+#             */
/*   Updated: 2025/01/07 17:13:32 by ljylhank         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipes_redirects.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

int	open_this_file(t_executor *exe, t_file file)
{
	int	r;

	if (file.type == 0)
		return (-2);
	if (file.type == '<')
		r = open(file.path, O_RDONLY);
	else if (file.type == '<' << 2)
		r = get_delimiter_input(exe, file.delimiter);
	else if (file.type == '>')
		r = open(file.path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	else
		r = open(file.path, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (r < 0)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd(file.path, STDERR_FILENO);
		return (-1);
	}
	arr_push(&exe->garbage_fds, &r);
	return (r);
}

static void	get_last(t_cmd *cmd, t_file *last_file, bool is_input)
{
	size_t	i;

	i = 0;
	last_file->fd = -42;
	while (i < cmd->files.length && is_input)
	{
		if (cmd->files.fs[i].type == '<'
			|| cmd->files.fs[i].type == '<' << 2)
			if (cmd->files.fs[i].fd != -2)
				*last_file = cmd->files.fs[i];
		++i;
	}
	while (i < cmd->files.length && !is_input)
	{
		if (cmd->files.fs[i].type == '>'
			|| cmd->files.fs[i].type == '>' << 2)
			if (cmd->files.fs[i].fd != -2)
				*last_file = cmd->files.fs[i];
		++i;
	}
}

static void	close_all_but_last(t_cmd *cmds, size_t length)
{
	size_t	i;
	int		file_len;
	int		subi;
	t_file	last_in;
	t_file	last_out;

	i = 0;
	while (i < length)
	{
		subi = 0;
		file_len = cmds[i].files.length;
		get_last(&cmds[i], &last_in, true);
		get_last(&cmds[i], &last_out, false);
		while (subi < file_len)
		{
			if (cmds[i].files.fs[subi].fd != -2
				&& cmds[i].files.fs[subi].fd != last_in.fd
				&& cmds[i].files.fs[subi].fd != last_out.fd)
				close(cmds[i].files.fs[subi].fd);
			++subi;
		}
		++i;
	}
}

bool	get_create_files(t_executor *exe, t_cmd *cmds, size_t length)
{
	size_t	i;
	int		file_len;
	int		subi;
	bool	errors;

	i = (size_t) - 1;
	errors = false;
	while (++i < length && g_signum != SIGINT)
	{
		file_len = cmds[i].files.length;
		subi = -1;
		while (++subi < file_len)
		{
			cmds[i].files.fs[subi].fd = \
				open_this_file(exe, cmds[i].files.fs[subi]);
			if (g_signum == SIGINT || cmds[i].files.fs[subi].fd == -1)
			{
				errors = true;
				cmds[i].file_error = true;
				break ;
			}
		}
	}
	close_all_but_last(cmds, length);
	return (errors);
}

void	apply_redirects(t_cmd *cmd)
{
	t_file	last_in;
	t_file	last_out;

	get_last(cmd, &last_in, true);
	get_last(cmd, &last_out, false);
	if (last_in.fd != -42)
		dup2(last_in.fd, STDIN_FILENO);
	if (last_out.fd != -42)
		dup2(last_out.fd, STDOUT_FILENO);
}
