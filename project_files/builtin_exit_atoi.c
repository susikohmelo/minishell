/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit_atoi.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljylhank <ljylhank@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 14:21:00 by ljylhank          #+#    #+#             */
/*   Updated: 2025/01/06 16:18:45 by lfiestas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <limits.h>
#include <stdint.h>

static int	print_error(const char *nptr)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(nptr, 2);
	ft_putendl_fd(": numeric argument required", 2);
	return (2);
}

int	builtin_exit_atoi(const char *nptr)
{
	int			i;
	__int128_t	rval;
	int			sign;

	i = 0;
	rval = 0;
	sign = 1;
	while (nptr[i] && (nptr[i] == ' '))
		i++;
	if (!nptr[i])
		return (print_error(nptr));
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign = -1;
		++i;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		rval = rval * 10 + nptr[i] - '0';
		if (rval > LONG_MAX)
			return (print_error(nptr));
		++i;
	}
	return ((int)rval * sign);
}
