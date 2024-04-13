wc_is_ucs_wide(wc_uint32 ucs)
{
    if (ucs <= WC_C_UCS2_END)
	return (wc_map_range_search((wc_uint16)ucs,
		ucs_wide_map, N_ucs_wide_map) != NULL);
    else
	return ((ucs & ~0xFFFF) == WC_C_UCS4_PLANE2 ||
		(ucs & ~0xFFFF) == WC_C_UCS4_PLANE3);
}