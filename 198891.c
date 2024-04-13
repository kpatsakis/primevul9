wc_ucs_to_fullwidth(wc_uint32 ucs)
{
    wc_map *map;

    if (ucs <= WC_C_UCS2_END &&
	(map = wc_map_search((wc_uint16)ucs,
	ucs_fullwidth_map, N_ucs_fullwidth_map)) != NULL)
	return map->code2;
    return WC_C_UCS4_ERROR;
}