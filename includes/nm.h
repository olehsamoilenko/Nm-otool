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

# if DEBUG
#  define DEBUG 1
# else
#  define DEBUG 0
#endif

# if NM
#  define NM 1
# else
#  define NM 0
#endif

# if OTOOL
#  define OTOOL 1
# else
#  define OTOOL 0
#endif

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

	bool flag_n;
	bool flag_p;
	bool flag_r;
	bool flag_x;
	bool flag_j;
}				t_data;

typedef struct s_symbol // move to symbol.c
{
	char *str;
	uint32_t n_strx;
	uint8_t n_type;
	uint8_t n_sect;
	uint16_t n_desc;
	uint64_t n_value;
} t_symbol;

void *get(t_data data, size_t offset, size_t size);
uint16_t ntoh16(bool cigam, uint16_t nbr);
uint32_t ntoh(bool cigam, uint32_t nbr);
uint64_t ntoh64(bool cigam, uint64_t nbr);

int parse_object(t_data *data, uint32_t offset, char *label);
int parse_fat(t_data *data);
int parse_archive(t_data *data, uint32_t offset);
int parse_segment(t_data *data, uint32_t offset, uint32_t global_offset, cpu_type_t cputype);
int parse_load_command(t_data *data, struct load_command *lc, uint32_t offset, uint32_t global_offset, cpu_type_t cputype);

void print_symbols(t_data data, t_symbol *symbols, uint32_t nsyms);
void sort_symbols(t_symbol *symbols, uint32_t nsyms, t_data data);
int parse_symbol(t_data *data, uint32_t offset, uint32_t stroff, t_symbol *symbol);

#endif
