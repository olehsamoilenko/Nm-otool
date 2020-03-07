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
		{
			cputype = SWAP_32(cputype);
		}
		
		if (cputype == CPU_TYPE_X86_64)
		{
			res = false;
			if (DEBUG)
				ft_printf("[FAT] Found correct arch: %d. Showing all arches disabled.\n", cputype);
			break ;
		}
		
		offset += arch_size;
		narchs--;
	}

	if (DEBUG)
		ft_printf("[FAT] Correct arch wasn't found. Showing all arches enabled.\n");
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
	else
	{
		if (DEBUG)
			ft_printf("Unknown cputype\n");
		return ("");
	}
}

int parse_fat(t_data *data)
{
	// Nm only: filename
	struct fat_header *header = get(*data, 0, sizeof(struct fat_header));
	if (header == NULL)
		return (EXIT_FAILURE);
	
	uint32_t narchs = data->cigam ? SWAP_32(header->nfat_arch)
								  : header->nfat_arch;
	if (DEBUG)
		ft_printf("[FAT] narchs: %d\n", narchs);
	
	uint32_t offset = sizeof(struct fat_header);
	uint32_t arch_size = data->is64 ? sizeof(struct fat_arch_64)
									: sizeof(struct fat_arch);
									
	bool cigam = data->cigam;
	bool is64 = data->is64;
	int res = EXIT_SUCCESS;

	bool show_all = show_all_enabled(narchs, data, offset, arch_size);
	while (narchs)
	{
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
			// works ?
			// char *test = &(cpusubtype);
			// ft_printf("%x %x %x %x\n", test[0], test[1], test[2], test[3]);
		}
		
		if (DEBUG)
			ft_printf("[FAT] arch offset: %d, cputype: %d, cpusubtype: %d\n", arch_offset, cputype, cpusubtype);

		if (show_all
			|| (cputype == CPU_TYPE_X86_64 && cpusubtype == CPU_SUBTYPE_X86_64_ALL)) // wat ?
		{
			if (show_all)
				ft_printf("\n%s (for architecture %s):\n", data->filename, get_arch_name(cputype, cpusubtype));
			res = parse_object(data, arch_offset);
			if (res == EXIT_FAILURE)
			{
				if (DEBUG)
					ft_printf("[FAT] parse object failed\n");
				return (EXIT_FAILURE);
			}
		}
		else if (DEBUG)
			ft_printf("[FAT] Incorrect arch. type: %d, subtype: %d\n", cputype, cpusubtype);
		
		offset += arch_size;
		narchs--;
	}

	return (res);
}
