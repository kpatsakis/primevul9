wc_ucs_precompose(wc_uint32 ucs1, wc_uint32 ucs2)
{
    wc_map3 *map;

    if (WcOption.use_combining &&
	ucs1 <= WC_C_UCS2_END && ucs2 <= WC_C_UCS2_END &&
	(map = wc_map3_search((wc_uint16)ucs1, (wc_uint16)ucs2,
	ucs_precompose_map, N_ucs_precompose_map)) != NULL)
	return map->code3;
    return WC_C_UCS4_ERROR;
}