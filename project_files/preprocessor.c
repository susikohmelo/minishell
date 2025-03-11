/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   preprocessor.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 15:23:05 by lfiestas          #+#    #+#             */
/*   Updated: 2025/01/07 16:15:51 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	has_tokens(const char *var)
{
	while (*var)
		if (ft_strchr("\"\'<>&|)", *var++))
			return (true);
	return (false);
}

/* Returns number of bytes read from source
 */
static size_t	concat_var(
	t_array *out, t_executor *exe, const char *source, int in_string)
{
	size_t	i;
	size_t	length;
	char	*var;

	if (source[0] == '?')
	{
		var = ft_i_to_str(ft_arena_alloc(&exe->arena, 12), exe->exit_status);
		return (arr_concat(out, var, ft_strlen(var)), ft_strlen("?"));
	}
	else if (!ft_isalnum(source[0]) && source[0] != '_')
		return (arr_push(out, "$"), 0);
	length = 0;
	while (ft_isalnum(source[length]) || source[length] == '_')
		++length;
	i = (size_t) - 1;
	var = "";
	while (++i < exe->envp.length)
		if (ft_strncmp(exe->envp.ps[i], source, length) == 0
			&& ((char *)exe->envp.ps[i])[length] == '=')
			var = exe->envp.ps[i] + ft_strlen("=") + length;
	arr_concat(out, "'", !in_string && has_tokens(var));
	arr_concat(out, var, ft_strlen(var));
	return (arr_concat(out, "'", !in_string && has_tokens(var)), length);
}

// [ EXPANDS VARIABLES AND QUOTES ]
char	*preprocess(t_executor *exe, const char *source)
{
	t_array		src_out;
	size_t		len;
	int			in_string;

	if (ft_strchr(source, '$') == NULL)
		return ((char *)source);
	src_out = arr_new(sizeof(char), 32, &exe->arena);
	in_string = false;
	len = (size_t) - 1;
	while (source[++len] != '\0')
	{
		if (!in_string && (source[len] == '\'' || source[len] == '"'))
			in_string = source[len];
		else if (in_string && source[len] == in_string)
			in_string = false;
		else if (in_string != '\'' && source[len] == '$')
		{
			arr_concat(&src_out, source++, len);
			source += len + concat_var(&src_out, exe, source + len, in_string);
			len = (size_t) - 1;
		}
	}
	return (arr_concat(&src_out, source, len), src_out.cs);
}
