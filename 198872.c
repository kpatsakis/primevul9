wc_ucs_to_iso2022w(wc_uint32 ucs)
{
    wc_table *t;
    wc_wchar_t cc;
    int f;

    if (ucs <= WC_C_UCS2_END) {
	for (f = 0; f <= WC_F_CS94W_END - WC_F_ISO_BASE; f++) {
	    t = &ucs_cs94w_table[f];
	    if (t->map == NULL)
		continue;
	    cc = wc_ucs_to_any((wc_uint16)ucs, t);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	}
	for (f = 0; f <= WC_F_CS96W_END - WC_F_ISO_BASE; f++) {
	    t = &ucs_cs96w_table[f];
	    if (t->map == NULL)
		continue;
	    cc = wc_ucs_to_any((wc_uint16)ucs, t);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	}
    }
    cc.ccs = WC_CCS_UNKNOWN_W;
    return cc;
}