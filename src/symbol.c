/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbol.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osamoile <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/16 19:37:34 by osamoile          #+#    #+#             */
/*   Updated: 2020/02/16 19:37:36 by osamoile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

char define_type(uint8_t n_type, uint8_t n_sect, uint64_t n_value, t_data data)
{
	const uint8_t type = n_type & N_TYPE;
	uint8_t res;

	/*(type != N_ABS && type != N_SECT && type != N_INDR && type != N_UNDF) || */
	if (n_type & N_STAB)
		res = 0;
	else if (n_type && type == N_UNDF && n_value)
		res = 'c';
	else if (n_type && type == N_UNDF)
		res = 'u';
	else if (type == N_ABS)
		res = 'a';
	else if (type == N_SECT && n_sect == data.text_section_number)
		res = 't';
	else if (type == N_SECT && n_sect == data.data_section_number)
		res = 'd';
	else if (type == N_SECT && n_sect == data.bss_section_number)
		res = 'b';
	else if (type == N_SECT)
		res = 's';
	else if (type == N_INDR)
		res = 'i';
	else
		res = '?';

	if (n_type & N_EXT)
		res = ft_toupper(res);

	return (res);
}

void sort_symbols(t_symbol *symbols, uint32_t nsyms, t_data data)
{
	int i = -1;
	int j = -1;

	while (++i < nsyms)
	{
		while (++j < nsyms)
		{
			bool swap = false;
			int diff_str = ft_strcmp(symbols[i].str, symbols[j].str);
			// if (data.flag_n)
			// {
			// 	if (define_type(symbols[i].n_type, symbols[i].n_sect, symbols[i].n_value, data) == 'U'
			// 		|| diff_adr < 0
			// 		|| (diff_adr == 0 && diff_str < 0))
			// 		swap = true;
			// }
			// else
			// {
				if (diff_str < 0 || (diff_str == 0 && symbols[i].n_value < symbols[j].n_value))
					swap = true;
			// }
			if (data.flag_r)
				swap = !swap;
			if (swap)
			{
				t_symbol tmp = symbols[i];
				symbols[i] = symbols[j];
				symbols[j] = tmp;
			}
		}
		j = -1;
	}
}

int parse_symbol(t_data *data, uint32_t offset, uint32_t stroff, t_symbol *symbol)
{
	const size_t sym_size = data->is64	? sizeof(struct nlist_64)
										: sizeof(struct nlist);
	void *sym = get(*data, offset, sym_size);
	if (sym == NULL)
		return (EXIT_FAILURE);

	symbol->n_type = data->is64		? ((struct nlist_64 *)sym)->n_type
									: ((struct nlist *)sym)->n_type;
	symbol->n_sect = data->is64		? ((struct nlist_64 *)sym)->n_sect
									: ((struct nlist *)sym)->n_sect;
	symbol->n_desc = data->is64		? ((struct nlist_64 *)sym)->n_desc
									: ((struct nlist *)sym)->n_desc;
	symbol->n_desc = ntoh16(data->cigam, symbol->n_desc);
	symbol->n_value = data->is64	? ((struct nlist_64 *)sym)->n_value
									: ((struct nlist *)sym)->n_value;
	if (data->is64)
		symbol->n_value = ntoh64(data->cigam, symbol->n_value);
	else
		symbol->n_value = ntoh(data->cigam, symbol->n_value);
	symbol->n_strx = data->is64	? ((struct nlist_64 *)sym)->n_un.n_strx
								: ((struct nlist *)sym)->n_un.n_strx;
	symbol->n_strx = ntoh(data->cigam, symbol->n_strx);
	symbol->str = get(*data, stroff + symbol->n_strx, 0); // use symbol->n_strx as index
	if (DEBUG)
		ft_printf("[SYMBOL] type: %2d, strx: %4d, str: %20s, nsect: %d, nvalue: %lu\n",
			symbol->n_type, symbol->n_strx, symbol->str, symbol->n_sect, symbol->n_value);
	if (symbol->str == NULL)
	{
		if (DEBUG)
			ft_printf("[SYMBOL] bad string index\n");
		symbol->str = "bad string index";
	}

	return (EXIT_SUCCESS);
}

void print_symbols(t_data data, t_symbol *symbols, uint32_t nsyms)
{
	int i = -1;

	while (++i < nsyms)
	{
		char type = define_type(symbols[i].n_type, symbols[i].n_sect, symbols[i].n_value, data);
		if (type)
		{
			if (!data.flag_j)
			{
				if (data.flag_x)
				{
					if (data.is64)
						ft_printf("%016lx %02x %02x %04x %08x ",
						symbols[i].n_value, symbols[i].n_type, symbols[i].n_sect,
						symbols[i].n_desc, symbols[i].n_strx);
					else
					{
						ft_printf("%08x %02x %02x %04x %08x ",
						symbols[i].n_value, symbols[i].n_type, symbols[i].n_sect,
						symbols[i].n_desc, symbols[i].n_strx);
					}
				}
				else if (type == 'U')
				{
					if (data.is64)
						ft_printf("%16s ", "");
					else
					{
						ft_printf("%8s ", "");
					}
					ft_printf("%c ", type);
				}
				else
				{
					if (data.is64)
						ft_printf("%016lx ", symbols[i].n_value);
					else
					{
						ft_printf("%08x ", symbols[i].n_value);
					}
					ft_printf("%c ", type);
				}
			}
			ft_printf("%s\n", symbols[i].str);
		}
		else
		{
			if (DEBUG)
				ft_printf("[SYMBOL] skip: %s, type: %d\n", symbols[i].str, type);
		}
	}
}
