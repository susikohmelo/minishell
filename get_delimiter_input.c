/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_delimiter_input.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljylhank <ljylhank@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 16:12:42 by ljylhank          #+#    #+#             */
/*   Updated: 2025/01/07 17:42:25 by ljylhank         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "pipes_redirects.h"
#include <readline/readline.h>

static bool	has_delimiter(char *buffer, char *delimiter)
{
	int	i;

	i = 0;
	while (buffer[i])
	{
		if (!delimiter[i] && buffer[i] == '\n')
			break ;
		if (delimiter[i] != buffer[i])
			return (false);
		++i;
	}
	if (i != (int)ft_strlen(delimiter))
		return (false);
	return (true);
}

static int	check_for_killsignal(void)
{
	if (g_signum == SIGINT)
	{
		rl_done = 1;
	}
	return (0);
}

static void	read_delim(t_executor *exe, char *delimiter, int pipe_fd[2])
{
	char	*buffer;
	char	*processed_buffer;

	while (1)
	{
		buffer = readline("> ");
		if (!buffer || g_signum == SIGINT)
		{
			if (!buffer)
				ft_putendl_fd("minishell: warning: delimited by EOF", 2);
			break ;
		}
		processed_buffer = preprocess(exe, buffer);
		if (has_delimiter(processed_buffer, delimiter))
			break ;
		ft_putendl_fd(processed_buffer, pipe_fd[1]);
		free(buffer);
	}
	free(buffer);
	close(pipe_fd[1]);
}

int	get_delimiter_input(t_executor *exe, char *delimiter)
{
	int		pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (-1);
	arr_push(&exe->garbage_fds, &pipe_fd[0]);
	arr_push(&exe->garbage_fds, &pipe_fd[1]);
	rl_event_hook = check_for_killsignal;
	read_delim(exe, delimiter, pipe_fd);
	rl_event_hook = redisplay_prompt;
	return (pipe_fd[0]);
}
