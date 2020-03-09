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

int parse_load_command(t_data *data, struct load_command *lc, uint32_t offset, uint32_t global_offset, cpu_type_t cputype)
{
	const uint32_t cmd_seg = data->is64 ? LC_SEGMENT_64 : LC_SEGMENT;

	if (lc->cmd == cmd_seg || ntoh(data, lc->cmd) == cmd_seg) // wtf ?
	{
		if (DEBUG)
			ft_printf("[LC] cmd: SEGMENT\n");
		int res = parse_segment(data, offset, global_offset, cputype);
		if (res == EXIT_FAILURE)
		{
			if (DEBUG)
			ft_printf("[LC] parse segment failed\n");
			return (EXIT_FAILURE);
		}
	}
	else if (NM && ntoh(data->cigam, lc->cmd) == LC_SYMTAB)
	{
		// NM only
		struct symtab_command *sym_cmd = get(*data, offset, sizeof(struct symtab_command));
		if (sym_cmd == NULL)
		{
			if (DEBUG)
				ft_printf("[LC] get sym_cmd failed\n");
			return (EXIT_FAILURE);
		}
		uint32_t nsyms = ntoh(data->cigam, sym_cmd->nsyms);
		uint32_t symoff = ntoh(data->cigam, sym_cmd->symoff);
		uint32_t stroff = ntoh(data->cigam, sym_cmd->stroff);
		
		if (DEBUG)
			ft_printf("[LC] cmd: SYMTAB, nsyms: %d, symoff: %d, stroff: %d\n", nsyms, symoff, stroff);

		const size_t sym_size = data->is64	? sizeof(struct nlist_64)
											: sizeof(struct nlist);
		int new_offset = 0; // replace with offset, it's offset to every symbol

		t_symbol *symbols = malloc(nsyms * sizeof(t_symbol));
		t_symbol *tmp = symbols;

		while (nsyms)
		{
			int res = parse_symbol(data, global_offset + symoff + new_offset, global_offset + stroff, tmp);

			if (res == EXIT_FAILURE)
			{
				if (DEBUG)
					ft_printf("[LC] parse symbol failed\n");
				return (EXIT_FAILURE);
			}

			new_offset += sym_size;
			tmp++;
			nsyms--;
		}

		if (!data->flag_p)
			sort_symbols(symbols, ntoh(data->cigam, sym_cmd->nsyms), *data);
		print_symbols(*data, symbols, ntoh(data->cigam, sym_cmd->nsyms));
	}
	else if (lc->cmd == 0 && DEBUG)
		ft_printf("[LC] cmd: 0, failed\n");
	else if (DEBUG)
		ft_printf("[LC] skip cmd: %d\n", ntoh(data->cigam, lc->cmd));

	return (EXIT_SUCCESS);
}