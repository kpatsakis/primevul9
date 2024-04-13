wc_is_ucs_combining(wc_uint32 ucs)
{
    return (WcOption.use_combining && ucs <= WC_C_UCS2_END &&
	wc_map_range_search((wc_uint16)ucs,
	ucs_combining_map, N_ucs_combining_map) != NULL);
}