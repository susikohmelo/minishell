/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 15:32:02 by lfiestas          #+#    #+#             */
/*   Updated: 2025/01/08 11:56:37 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "get_next_line.h"
#include "pipes_redirects.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

// [ GET/SET INPUT FILE ]
// This is stdin or a .sh script depending on the arguments
static bool	set_input_file(t_executor *exe, int argc, char **argv)
{
	exe->script_fd = -1;
	if (argc >= 2 && argv[1][0] == -1)
		exe->script_fd = ft_atoi(argv[1] + 1);
	else if (argc >= 2)
	{
		exe->script_fd = open(argv[1], O_RDONLY);
		if (exe->script_fd == -1)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(strerror(errno), STDERR_FILENO);
			ft_putstr_fd(": ", STDERR_FILENO);
			ft_putendl_fd(argv[1], STDERR_FILENO);
			return (false);
		}
	}
	else if (!isatty(STDIN_FILENO))
		exe->script_fd = STDIN_FILENO;
	return (true);
}

// [ BASIC LOGIC OF THE MAIN FUNCTION ]
// 1. Initialize executor - (struct that has basic info about cmds to execute)
// 2. Set input - we want to know if we read from stdin or a .sh file
// 3. On loop: readline, parse & execute it, clean up memory
int	main(int argc, char **argv, char **envp)
{
	t_executor	exe;

	executor_init(&exe, argv[0], envp);
	if (!set_input_file(&exe, argc, argv))
		executor_exit(&exe, 1);
	while (1)
	{
		if (exe.script_fd != -1)
			exe.line = get_next_line(exe.script_fd);
		else
			exe.line = readline("minishell> ");
		if (exe.line == NULL)
			break ;
		if (exe.line[0] != '\0')
			add_history(exe.line);
		lex(&exe, exe.line);
		if (parse(&exe))
		{
			run_commands(&exe);
			dup2(exe.duped_stdin_fd, STDIN_FILENO);
			close(exe.duped_stdin_fd);
		}
		executor_reset(&exe);
	}
	executor_exit(&exe, exe.exit_status);
}
