wc_is_ucs_digit(wc_uint32 ucs)
{
    return (ucs <= WC_C_UCS2_END &&
	wc_map_range_search((wc_uint16)ucs,
	ucs_isdigit_map, N_ucs_isdigit_map) != NULL);
}