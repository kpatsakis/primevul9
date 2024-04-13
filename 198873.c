wc_ucs_to_any(wc_uint32 ucs, wc_table *t)
{
    wc_wchar_t cc;
    wc_map *map;

    if (t && t->map && ucs && ucs <= WC_C_UCS2_END) {
	map = wc_map_search((wc_uint16)ucs, t->map, t->n);
	if (map)
	    return t->conv(t->ccs, map->code2);
    }
    if (t && (ucs & ~0xFFFF) == WC_C_UCS4_PLANE2) {
	if (t->ccs == WC_CCS_JIS_X_0213_1)
	    map = wc_map_search((wc_uint16)(ucs & 0xffff),
		ucs_p2_jisx02131_map, N_ucs_p2_jisx02131_map);
	else if (t->ccs == WC_CCS_JIS_X_0213_2)
	    map = wc_map_search((wc_uint16)(ucs & 0xffff),
		ucs_p2_jisx02132_map, N_ucs_p2_jisx02132_map);
	else if (t->ccs == WC_CCS_HKSCS ||
		 t->ccs == WC_CCS_HKSCS_1 || t->ccs == WC_CCS_HKSCS_2)
	    map = wc_map_search((wc_uint16)(ucs & 0xffff),
		ucs_p2_hkscs_map, N_ucs_p2_hkscs_map);
	else
	    map = NULL;
	if (map)
	    return t->conv(t->ccs, map->code2);
    }
    cc.ccs = WC_CCS_UNKNOWN;
    cc.code = 0;
    return cc;
}