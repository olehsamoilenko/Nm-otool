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

int parse_section(t_data *data, void *section, int gl_offset, cpu_type_t cputype)
{
    char *sectname = data->is64 ? ((struct section_64 *)section)->sectname
                                : ((struct section *)section)->sectname;
    char *segname = data->is64  ? ((struct section_64 *)section)->segname
                                : ((struct section *)section)->segname;
    data->sections_total++;
    if (ft_strequ(segname, SEG_TEXT) && ft_strequ(sectname, SECT_TEXT))
    {
        data->text_section_number = data->sections_total;
        if (OTOOL)
        {
            uint64_t size = data->is64      ? ((struct section_64 *)section)->size
                                            : ((struct section *)section)->size;
            uint32_t offset = data->is64    ? ((struct section_64 *)section)->offset
                                            : ((struct section *)section)->offset;
            uint64_t addr = data->is64      ? ((struct section_64 *)section)->addr
                                            : ((struct section *)section)->addr;
            offset = ntoh(data->cigam, offset);
            if (!data->is64)
            {
                addr = ntoh(data->cigam, addr);
                size = ntoh(data->cigam, size);
            }
            ft_printf("Contents of (%s,%s) section", segname, sectname);
            void *text = get(*data, offset + gl_offset, size);
            bool use_bunch;
            if (cputype != CPU_TYPE_X86 && cputype != CPU_TYPE_X86_64)
                use_bunch = true;
            else
                use_bunch = false;
            int values_per_line;
            if (use_bunch)
                values_per_line = 4;
            else
                values_per_line = 16;
            if (text != NULL)
            {
                if (use_bunch)
                    size /= 4;
                int i = 0;
                while (size)
                {
                    if (i == 0)
                    {
                        i = values_per_line;
                        ft_putchar('\n');
                        if (data->is64)
                            ft_printf("%016lx\t", addr);
                        else
                            ft_printf("%08x\t", addr);
                        addr += 16;
                    }
                    if (use_bunch)
                    {
                        ft_printf("%08x", ntoh(data->cigam, *(uint32_t *)text));
                        text += 4;
                    }
                    else
                    {
                        ft_printf("%02x", *(uint8_t *)text);
                        text += 1;
                    }
                    ft_putchar(' ');
                    i--;
                    size--;
                }
                ft_putchar('\n');
            }
            else
                return (EXIT_FAILURE);
        }
    }
    else if (ft_strequ(segname, SEG_DATA) && ft_strequ(sectname, SECT_DATA))
        data->data_section_number = data->sections_total;
    else if (ft_strequ(segname, SEG_DATA) && ft_strequ(sectname, SECT_BSS))
        data->bss_section_number = data->sections_total;
    return (EXIT_SUCCESS);
}

int parse_segment(t_data *data, uint32_t offset, uint32_t global_offset, cpu_type_t cputype)
{
    int res = EXIT_SUCCESS;
    const size_t seg_cmd_size = data->is64  ? sizeof(struct segment_command_64)
                                            : sizeof(struct segment_command);
    void *seg_cmd = get(*data, offset, sizeof(struct segment_command_64));
    if (seg_cmd == NULL)
        return (EXIT_FAILURE);
    uint32_t nsects = data->is64    ? ((struct segment_command_64 *)seg_cmd)->nsects
                                    : ((struct segment_command *)seg_cmd)->nsects;
    nsects = ntoh(data->cigam, nsects);
    offset += seg_cmd_size;
    const size_t sec_size = data->is64  ? sizeof(struct section_64)
                                        : sizeof(struct section);
    int i = 0;
    while (++i <= nsects)
    {
        void *section = get(*data, offset, sec_size);
        if (section == NULL)
            return (EXIT_FAILURE);
        int res = parse_section(data, section, global_offset, cputype);
        if (res == EXIT_FAILURE)
            return (EXIT_FAILURE);
        offset += sec_size;
    }
    return res;
}
