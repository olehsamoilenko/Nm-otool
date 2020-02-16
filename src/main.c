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

void *get(t_data data, size_t offset, size_t size)
{
	if (offset + size <= data.len)
	{
		return (data.start + offset);
	}
	else
		return (NULL);
}



int parse_mach_o(t_data *data, uint32_t offset)
{
	size_t header_size = data->is64 ? sizeof(struct mach_header_64) : sizeof(struct mach_header);
	void *header = get(*data, offset, header_size);
	if (header == NULL)
	{
		ft_printf("get header failed\n");
		return (EXIT_FAILURE);
	}
	uint32_t ncmds = data->is64	? ((struct mach_header_64 *)header)->ncmds
								: ((struct mach_header *)header)->ncmds;
	ncmds = ntoh(data->cigam, ncmds);

	uint32_t fat_offset = offset;
	offset += header_size;

	data->sections_total = 0; // total sections in mach-o
	// ft_printf("ncmds: %d\n", ncmds);
	while (ncmds)
	{
		struct load_command *lc = get(*data, offset, sizeof(struct load_command));
		if (lc == NULL)
		{
			ft_printf("get lc failed\n");
			return (EXIT_FAILURE);
		}

		if (parse_load_command(data, lc, offset, fat_offset) == EXIT_FAILURE)
		{
			ft_printf("parse_load_command failed\n");
			return (EXIT_FAILURE);
		}

		offset += ntoh(data->cigam, lc->cmdsize);
		ncmds--;
	}

	return (EXIT_SUCCESS);
}

int parse_object(t_data *data, uint32_t offset)
{
	int res = EXIT_FAILURE;
	
	uint32_t magic = *(uint32_t *)get(*data, offset, sizeof(uint32_t));

	if (!magic)
	{
		ft_printf("failed: bad magic\n");
	}
	else if (magic == MH_MAGIC_64)
	{
		#if DEBUG
			ft_printf("MH_MAGIC_64\n");
		#endif
		data->is64 = true;
		data->cigam = false;
		res = parse_mach_o(data, offset);
	}
	else if (magic == MH_MAGIC)
	{
		#if DEBUG
			ft_printf("MH_MAGIC\n");
		#endif
		data->is64 = false;
		data->cigam = false;
		res = parse_mach_o(data, offset);
	}
	else if (magic == MH_CIGAM)
	{
		#if DEBUG
			ft_printf("MH_CIGAM\n");
		#endif
		data->is64 = false;
		data->cigam = true;
		res = parse_mach_o(data, offset);
	}
	else if (magic == MH_CIGAM_64)
	{
		ft_printf("failed: MH_CIGAM_64\n");
	}
	else if (magic == FAT_MAGIC)
	{
		ft_printf("failed: FAT_MAGIC\n");
	}
	else if (magic == FAT_CIGAM)
	{
		#if DEBUG
			ft_printf("FAT_CIGAM\n");
		#endif
		data->is64 = false;
		data->cigam = true;
		res = parse_fat(data);
	}
	else if (magic == FAT_CIGAM_64)
	{
		ft_printf("failed: FAT_CIGAM_64\n");
	}
	else if (magic == *(uint32_t*)ARMAG)
	{
		#if DEBUG
			ft_printf("ARCHIVE\n");
		#endif
		res = parse_archive(data, offset);
	}
	else
	{
		ft_printf("failed: Other\n");
	}
		
	return (res);
}

int parse_file(char *file_name)
{
	int			fd;
	struct stat	stat;
	void		*data_ptr;

	if ((fd = open(file_name, O_RDONLY)) == -1)
		return (EXIT_FAILURE);
	if (fstat(fd, &stat) == -1)
		return (EXIT_FAILURE);
	if (S_ISDIR(stat.st_mode))
		return (EXIT_FAILURE);
	if ((data_ptr = mmap(0, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (EXIT_FAILURE);
	if (close(fd) == -1) // close in case of error
		return (EXIT_FAILURE);

	t_data data;
	data.start = data_ptr;
	data.len = stat.st_size;

	int res = parse_object(&data, 0);

	if (munmap(data_ptr, stat.st_size) == -1) // munmap in case of error
		return (EXIT_FAILURE);

	return (res);
}

int main(int argc, char **argv)
{

	if (argc == 1)
	{
		// a.out
	}
	else
	{
		argv++;
		while (*argv != NULL)
		{
			int res = parse_file(*argv);
			// ft_printf("res: %d\n", res);
			argv++;
		}
	}

	return (EXIT_SUCCESS);
}