/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fat.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osamoile <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/16 15:24:28 by osamoile          #+#    #+#             */
/*   Updated: 2020/02/16 15:24:29 by osamoile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

#define ERR_ARCHOFFSET "truncated of malformed fat file (offset plus size of \
cputype cpusubtype extends past the end of the file)"

bool show_all_enabled(uint32_t narchs, const t_data *data, uint32_t offset, uint32_t arch_size)
{
    bool res = true;
    while (narchs)
    {
        void *arch = get(*data, offset, arch_size);
        if (arch == NULL)
            return (false);
        cpu_type_t cputype = data->is64 ? ((struct fat_arch_64 *)arch)->cputype
                                        : ((struct fat_arch *)arch)->cputype;
        if (data->cigam)
            cputype = SWAP_32(cputype);
        if (cputype == CPU_TYPE_X86_64)
        {
            res = false;
            break ;
        }
        offset += arch_size;
        narchs--;
    }
    return (res);
}

char *get_arch_name(cpu_type_t cputype, cpu_subtype_t cpusubtype)
{
    if (cputype == CPU_TYPE_I386 && cpusubtype == CPU_SUBTYPE_I386_ALL)
        return ("i386");
    else if (cputype == CPU_TYPE_ARM64 && cpusubtype == CPU_SUBTYPE_ARM64_ALL)
        return ("arm64");
    else if (cputype == CPU_TYPE_ARM && cpusubtype == CPU_SUBTYPE_ARM_V7)
        return ("armv7");
    else if (cputype == CPU_TYPE_ARM && cpusubtype == CPU_SUBTYPE_ARM_V7S)
        return ("armv7s");
    else if (cputype == CPU_TYPE_POWERPC && cpusubtype == CPU_SUBTYPE_POWERPC_ALL)
        return ("ppc");
    else
        return ("");
}

int parse_fat(t_data *data)
{
    struct fat_header *header = get(*data, 0, sizeof(struct fat_header));
    if (header == NULL)
        return (EXIT_FAILURE);
    uint32_t narchs = data->cigam ? SWAP_32(header->nfat_arch)
                                  : header->nfat_arch;
    uint32_t offset = sizeof(struct fat_header);
    uint32_t arch_size = data->is64 ? sizeof(struct fat_arch_64)
                                    : sizeof(struct fat_arch);
    bool cigam = data->cigam;
    bool is64 = data->is64;
    bool show_all = show_all_enabled(narchs, data, offset, arch_size);
    int res = EXIT_SUCCESS;
    while (narchs)
    {
        void *arch = get(*data, offset, arch_size);
        if (arch == NULL)
            return (EXIT_FAILURE);
        data->cigam = cigam;
        data->is64 = is64;
        uint32_t arch_offset = data->is64       ? ((struct fat_arch_64 *)arch)->offset
                                                : ((struct fat_arch *)arch)->offset;
        cpu_type_t cputype = data->is64         ? ((struct fat_arch_64 *)arch)->cputype
                                                : ((struct fat_arch *)arch)->cputype;
        cpu_subtype_t cpusubtype = data->is64   ? ((struct fat_arch_64 *)arch)->cpusubtype
                                                : ((struct fat_arch *)arch)->cpusubtype;
        uint64_t size = data->is64              ? ((struct fat_arch_64 *)arch)->size
                                                : ((struct fat_arch *)arch)->size;
        if (data->cigam) {
            arch_offset = SWAP_32(arch_offset);
            cputype = SWAP_32(cputype);
            cpusubtype = SWAP_32(cpusubtype);
            cpusubtype &= 0x0000ffff;
            if (data->is64)
                size = SWAP_64(size);
            else
                size = SWAP_32(size);
        }
        if (get(*data, arch_offset + size, 0) == NULL)
        {
            print_error(ERR_ARCHOFFSET);
            return (EXIT_FAILURE);
        }
        if (show_all
            || (cputype == CPU_TYPE_X86_64 && cpusubtype == CPU_SUBTYPE_X86_64_ALL))
        {
            char *label = "";
            if (show_all)
            {
                char *arch = get_arch_name(cputype, cpusubtype);
                if (*arch || NM)
                {
                    label = ft_strjoin(data->filename, " (");
                    if (NM)
                    {
                        label = ft_strjoin("\n", label);
                        label = ft_strjoin(label, "for ");
                    }
                    label = ft_strjoin(label, "architecture ");
                    label = ft_strjoin(label, get_arch_name(cputype, cpusubtype));
                    label = ft_strjoin(label, "):\n");
                }
                else
                    label = ft_strjoin(data->filename, ":\n");
            }
            else if (OTOOL)
                label = ft_strjoin(data->filename, ":\n");
            res |= parse_object(data, arch_offset, label);
        }
        offset += arch_size;
        narchs--;
    }
    return (res);
}
