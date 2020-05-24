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
    offset += SARMAG;
    struct ar_hdr *hdr = get(*data, offset, sizeof(struct ar_hdr));
    if (hdr == NULL)
        return (EXIT_FAILURE);
    offset += sizeof(struct ar_hdr) + ft_atoi(hdr->ar_size);
    if (OTOOL)
        ft_printf("Archive : %s\n", data->filename);
    while ((hdr = get(*data, offset, sizeof(struct ar_hdr))) != NULL)
    {
        if (!ft_strnequ(hdr->ar_name, AR_EFMT1, ft_strlen(AR_EFMT1)))
            return (EXIT_SUCCESS);
        uint32_t name_len = ft_atoi(hdr->ar_name + ft_strlen(AR_EFMT1));
        char *name = get(*data, offset + sizeof(struct ar_hdr), name_len);
        if (name == NULL)
            return (EXIT_FAILURE);
        char *label = data->filename;
        label = ft_strjoin(label, "(");
        label = ft_strjoin(label, name);
        label = ft_strjoin(label, "):\n");
        if (NM)
            label = ft_strjoin("\n", label);
        int res = parse_object(data, offset + sizeof(struct ar_hdr) + name_len, label);
        if (res == EXIT_FAILURE)
            return (EXIT_FAILURE);
        offset += sizeof(struct ar_hdr) + ft_atoi(hdr->ar_size);
    }
    return (EXIT_SUCCESS);
}
