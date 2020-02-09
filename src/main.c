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

typedef struct	s_data
{
	void *start;
	size_t len;
	bool is64;
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
	const size_t seg_cmd_size = data->is64	? sizeof(struct segment_command_64)
											: sizeof(struct segment_command);

	void *seg_cmd = get(*data, offset, sizeof(struct segment_command_64));
	if (seg_cmd == NULL)
		return (EXIT_FAILURE);
	uint32_t nsects = data->is64	? ((struct segment_command_64 *)seg_cmd)->nsects
									: ((struct segment_command *)seg_cmd)->nsects;
	ft_printf("PARSE SEGMENT nsects: %d\n", nsects);
	offset += seg_cmd_size;

	const size_t sec_size = data->is64	? sizeof(struct section_64)
										: sizeof(struct section);
	while (nsects)
	{
		void *section = get(*data, offset, sec_size);
		if (section == NULL)
			return (EXIT_FAILURE);
		int res = parse_section(data, section);
		if (res == EXIT_FAILURE)
			return (EXIT_FAILURE);
		offset += sizeof(struct section_64);
		nsects--;
	}

	return EXIT_SUCCESS;
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

	ft_printf("%s\n", str);

	return (EXIT_SUCCESS);
}

int parse_load_command(t_data *data, struct load_command *lc, uint32_t offset)
{
	const uint32_t cmd_seg = data->is64 ? LC_SEGMENT_64 : LC_SEGMENT;

	if (lc->cmd == cmd_seg)
	{
		int res = parse_segment(data, offset);
		if (res == EXIT_FAILURE)
			return (EXIT_FAILURE);
	}
	else if (lc->cmd == LC_SYMTAB)
	{
		// NM only
		struct symtab_command *sym_cmd = get(*data, offset, sizeof(struct symtab_command));
		if (sym_cmd == NULL)
			return (EXIT_FAILURE);
		uint32_t nsyms = sym_cmd->nsyms;

		const size_t sym_size = data->is64	? sizeof(struct nlist_64)
											: sizeof(struct nlist);
		int new_offset = 0; // replace with offset
		while (nsyms)
		{
			int res = parse_symbol(data, sym_size, sym_cmd->symoff + new_offset, sym_cmd->stroff);
			if (res == EXIT_FAILURE)
				return (EXIT_FAILURE);

			new_offset += sym_size;
			nsyms--;
		}
	}

	return (EXIT_SUCCESS);
}

int parse_mach_o(t_data *data)
{
	size_t header_size = data->is64 ? sizeof(struct mach_header_64) : sizeof(struct mach_header);
	void *header = get(*data, 0, header_size);
	if (header == NULL)
		return (EXIT_FAILURE);
	uint32_t ncmds = data->is64	? ((struct mach_header_64 *)header)->ncmds
								: ((struct mach_header *)header)->ncmds;
	
	ft_printf("ncmds: %d\n", ncmds);

	int offset = header_size;

	
	while (ncmds)
	{
		struct load_command *lc = get(*data, offset, sizeof(struct load_command));
		if (lc == NULL)
			return (EXIT_FAILURE);

		int res = parse_load_command(data, lc, offset);
		if (res == EXIT_FAILURE)
			return (EXIT_FAILURE);

		offset += lc->cmdsize;
		ncmds--;
	}

	return (EXIT_SUCCESS);
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


	int res;
	uint32_t magic = *(uint32_t *)get(data, 0, sizeof(uint32_t));

	if (!magic)
		ft_printf("NULL\n");
	else if (magic == MH_MAGIC_64)
	{
		ft_printf("MH_MAGIC_64\n");
		data.is64 = true;
		res = parse_mach_o(&data);
	}
	else
		ft_printf("Other\n");



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