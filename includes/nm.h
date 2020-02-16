/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osamoile <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/16 09:38:18 by osamoile          #+#    #+#             */
/*   Updated: 2020/02/08 19:36:35 by osamoile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NM_H
# define NM_H

# include <mach-o/loader.h>
# include <mach-o/nlist.h>
# include <mach-o/fat.h>
# include <ar.h>
# include "libft.h"

typedef struct	s_data
{
	void *start;
	size_t len;
	bool is64;
	bool cigam;
}				t_data;

void *get(t_data data, size_t offset, size_t size);

int parse_object(t_data *data, uint32_t offset);
int parse_archive(t_data *data, uint32_t offset);

#endif
