/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   archive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osamoile <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/16 10:28:05 by osamoile          #+#    #+#             */
/*   Updated: 2020/02/16 10:28:08 by osamoile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

int parse_archive(t_data *data, uint32_t offset)
{
	// ? skip first
	offset += SARMAG;
	struct ar_hdr *hdr = get(*data, offset, sizeof(struct ar_hdr));
	if (hdr == NULL)
	{
		if (DEBUG)
			ft_printf("[ARCHIVE] get first header failed\n");
		return (EXIT_FAILURE);
	}
	offset += sizeof(struct ar_hdr) + ft_atoi(hdr->ar_size);

	while ((hdr = get(*data, offset, sizeof(struct ar_hdr))) != NULL)
	{
		if (DEBUG)
			ft_printf("[ARCHIVE] name: %s\n", hdr->ar_name);

		if (!ft_strnequ(hdr->ar_name, AR_EFMT1, ft_strlen(AR_EFMT1)))
		{
			if (DEBUG)
				ft_printf("[ARCHIVE] failed: no AR_EFMT1\n");
			return (EXIT_FAILURE);
		}

		uint32_t name_len = ft_atoi(hdr->ar_name + ft_strlen(AR_EFMT1));
		char *name = get(*data, offset + sizeof(struct ar_hdr), name_len);
		if (name == NULL)
		{
			if (DEBUG)
				ft_printf("[ARCHIVE] name get failed\n");
			return (EXIT_FAILURE);
		}
		ft_printf("\n%s(%s):\n", data->filename, name);
		if (DEBUG)
			ft_printf("[ARCHIVE] offset: %d\n", offset + sizeof(struct ar_hdr) + name_len);

		int res = parse_object(data, offset + sizeof(struct ar_hdr) + name_len);
		if (res == EXIT_FAILURE)
		{
			if (DEBUG)
				ft_printf("[ARCHIVE] parse object failed\n");
			return (EXIT_FAILURE);
		}

		offset += sizeof(struct ar_hdr) + ft_atoi(hdr->ar_size);
	}

	return (EXIT_SUCCESS);
}
