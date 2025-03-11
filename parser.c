/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 16:25:32 by lfiestas          #+#    #+#             */
/*   Updated: 2025/01/07 16:15:33 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>

static void	init_parsing(t_executor *exe, bool *has_error)
{
	size_t	i;
	t_array	cmds;
	t_cmd	temp;

	*has_error = false;
	cmds = arr_new(sizeof(t_cmd), 1, &exe->arena);
	i = (size_t) - 1;
	while (++i < exe->tokens_length)
	{
		if (exe->tokens[i] != '|'
			&& exe->tokens[i] != '|' << 2 && exe->tokens[i] != '&' << 2)
			continue ;
		temp = (t_cmd){.type = exe->tokens[i], .pid = -1};
		temp.files = arr_new(sizeof(t_file), 2, &exe->arena);
		arr_push(&cmds, &temp);
	}
	temp = (t_cmd){.pid = -1, .files = arr_new(sizeof(t_file), 2, &exe->arena)};
	arr_push(&cmds, &temp);
	exe->cmds = cmds.ms;
}

static bool	parse_redirection(
	t_executor *exe, t_cmd *cmd, t_parser_indices *i, t_token token)
{
	t_file	file;

	if (token != '<' && token != '>' && token != '<' << 2 && token != '>' << 2)
		return (false);
	file = (t_file){
		.path = exe->token_strs.ps[i->token_strs++],
		.type = token,
		.fd = -1,
	};
	arr_push(&cmd->files, &file);
	return (true);
}

static void	parse_subshell(t_executor *e, t_array *argv, t_parser_indices *i)
{
	int		pipe_fds[2];
	char	*pipe_str;

	if (pipe(pipe_fds) == -1)
		ft_putendl_fd("minishell: subshell pipe failed", STDERR_FILENO);
	pipe_str = ft_arena_alloc(&e->arena, 16);
	pipe_str[0] = -1;
	ft_i_to_str(pipe_str + 1, pipe_fds[0]);
	ft_putendl_fd(e->token_strs.ps[i->token_strs++], pipe_fds[1]);
	close(pipe_fds[1]);
	arr_push(argv, &e->minishell_path);
	arr_push(argv, &pipe_str);
	arr_push(&e->garbage_fds, &pipe_fds[0]);
	argv->ps[0] = e->minishell_path;
}

static bool	parse_cmd(t_executor *e, t_parser_indices *i)
{
	t_array	argv;

	argv = arr_new(sizeof(char *), 4, &e->arena);
	while (e->tokens[i->tokens] != 0 && e->tokens[i->tokens] != '|'
		&& e->tokens[i->tokens] != '|' << 2 && e->tokens[i->tokens] != '&' << 2)
	{
		if (parse_redirection(e, &e->cmds[i->cmds], i, e->tokens[i->tokens]))
		{
			if (e->tokens[++i->tokens] != '"')
				return (false);
		}
		else if (e->tokens[i->tokens] == '(')
			parse_subshell(e, &argv, i);
		else if (e->tokens[i->tokens] == ')')
			return (i->tokens++, false);
		else if (e->tokens[i->tokens] == '"')
			arr_push(&argv, &e->token_strs.ps[i->token_strs++]);
		i->tokens++;
	}
	if (argv.ps[0] == e->minishell_path && argv.length != 2)
		return (false);
	if (e->tokens[i->tokens] != 0)
		i->tokens++;
	e->cmds[i->cmds++].argv = (char **)argv.ps;
	return (argv.ps[0] || e->cmds[i->cmds - 1].files.length);
}

// [ PARSE THROUH LEXED TOKEN ARRAY & PUT TOGETHER INSTRUCTIONS TO RUN CMDS ]
bool	parse(t_executor *e)
{
	t_parser_indices	i;
	bool				has_error;

	i = (t_parser_indices){0};
	init_parsing(e, &has_error);
	while (!has_error && e->tokens[i.tokens] != '\0')
		if (!parse_cmd(e, &i))
			has_error = true;
	e->cmds_length = i.cmds;
	if (has_error || (e->cmds_length && e->cmds[e->cmds_length - 1].type))
	{
		has_error = true;
		ft_putstr_fd("minishell: parse error near `", 2);
		if (e->tokens[i.tokens - 1] == '"')
			ft_putstr_fd(e->token_strs.ps[i.token_strs - 1], STDERR_FILENO);
		else if (e->tokens[i.tokens - 1] <= CHAR_MAX)
			ft_putchar_fd(e->tokens[i.tokens - 1], STDERR_FILENO);
		else
			ft_putstr_fd((char [3]){
				e->tokens[i.tokens - 1] >> 2, e->tokens[i.tokens - 1] >> 2}, 2);
		ft_putendl_fd("'", STDERR_FILENO);
		e->exit_status = 1;
	}
	return (!has_error);
}
