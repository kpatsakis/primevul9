wc_is_ucs_ambiguous_width(wc_uint32 ucs)
{
    if (0xa1 <= ucs && ucs <= 0xfe && WcOption.use_jisx0213)
	return 1;
    else if (ucs <= WC_C_UCS2_END)
	return (wc_map_range_search((wc_uint16)ucs,
		    ucs_ambwidth_map, N_ucs_ambwidth_map) != NULL);
    else
	return ((0xF0000 <= ucs && ucs <= 0xFFFFD)
		|| (0x100000 <= ucs && ucs <= 0x10FFFD));
}