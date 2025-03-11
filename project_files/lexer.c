/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 11:06:50 by lfiestas          #+#    #+#             */
/*   Updated: 2025/01/08 10:17:18 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// [ HANDLES STRINGS->TOKEN ]
static char	*push_token_string(
	t_executor *exe, const char *source, bool *is_wildcard)
{
	t_array	token_str;
	int		in_string;

	token_str = arr_new(sizeof(char), 8, &exe->arena);
	*is_wildcard = false;
	in_string = false;
	while (*source != '\0')
	{
		if (!in_string && (ft_isspace(*source) || ft_strchr("<>&|)", *source)))
			break ;
		if (!in_string && (*source == '\'' || *source == '"'))
			in_string = *source;
		else if (in_string && *source == in_string)
			in_string = false;
		else if (!in_string && *source == '*')
		{
			arr_push(&token_str, &(char){WILDCARD});
			*is_wildcard = true;
		}
		else
			arr_push(&token_str, source);
		++source;
	}
	arr_push(&exe->token_strs, &token_str.cs);
	return ((char *)source);
}

// [ HANDLES () SYMBOLS->TOKEN ]
static char	*push_subshell_string(
	t_executor *exe, t_array *tokens, const char *source)
{
	t_array	token_str;
	size_t	closings_needed;

	token_str = arr_new(sizeof(char), 8, &exe->arena);
	closings_needed = 1;
	while (*source != '\0')
	{
		if (*source == '(')
			++closings_needed;
		if (*source == ')')
			--closings_needed;
		if (closings_needed == 0)
			break ;
		arr_push(&token_str, source);
		++source;
	}
	if (*source == '\0' || token_str.cs[0] == '\0')
	{
		arr_push(tokens, &(t_token){')'});
		return ((char *)source);
	}
	arr_push(tokens, &(t_token){'('});
	arr_push(&exe->token_strs, &token_str.cs);
	return ((char *)source + ft_strlen(")"));
}

// [ CONVERTS INPUT INTO TWO ARRAYS OF TOKENS ]
// One array contains the symbols. <>&|" etc.
// The other contains the strings
void	lex(t_executor *exe, const char *src)
{
	t_array	tokens;
	bool	is_wildcard;

	src = preprocess(exe, src);
	tokens = arr_new(sizeof(t_token), ft_strlen(src) + 1, &exe->arena);
	while (*src != '\0')
	{
		while (ft_isspace(*src))
			++src;
		if (*src && ft_strchr("<>&|", *src) && src[1] == src[0] && ++src)
			arr_push(&tokens, &(t_token){(*src++) << 2});
		else if (*src && ft_strchr("<>|)", *src))
			arr_push(&tokens, &(t_token){(*src++)});
		else if (*src == '(')
			src = push_subshell_string(exe, &tokens, src + ft_strlen("("));
		else if (*src)
		{
			arr_push(&tokens, &(t_token){'"'});
			src = push_token_string(exe, src, &is_wildcard);
			if (is_wildcard)
				expand_wildcard(exe, &tokens);
		}
	}
	exe->tokens = tokens.ts;
	exe->tokens_length = tokens.length;
}
