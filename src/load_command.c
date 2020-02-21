/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osamoile <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/16 19:40:39 by osamoile          #+#    #+#             */
/*   Updated: 2020/02/16 19:40:41 by osamoile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

typedef struct s_symbol
{
	char *str;
	uint8_t n_type;
	uint8_t n_sect;
	uint64_t n_value;
} t_symbol;

char define_type(uint8_t n_type, uint8_t n_sect, t_data data)
{
	const uint8_t type = n_type & N_TYPE;
	uint8_t res;

	// ft_printf("%x\n", n_type & N_STAB);
	if (/*(type != N_ABS && type != N_SECT && type != N_INDR && type != N_UNDF) || */n_type & N_STAB)
	{
		// ft_printf("HELLO %x\n", n_type & N_STAB);
		res = 0;
	}
	else if (type == N_UNDF) // ? check value
		res = 'u';
	else if (type == N_ABS)
		res = 'a';
	else if (type == N_SECT && n_sect == data.text_section_number) // not nulled !
		res = 't';
	else if (type == N_SECT && n_sect == data.data_section_number) // not nulled !
		res = 'd';
	else if (type == N_SECT && n_sect == data.bss_section_number) // not nulled !
		res = 'b';
	else if (type == N_INDR)
		res = 'i';
	else // is N_SECT
	{
		res = 's';
		// ft_printf("sect #%d\n", n_sect);
	}

	if (n_type & N_EXT)
		res = ft_toupper(res);

	return (res);
}

void sort_symbols(t_symbol *symbols, uint32_t nsyms)
{
	int i = -1;
	int j = -1;

	while (++i < nsyms)
	{
		while (++j < nsyms)
		{
			if (ft_strcmp(symbols[i].str, symbols[j].str) < 0)
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
	
	symbol->n_value = data->is64	? ((struct nlist_64 *)sym)->n_value
									: ((struct nlist *)sym)->n_value;
	symbol->n_value = ntoh(data->cigam, symbol->n_value); // 64 bit error
	uint32_t n_strx = data->is64	? ((struct nlist_64 *)sym)->n_un.n_strx
									: ((struct nlist *)sym)->n_un.n_strx;
	n_strx = ntoh(data->cigam, n_strx);
	symbol->str = get(*data, stroff + n_strx, 0);
	// ft_printf("SYMBOL: type: %d, strx: %d, str: %s, nsect: %d, nvalue: %d\n",
	// 	symbol->n_type, n_strx, symbol->str, symbol->n_sect, symbol->n_value);
	if (symbol->str == NULL)
	{
		ft_printf("failed: str null\n");
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

void print_symbols(t_data data, t_symbol *symbols, uint32_t nsyms)
{
	int i = -1;

	// ft_printf("%d %d %d\n", data.text_section_number, data.data_section_number, data.bss_section_number);
	while (++i < nsyms)
	{
		char type = define_type(symbols[i].n_type, symbols[i].n_sect, data);
		// ft_printf("n sect: %d              ", symbols[i].n_sect);
		if (type)
		{
			if (type == 'U')
			{
				if (data.is64)
					ft_printf("%16s", "");
				else
				{
					ft_printf("%8s", "");
				}
			}
			else
			{
				if (data.is64)
					ft_printf("%016lx", symbols[i].n_value);
				else
				{
					ft_printf("%08lx", symbols[i].n_value);
				}
			}

			ft_printf(" %c %s\n", type, symbols[i].str);
		}
		else
		{
			#if DEBUG
				ft_printf("SKIP: %s\n", symbols[i].str);
				ft_printf("SYMBOL: type: %d, strx: %d, str: %s\n", type, n_strx, str);
			#endif
		}
	}
}

int parse_load_command(t_data *data, struct load_command *lc, uint32_t offset, uint32_t global_offset)
{
	const uint32_t cmd_seg = data->is64 ? LC_SEGMENT_64 : LC_SEGMENT;

	// (ntoh(data, lc->cmd) == cmd_seg)
	if (lc->cmd == cmd_seg)
	{
		// ft_printf("LC_SEGMENT\n");
		int res = parse_segment(data, offset);
		if (res == EXIT_FAILURE)
		{
			ft_printf("parse segment failed\n");
			return (EXIT_FAILURE);
		}
	}
	else if (ntoh(data->cigam, lc->cmd) == LC_SYMTAB)
	{
		// NM only
		struct symtab_command *sym_cmd = get(*data, offset, sizeof(struct symtab_command));
		if (sym_cmd == NULL)
		{
			ft_printf("get sym_cmd failed\n");
			return (EXIT_FAILURE);
		}
		uint32_t nsyms = ntoh(data->cigam, sym_cmd->nsyms);
		uint32_t symoff = ntoh(data->cigam, sym_cmd->symoff);
		uint32_t stroff = ntoh(data->cigam, sym_cmd->stroff);
		
		// #if DEBUG
			// ft_printf("SYMTAB nsyms: %d, symoff: %d, stroff: %d\n", nsyms, symoff, stroff);
		// #endif

		const size_t sym_size = data->is64	? sizeof(struct nlist_64)
											: sizeof(struct nlist);
		int new_offset = 0; // replace with offset

		t_symbol *symbols = malloc(nsyms * sizeof(t_symbol));
		t_symbol *tmp = symbols;

		while (nsyms)
		{
			int res = parse_symbol(data, global_offset + symoff + new_offset, global_offset + stroff, tmp);

			if (res == EXIT_FAILURE)
			{
				ft_printf("parse symbol failed\n");
				return (EXIT_FAILURE);
			}

			new_offset += sym_size;
			tmp++;
			nsyms--;
		}

		sort_symbols(symbols, ntoh(data->cigam, sym_cmd->nsyms));
		print_symbols(*data, symbols, ntoh(data->cigam, sym_cmd->nsyms));
	}
	// else
	// {
	// 	ft_printf("pass %#x\n", lc->cmd);
	// }

	return (EXIT_SUCCESS);
}