/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljylhank <ljylhank@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 17:29:51 by ljylhank          #+#    #+#             */
/*   Updated: 2025/01/06 16:10:31 by ljylhank         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <readline/readline.h>

sig_atomic_t	g_signum = 0;

void	handle_signals(int signum)
{
	g_signum = signum;
}

int	redisplay_prompt(void)
{
	if (g_signum)
	{
		if (g_signum == SIGINT)
			ft_putstr_fd("\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	g_signum = 0;
	return (0);
}
