wc_ucs_to_iso2022(wc_uint32 ucs)
{
    wc_table *t;
    wc_wchar_t cc;
    int f;

    if (ucs <= WC_C_UCS2_END) {
	for (f = 0; f <= WC_F_CS96_END - WC_F_ISO_BASE; f++) {
	    t = &ucs_cs96_table[f];
	    if (t->map == NULL)
		continue;
	    cc = wc_ucs_to_any((wc_uint16)ucs, t);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	}
	for (f = 0; f <= WC_F_CS94_END - WC_F_ISO_BASE; f++) {
	    t = &ucs_cs94_table[f];
	    if (t->map == NULL)
		continue;
	    cc = wc_ucs_to_any((wc_uint16)ucs, t);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	}
	for (f = 0; f <= WC_F_CS942_END - WC_F_ISO_BASE; f++) {
	    t = &ucs_cs942_table[f];
	    if (t->map == NULL)
		continue;
	    cc = wc_ucs_to_any((wc_uint16)ucs, t);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	}
    }
    cc.ccs = WC_CCS_UNKNOWN;
    return cc;
}