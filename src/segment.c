/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   segment.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osamoile <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/16 19:37:34 by osamoile          #+#    #+#             */
/*   Updated: 2020/02/16 19:37:36 by osamoile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

int parse_section(t_data *data, void *section, int gl_offset)
{
	char *sectname = data->is64 ?	((struct section_64 *)section)->sectname
								:	((struct section *)section)->sectname;
	char *segname = data->is64 ?	((struct section_64 *)section)->segname
								:	((struct section *)section)->segname;
	if (DEBUG)
		ft_printf("[SECTION] %s\n", sectname);
	data->sections_total++;
	if (ft_strequ(segname, SEG_TEXT) && ft_strequ(sectname, SECT_TEXT))
	{
		data->text_section_number = data->sections_total;
		if (DEBUG)
			ft_printf("[SECTION] SEG_TEXT found\n");
		if (OTOOL)
		{
			uint64_t size = data->is64		?	((struct section_64 *)section)->size
											:	((struct section *)section)->size;
			uint32_t offset = data->is64	?	((struct section_64 *)section)->offset
											:	((struct section *)section)->offset;
			uint64_t addr = data->is64		?	((struct section_64 *)section)->addr
											:	((struct section *)section)->addr;
			
			ft_printf("Contents of (%s,%s) section", segname, sectname);
			void *text = get(*data, offset + gl_offset, size);
			if (text != NULL)
			{
				size /= 4;
				int i = 0;
				while (size)
				{
					if (i == 0)
					{
						i = 4;
						ft_putchar('\n');
						if (data->is64)
							ft_printf("%016lx\t", addr);
						else
							ft_printf("%08x\t", addr);
						addr += 16;
					}
					ft_printf("%08x ", *(uint32_t *)text);
					i--;
					text = (uint32_t *)text + 1;
					size--;
				}
				ft_putchar('\n');
			}
			else
			{
				if (DEBUG)
					ft_printf("[SECTION] fail getting text\n");
				return (EXIT_FAILURE);
			}
		}
	}
	else if (ft_strequ(segname, SEG_DATA) && ft_strequ(sectname, SECT_DATA))
	{
		data->data_section_number = data->sections_total;
		if (DEBUG)
			ft_printf("[SECTION] SECT_DATA found\n");
	}
	else if (ft_strequ(segname, SEG_DATA) && ft_strequ(sectname, SECT_BSS))
	{
		data->bss_section_number = data->sections_total;
		if (DEBUG)
			ft_printf("[SECTION] SECT_BSS found\n");
	}

	return (EXIT_SUCCESS);
}

int parse_segment(t_data *data, uint32_t offset, uint32_t global_offset)
{
	int res = EXIT_SUCCESS;
	const size_t seg_cmd_size = data->is64	? sizeof(struct segment_command_64)
											: sizeof(struct segment_command);

	void *seg_cmd = get(*data, offset, sizeof(struct segment_command_64));
	if (seg_cmd == NULL)
	{
		if (DEBUG)
			ft_printf("[SEGMENT] get seg cmd failed\n");
		return (EXIT_FAILURE);
	}
	uint32_t nsects = data->is64	? ((struct segment_command_64 *)seg_cmd)->nsects
									: ((struct segment_command *)seg_cmd)->nsects;
	nsects = ntoh(data->cigam, nsects);
	if (DEBUG)
		ft_printf("[SEGMENT] nsects: %d\n", nsects);
	offset += seg_cmd_size;

	const size_t sec_size = data->is64	? sizeof(struct section_64)
										: sizeof(struct section);

	int i = 0;
	while (++i <= nsects)
	{
		void *section = get(*data, offset, sec_size);
		if (section == NULL)
		{
			if (DEBUG)
				ft_printf("[SEGMENT] get section failed\n");
			return (EXIT_FAILURE);
		}
		int res = parse_section(data, section, global_offset);
		if (res == EXIT_FAILURE)
		{
			if (DEBUG)
				ft_printf("[SEGMENT] parse section failed\n");
			return (EXIT_FAILURE);
		}
		offset += sec_size;
	}

	return res;
}
