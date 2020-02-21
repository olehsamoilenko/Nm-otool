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

# define SWAP_16(x) ((((x) & 0xff00U) >> 8U) | (((x) & 0x00ffU) << 8U))
# define SWAP_32(x) ((SWAP_16(x) << 16U) | (SWAP_16((x) >> 16U)))
# define SWAP_64(x) ((SWAP_32(x) << 32U) | (SWAP_32((x) >> 32U)))

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
	char *filename;

	int sections_total;
	int text_section_number;
	int data_section_number;
	int bss_section_number;
}				t_data;

void *get(t_data data, size_t offset, size_t size);
uint32_t ntoh(bool cigam, uint32_t nbr);

int parse_object(t_data *data, uint32_t offset);
int parse_fat(t_data *data);
int parse_archive(t_data *data, uint32_t offset);
int parse_segment(t_data *data, uint32_t offset);
int parse_load_command(t_data *data, struct load_command *lc, uint32_t offset, uint32_t global_offset);

#endif
