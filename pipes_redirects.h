/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_redirects.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljylhank <ljylhank@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 17:13:43 by ljylhank          #+#    #+#             */
/*   Updated: 2025/01/07 14:59:20 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPES_REDIRECTS_H
# define PIPES_REDIRECTS_H

# include "minishell.h"
# include <stdio.h>
# include <sys/wait.h>
# include <errno.h>
# include <fcntl.h>
# include <stdlib.h>
# include <unistd.h>

int		open_this_file(t_executor *exe, t_file file);
void	run_commands(t_executor *exe);
int		get_delimiter_input(t_executor *exe, char *delimiter);

void	apply_redirects(t_cmd *cmd);
bool	get_create_files(t_executor *exe, t_cmd *cmds, size_t length);
void	execute_cmnd(t_executor *exe, char **argv, char **envp, bool file_err);
void	pipexit(t_executor *exe, char *str, char *str2, int use_perror);
void	free_strarr(char **strarr);

void	apply_builtin(t_executor *exe, char **argv);
void	execute_builtin(t_executor *exe, char **argv, char **envp);

#endif
