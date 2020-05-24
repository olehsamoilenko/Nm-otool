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

#define ERR_CMDSIZE "truncated or malformed object (load command cmdsize not a multiple of "
#define ERR_CMD0 "truncated or malformed object (load command extends past the end all load command in the file)"

void print_error(char *str)
{
    if (NM)
        write(STDERR_FILENO, "./ft_nm: ", 9);
    else if (OTOOL)
        write(STDERR_FILENO, "./ft_otool: ", 12);
    write(STDERR_FILENO, str, ft_strlen(str));
    write(STDERR_FILENO, "\n", 1);
}

int parse_load_command(t_data *data, struct load_command *lc, uint32_t offset, uint32_t global_offset, cpu_type_t cputype)
{
    const uint32_t cmd_seg = data->is64 ? LC_SEGMENT_64 : LC_SEGMENT;
    uint32_t cmdsize = ntoh(data->cigam, lc->cmdsize);
    const uint8_t multiple_of = data->is64 ? 8 : 4;
    if (cmdsize % multiple_of != 0)
    {
        char *num_str = ft_itoa(multiple_of);
        char *err_msg = ft_strjoin(ERR_CMDSIZE, num_str);
        err_msg = ft_strjoin(err_msg, ")");
        print_error(err_msg);
        return (EXIT_FAILURE);
    }
    if (lc->cmd == cmd_seg || ntoh(data, lc->cmd) == cmd_seg)
    {
        int res = parse_segment(data, offset, global_offset, cputype);
        if (res == EXIT_FAILURE)
            return (EXIT_FAILURE);
    }
    else if (ntoh(data->cigam, lc->cmd) == LC_SYMTAB)
    {
        struct symtab_command *sym_cmd = get(*data, offset, sizeof(struct symtab_command));
        if (sym_cmd == NULL)
            return (EXIT_FAILURE);
        uint32_t nsyms = ntoh(data->cigam, sym_cmd->nsyms);
        uint32_t symoff = ntoh(data->cigam, sym_cmd->symoff);
        uint32_t stroff = ntoh(data->cigam, sym_cmd->stroff);
        const size_t sym_size = data->is64  ? sizeof(struct nlist_64)
                                            : sizeof(struct nlist);
        int new_offset = 0;
        t_symbol *symbols = malloc(nsyms * sizeof(t_symbol));
        t_symbol *tmp = symbols;
        while (nsyms)
        {
            int res = parse_symbol(data, global_offset + symoff + new_offset, global_offset + stroff, tmp);
            if (res == EXIT_FAILURE)
                return (EXIT_FAILURE);
            new_offset += sym_size;
            tmp++;
            nsyms--;
        }
        if (NM)
        {
            if (!data->flag_p)
                sort_symbols(symbols, ntoh(data->cigam, sym_cmd->nsyms), *data);
            print_symbols(*data, symbols, ntoh(data->cigam, sym_cmd->nsyms));
        }
    }
    else if (lc->cmd == 0)
    {
        print_error(ERR_CMD0);
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}