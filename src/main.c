/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osamoile <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/16 09:37:16 by osamoile          #+#    #+#             */
/*   Updated: 2019/11/16 09:37:23 by osamoile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

uint32_t ntoh(bool cigam, uint32_t nbr)
{
    if (cigam)
        nbr = SWAP_32(nbr);
    return (nbr);
}

uint64_t ntoh64(bool cigam, uint64_t nbr)
{
    if (cigam)
        nbr = SWAP_64(nbr);
    return (nbr);
}

uint16_t ntoh16(bool cigam, uint16_t nbr) // mb without it ?
{
    if (cigam)
        nbr = SWAP_16(nbr);
    return (nbr);
}

void *get(t_data data, size_t offset, size_t size)
{
    if (offset + size <= data.len)
        return (data.start + offset);
    else
        return (NULL);
}



int parse_mach_o(t_data *data, uint32_t offset, char *label)
{
    ft_putstr(label);
    size_t header_size = data->is64 ? sizeof(struct mach_header_64) : sizeof(struct mach_header);
    void *header = get(*data, offset, header_size);
    if (header == NULL)
    {
        if (DEBUG)
            ft_printf("[MACH-O] get header failed\n");
        return (EXIT_FAILURE);
    }
    uint32_t ncmds = data->is64     ? ((struct mach_header_64 *)header)->ncmds
                                    : ((struct mach_header *)header)->ncmds;
    cpu_type_t cputype = data->is64 ? ((struct mach_header_64 *)header)->cputype
                                    : ((struct mach_header *)header)->cputype;
    ncmds = ntoh(data->cigam, ncmds);
    uint32_t fat_offset = offset;
    offset += header_size;
    data->sections_total = 0;
    data->data_section_number = 0;
    data->bss_section_number = 0;
    data->text_section_number = 0;
    if (DEBUG)
        ft_printf("[MACH-O] ncmds: %d\n", ncmds);
    while (ncmds)
    {
        struct load_command *lc = get(*data, offset, sizeof(struct load_command));
        if (lc == NULL)
        {
            if (DEBUG)
                ft_printf("[MACH-O] get lc failed\n");
            return (EXIT_FAILURE);
        }
        if (parse_load_command(data, lc, offset, fat_offset, cputype) == EXIT_FAILURE)
        {
            if (DEBUG)
                ft_printf("[MACH-O] parse_load_command failed\n");
            return (EXIT_FAILURE);
        }
        offset += ntoh(data->cigam, lc->cmdsize);
        ncmds--;
    }
    return (EXIT_SUCCESS);
}

int parse_object(t_data *data, uint32_t offset, char *label)
{
    int res = EXIT_FAILURE;
    void *m = get(*data, offset, sizeof(uint32_t));
    uint32_t magic;
    if (m)
        magic = *(uint32_t *)m;
    else
        magic = 0;
    if (magic == 0)
    {
        if (DEBUG)
            ft_printf("[MAGIC] failed: bad magic\n");
    }
    else if (magic == MH_MAGIC_64)
    {
        if (DEBUG)
            ft_printf("[MAGIC] MH_MAGIC_64\n");
        data->is64 = true;
        data->cigam = false;
        res = parse_mach_o(data, offset, label);
    }
    else if (magic == MH_MAGIC)
    {
        if (DEBUG)
            ft_printf("[MAGIC] MH_MAGIC\n");
        data->is64 = false;
        data->cigam = false;
        res = parse_mach_o(data, offset, label);
    }
    else if (magic == MH_CIGAM)
    {
        if (DEBUG)
            ft_printf("[MAGIC] MH_CIGAM\n");
        data->is64 = false;
        data->cigam = true;
        res = parse_mach_o(data, offset, label);
    }
    else if (magic == MH_CIGAM_64)
    {
        if (DEBUG)
            ft_printf("[MAGIC] failed: MH_CIGAM_64\n");
    }
    else if (magic == FAT_MAGIC)
    {
        if (DEBUG)
            ft_printf("[MAGIC] failed: FAT_MAGIC\n");
    }
    else if (magic == FAT_CIGAM)
    {
        if (DEBUG)
            ft_printf("[MAGIC] FAT_CIGAM\n");
        data->is64 = false;
        data->cigam = true;
        res = parse_fat(data);
    }
    else if (magic == FAT_CIGAM_64)
    {
        if (DEBUG)
            ft_printf("[MAGIC] failed: FAT_CIGAM_64\n");
    }
    else if (magic == *(uint32_t*)ARMAG)
    {
        if (DEBUG)
            ft_printf("[MAGIC] ARCHIVE\n");
        res = parse_archive(data, offset);
    }
    else
    {
        if (DEBUG)
            ft_printf("[MAGIC] failed: Other\n");
    }
    return (res);
}

int parse_file(char *file_name, t_data *data)
{
    int            fd;
    struct stat    stat;
    void        *data_ptr;
    if ((fd = open(file_name, O_RDONLY)) == -1)
        return (EXIT_FAILURE);
    if (fstat(fd, &stat) == -1)
        return (EXIT_FAILURE);
    if (S_ISDIR(stat.st_mode))
        return (EXIT_FAILURE);
    if ((data_ptr = mmap(0, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        return (EXIT_FAILURE);
    if (close(fd) == -1)
        return (EXIT_FAILURE);
    data->start = data_ptr;
    data->len = stat.st_size;
    data->filename = file_name;
    char *label = "";
    if (OTOOL)
        label = ft_strjoin(file_name, ":\n");
    int res = parse_object(data, 0, label);
    if (munmap(data_ptr, stat.st_size) == -1)
        return (EXIT_FAILURE);
    return (res);
}

bool store_flag(char flag, t_data *data)
{
    const char *alailable = "nprjx";
    if (ft_strchr(alailable, flag) == NULL)
    {
        ft_printf("ft_nm: Unknown argument '-%c'. Please use: '-%s'\n", flag, alailable);
        return (false);
    }
    if (flag == 'n' && data->flag_n == false)
        data->flag_n = true;
    else if (flag == 'p' && data->flag_p == false)
        data->flag_p = true;
    else if (flag == 'r' && data->flag_r == false)
        data->flag_r = true;
    else if (flag == 'j' && data->flag_j == false)
        data->flag_j = true;
    else if (flag == 'x' && data->flag_x == false)
        data->flag_x = true;
    else
    {
        ft_printf("ft_nm: '-%c' may only occur zero or one times!\n", flag);
        return (false);
    }
    return (true);
}

bool get_flags(t_data *data, int argc, char **argv)
{
    int i = 0;
    while (++i < argc)
    {
        if (argv[i][0] == '-') // check nm
        {
            int j = 0;
            while (argv[i][++j])
            {
                if (store_flag(argv[i][j], data))
                {
                    if (DEBUG)
                        ft_printf("[ARGV] flag: %c\n", argv[i][j]);
                }
                else
                    return (false);
            }
        }
    }
    return (true);
}

// TODO: fix leaks
int main(int argc, char **argv)
{
    t_data data;
    int res = EXIT_SUCCESS;
    ft_bzero(&data, sizeof(t_data));
    if (get_flags(&data, argc, argv) == false)
        return (EXIT_FAILURE);
    if (argc == 1)
    {
        // a.out
    }
    else
    {
        int i = 0;
        while (++i < argc)
        {
            if (argv[i][0] != '-')
            {
                if (DEBUG)
                    ft_printf("[ARGV] parse file: %s\n", argv[i]);
                if (parse_file(argv[i], &data) == EXIT_FAILURE)
                {
                    res = EXIT_FAILURE;
                    if (DEBUG)
                        ft_printf("[ARGV] parse failed: %s\n", argv[i]);
                }
            }
        }
    }
    return (res);
}