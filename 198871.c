wc_ucs_totitle(wc_uint32 ucs)
{
    wc_map *conv = NULL;
    if (ucs <= WC_C_UCS2_END)
	conv = wc_map_search((wc_uint16)ucs,
			     ucs_totitle_map, N_ucs_totitle_map);
    return conv ? (wc_uint32)(conv->code2) : ucs;
}