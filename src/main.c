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

# define SWAP_16(x) ((((x) & 0xff00U) >> 8U) | (((x) & 0x00ffU) << 8U))
# define SWAP_32(x) ((SWAP_16(x) << 16U) | (SWAP_16((x) >> 16U)))
# define SWAP_64(x) ((SWAP_32(x) << 32U) | (SWAP_32((x) >> 32U)))

uint32_t ntoh(bool cigam, uint32_t nbr)
{
	if (cigam)
		nbr = SWAP_32(nbr);
	return (nbr);
}

typedef struct	s_data
{
	void *start;
	size_t len;
	bool is64;
	bool cigam;
}				t_data;

void *get(t_data data, size_t offset, size_t size)
{
	if (offset + size <= data.len)
	{
		return (data.start + offset);
	}
	else
		return (NULL);
}

int parse_section(t_data *data, void *section)
{
	char *sectname = data->is64 ?	((struct section_64 *)section)->sectname
								:	((struct section *)section)->sectname;
	ft_printf("PARSE SECTION %s\n", sectname);
	return (EXIT_SUCCESS);
}

int parse_segment(t_data *data, uint32_t offset)
{
	int res = EXIT_SUCCESS;
	const size_t seg_cmd_size = data->is64	? sizeof(struct segment_command_64)
											: sizeof(struct segment_command);

	void *seg_cmd = get(*data, offset, sizeof(struct segment_command_64));
	if (seg_cmd == NULL)
	{
		ft_printf("get seg cmd failed\n");
		return (EXIT_FAILURE);
	}
	uint32_t nsects = data->is64	? ((struct segment_command_64 *)seg_cmd)->nsects
									: ((struct segment_command *)seg_cmd)->nsects;
	ft_printf("SEGMENT nsects: %d\n", nsects);
	offset += seg_cmd_size;

	const size_t sec_size = data->is64	? sizeof(struct section_64)
										: sizeof(struct section);
	while (nsects)
	{
		void *section = get(*data, offset, sec_size);
		if (section == NULL)
		{
			ft_printf("get section failed\n");
			return (EXIT_FAILURE);
		}
		int res = parse_section(data, section);
		if (res == EXIT_FAILURE)
		{
			ft_printf("parse section failed\n");
			return (EXIT_FAILURE);
		}
		offset += sec_size;
		nsects--;
	}

	return res;
}

int parse_symbol(t_data *data, size_t sym_size, uint32_t offset, uint32_t stroff)
{
	void *sym = get(*data, offset, sym_size);
	if (sym == NULL)
		return (EXIT_FAILURE);

	uint32_t type = data->is64	? ((struct nlist_64 *)sym)->n_type
								: ((struct nlist *)sym)->n_type;
	uint32_t n_strx = data->is64	? ((struct nlist_64 *)sym)->n_un.n_strx
									: ((struct nlist *)sym)->n_un.n_strx;

	char *str = get(*data, stroff + n_strx, 0);
	if (str == NULL)
		return (EXIT_FAILURE);

	ft_printf("SYMBOL: type: %d, strx: %d, str: %s\n", type, n_strx, str);

	return (EXIT_SUCCESS);
}

int parse_load_command(t_data *data, struct load_command *lc, uint32_t offset, uint32_t global_offset)
{
	const uint32_t cmd_seg = data->is64 ? LC_SEGMENT_64 : LC_SEGMENT;

	if (lc->cmd == cmd_seg)
	{
		int res = parse_segment(data, offset);
		if (res == EXIT_FAILURE)
		{
			ft_printf("parse segment failed\n");
			return (EXIT_FAILURE);
		}
	}
	else if (lc->cmd == LC_SYMTAB)
	{
		// NM only
		struct symtab_command *sym_cmd = get(*data, offset, sizeof(struct symtab_command));
		if (sym_cmd == NULL)
			return (EXIT_FAILURE);
		uint32_t nsyms = sym_cmd->nsyms;
		
		ft_printf("PARSE SYMTAB nsyms: %d, symoff: %d, stroff: %d\n", nsyms, sym_cmd->symoff, sym_cmd->stroff);

		const size_t sym_size = data->is64	? sizeof(struct nlist_64)
											: sizeof(struct nlist);
		int new_offset = 0; // replace with offset

		while (nsyms)
		{
			int res = parse_symbol(data, sym_size, global_offset + sym_cmd->symoff + new_offset, global_offset + sym_cmd->stroff);

			if (res == EXIT_FAILURE)
			{
				ft_printf("parse symbol failed\n");
				return (EXIT_FAILURE);
			}

			new_offset += sym_size;
			nsyms--;
		}
	}

	return (EXIT_SUCCESS);
}

int parse_mach_o(t_data *data, uint32_t offset)
{
	size_t header_size = data->is64 ? sizeof(struct mach_header_64) : sizeof(struct mach_header);
	void *header = get(*data, offset, header_size);
	if (header == NULL)
		return (EXIT_FAILURE);
	uint32_t ncmds = data->is64	? ((struct mach_header_64 *)header)->ncmds
								: ((struct mach_header *)header)->ncmds;
	ncmds = ntoh(data->cigam, ncmds);

	uint32_t fat_offset = offset;
	offset += header_size;

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

int parse_object(t_data *data, uint32_t offset);

int parse_fat(t_data *data)
{
	// Nm only: filename
	struct fat_header *header = get(*data, 0, sizeof(struct fat_header));
	if (header == NULL)
		return (EXIT_FAILURE);
	
	uint32_t narchs = data->cigam ? SWAP_32(header->nfat_arch)
								  : header->nfat_arch;
	ft_printf("PARSE FAT narchs: %d\n", narchs);
	
	uint32_t offset = sizeof(struct fat_header);
	uint32_t arch_size = data->is64 ? sizeof(struct fat_arch_64)
									: sizeof(struct fat_arch);
									
	bool cigam = data->cigam;
	bool is64 = data->is64;
	int res = EXIT_SUCCESS;
	while (narchs)
	{
		ft_printf("ARCH %d\n", narchs);
		void *arch = get(*data, offset, arch_size);
		if (arch == NULL)
			return (EXIT_FAILURE);
		
		data->cigam = cigam;
		data->is64 = is64;
		
		uint32_t arch_offset = data->is64 ? ((struct fat_arch_64 *)arch)->offset
										  : ((struct fat_arch *)arch)->offset;
		cpu_type_t cputype = data->is64 ? ((struct fat_arch_64 *)arch)->cputype
										: ((struct fat_arch *)arch)->cputype;
		cpu_subtype_t cpusubtype = data->is64 ? ((struct fat_arch_64 *)arch)->cpusubtype
										      : ((struct fat_arch *)arch)->cpusubtype;

		if (data->cigam) {
			arch_offset = SWAP_32(arch_offset); // TODO: 64
			cputype = SWAP_32(cputype);
			cpusubtype = SWAP_32(cpusubtype);
			cpusubtype &= 0x0000ffff;
			// char *test = &(cpusubtype);
			// ft_printf("%x %x %x %x\n", test[0], test[1], test[2], test[3]);
		}
		
		ft_printf("arch offset: %d, cputype: %d %d\n", arch_offset, cputype, cpusubtype);
		// if (cputype == CPU_TYPE_X86_64 && cpusubtype == CPU_SUBTYPE_X86_64_ALL)
		// {
			res = parse_object(data, arch_offset);
			if (res == EXIT_FAILURE)
			{
				ft_printf("parse object failed: %d\n", res);
				return (EXIT_FAILURE);
			}
		// }
		
		offset += arch_size;
		narchs--;
	}

	return (res);
}

int parse_object(t_data *data, uint32_t offset)
{
	int res = EXIT_FAILURE;
	
	uint32_t magic = *(uint32_t *)get(*data, offset, sizeof(uint32_t));

	if (!magic)
	{
		ft_printf("bad magic\n");
	}
	else if (magic == MH_MAGIC_64)
	{
		ft_printf("MH_MAGIC_64\n");
		data->is64 = true;
		data->cigam = false;
		res = parse_mach_o(data, offset);
	}
	else if (magic == MH_MAGIC)
	{
		ft_printf("MH_MAGIC\n");
		data->is64 = false;
		data->cigam = false;
		res = parse_mach_o(data, offset);
	}
	else if (magic == MH_CIGAM)
	{
		ft_printf("MH_CIGAM\n");
		data->is64 = false;
		data->cigam = true;
		res = parse_mach_o(data, offset);
	}
	else if (magic == MH_CIGAM_64)
	{
		ft_printf("MH_CIGAM_64\n");
	}
	else if (magic == FAT_MAGIC)
	{
		ft_printf("FAT_MAGIC\n");
	}
	else if (magic == FAT_CIGAM)
	{
		ft_printf("FAT_CIGAM\n");
		data->is64 = false;
		data->cigam = true;
		res = parse_fat(data);
	}
	else if (magic == FAT_CIGAM_64)
	{
		ft_printf("FAT_CIGAM_64\n");
	}
	else
		ft_printf("Other\n");
		
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
			ft_printf("res: %d\n", res);
			argv++;
		}
	}

	return (EXIT_SUCCESS);
}