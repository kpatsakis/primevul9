wc_any_to_any(wc_wchar_t cc, wc_table *t)
{
    wc_ccs is_wide = WC_CCS_IS_WIDE(cc.ccs);
    wc_uint32 ucs = wc_any_to_ucs(cc);

    if (ucs != WC_C_UCS4_ERROR) {
	cc = wc_ucs_to_any(ucs, t);
	if (!WC_CCS_IS_UNKNOWN(cc.ccs))
	    return cc;

	ucs = wc_ucs_to_fullwidth(ucs);
	if (ucs != WC_C_UCS4_ERROR) {
	    cc = wc_ucs_to_any(ucs, t);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	}
    }
    cc.ccs = is_wide ? WC_CCS_UNKNOWN_W : WC_CCS_UNKNOWN;
    return cc;
}