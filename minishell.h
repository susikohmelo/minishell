/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 11:07:55 by lfiestas          #+#    #+#             */
/*   Updated: 2025/01/08 10:37:08 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <libft.h>
# include <stddef.h>
# include <stdint.h>
# include <stdbool.h>
# include <sys/types.h>
# include <signal.h>

extern sig_atomic_t	g_signum;

enum
{
	WILDCARD = -1
};

typedef uint16_t	t_token;

typedef struct s_file
{
	union
	{
		char	*path;
		char	*delimiter;
	};
	int			fd;
	t_token		type;
}	t_file;

typedef struct s_array
{
	union
	{
		char			*cs;
		int				*is;
		void			**ps;
		t_token			*ts;
		struct s_cmd	*ms;
		t_file			*fs;
		struct s_array	*as;
	};
	size_t				elem_size;
	size_t				length;
	size_t				capacity;
	t_arena				*arena;
}	t_array;

typedef struct s_cmd
{
	char	**argv;
	t_token	type;
	pid_t	pid;
	t_array	files;
	bool	file_error;
}	t_cmd;

// Executor has (nearly) all of the data we need to execute the commands
// and clean up after ourselves before the next loop
typedef struct s_executor
{
	t_arena	arena;
	t_token	*tokens;
	size_t	tokens_length;
	t_array	token_strs;
	t_array	garbage_fds;
	t_array	envp;
	t_cmd	*cmds;
	size_t	cmds_length;
	char	*line;
	char	*minishell_path;
	int		script_fd;
	int		duped_stdin_fd;
	int		exit_status;
	bool	skip_pipeline;
}	t_executor;

typedef struct s_parser_indices
{
	size_t	tokens;
	size_t	token_strs;
	size_t	cmds;
}	t_parser_indices;

char	*preprocess(t_executor *exe, const char *src);
void	lex(t_executor *exe, const char *src);
bool	parse(t_executor *exe);

void	executor_init(t_executor *exec, char *argv0, char **envp);
void	executor_reset(t_executor *exec);
void	executor_delete(t_executor *exec);
void	executor_exit(t_executor *exec, int exit_status);
void	handle_signals(int signum);
int		redisplay_prompt(void);

t_array	arr_new(size_t elem_size, size_t init_capacity, t_arena *optional);
void	arr_delete(t_array *arr);
void	arr_reset(t_array *arr);
void	arr_reserve(t_array *arr, size_t capacity);
void	arr_push(t_array *arr, const void *elem);
void	arr_concat(t_array *dest, const void *src, size_t src_length);

void	echo(t_executor *exe, char **argv);
void	cd(t_executor *exe, char **argv);
void	pwd(t_executor *exe);
void	export(t_executor *exe, char **argv, char **envp);
void	env(t_executor *exe, char **envp);
int		builtin_exit_atoi(const char *nptr);

bool	check_export_identifier(char *arg);
void	apply_export(t_executor *exe, char **argv);
size_t	find_env_var(char **envp, const char *var);

void	expand_wildcard(t_executor *exe, t_array *tokens);

#endif
