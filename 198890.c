wc_ucs_to_any_list(wc_uint32 ucs, wc_table **tlist)
{
    wc_wchar_t cc;
    wc_table **t;

    if (tlist != NULL) {
	for (t = tlist; *t != NULL; t++) {
	    if ((*t)->map == NULL)
		continue;
	    cc = wc_ucs_to_any(ucs, *t);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	}
    }
    cc.ccs = WC_CCS_UNKNOWN;
    return cc;
}