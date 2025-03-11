/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 17:52:41 by lfiestas          #+#    #+#             */
/*   Updated: 2025/01/04 17:55:05 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <dirent.h>

// [ CHECK IF STRING MATCHES WILDCARD PATERN ]
static bool	match_wildcard(const char *pattern, const char *string)
{
	if ((*pattern == '\0' && *string == '\0')
		|| (pattern[0] == WILDCARD && pattern[1] == '\0'))
		return (true);
	if ((*pattern == '\0' && *string != '\0')
		|| (*pattern != '\0' && *string == '\0'))
		return (false);
	if (*pattern == *string)
		return (match_wildcard(pattern + 1, string + 1));
	if (pattern[0] == WILDCARD && pattern[1] == *string)
		return (match_wildcard(pattern + 2, string + 1));
	if (*pattern == WILDCARD)
		return (match_wildcard(pattern + 0, string + 1));
	return (false);
}

static void	check_match(
	t_executor *exe, t_array *tokens, const char *match, char *pattern)
{
	size_t	i;

	if (match)
		return ;
	i = (size_t) - 1;
	while (pattern[++i] != '\0')
		if (pattern[i] == WILDCARD)
			pattern[i] = '*';
	arr_push(tokens, &(t_token){'"'});
	arr_push(&exe->token_strs, &pattern);
}

void	expand_wildcard(t_executor *exe, t_array *tokens)
{
	char			*pattern;
	char			*match;
	DIR				*directory;
	struct dirent	*entry;

	directory = opendir(".");
	pattern = ft_fast_strdup(\
		&exe->arena, exe->token_strs.ps[--exe->token_strs.length]);
	--tokens->length;
	match = NULL;
	entry = readdir(directory);
	while (entry != NULL)
	{
		if (entry->d_name[0] != '.' && match_wildcard(pattern, entry->d_name))
		{
			match = ft_fast_strdup(&exe->arena, entry->d_name);
			arr_push(tokens, &(t_token){'"'});
			arr_push(&exe->token_strs, &match);
		}
		entry = readdir(directory);
	}
	closedir(directory);
	check_match(exe, tokens, match, pattern);
}
