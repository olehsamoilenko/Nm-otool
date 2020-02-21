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

int parse_fat(t_data *data)
{
	// Nm only: filename
	struct fat_header *header = get(*data, 0, sizeof(struct fat_header));
	if (header == NULL)
		return (EXIT_FAILURE);
	
	uint32_t narchs = data->cigam ? SWAP_32(header->nfat_arch)
								  : header->nfat_arch;
	#if DEBUG
		ft_printf("FAT narchs: %d\n", narchs);
	#endif
	
	uint32_t offset = sizeof(struct fat_header);
	uint32_t arch_size = data->is64 ? sizeof(struct fat_arch_64)
									: sizeof(struct fat_arch);
									
	bool cigam = data->cigam;
	bool is64 = data->is64;
	int res = EXIT_SUCCESS;
	while (narchs)
	{
		#if DEBUG
			// ft_printf("ARCH %d\n", narchs);
		#endif
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
			#if DEBUG
				// ft_printf("%x %x %x %x\n", test[0], test[1], test[2], test[3]);
			#endif
		}
		
		#if DEBUG
			ft_printf("arch offset: %d, cputype: %d %d\n", arch_offset, cputype, cpusubtype);
		#endif
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