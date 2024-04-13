wc_any_to_any_ces(wc_wchar_t cc, wc_status *st)
{
    wc_uint32 ucs = wc_any_to_ucs(cc);
    wc_ccs is_wide = WC_CCS_IS_WIDE(cc.ccs);

    if (ucs < 0x80) {
	cc.ccs = WC_CCS_US_ASCII;
	cc.code = ucs;
	return cc;
    }
    if (ucs != WC_C_UCS4_ERROR) {
	if (st->ces_info->id & WC_CES_T_UTF) {
	    cc.ccs = wc_ucs_to_ccs(ucs);
	    cc.code = ucs;
	    return cc;
	} else if (st->ces_info->id == WC_CES_JOHAB) {
	    cc = wc_ucs_to_johab(ucs);
	    if (WC_CCS_IS_UNKNOWN(cc.ccs))
		cc.ccs = is_wide ? WC_CCS_UNKNOWN_W : WC_CCS_UNKNOWN;
	    return cc;
	}
	cc = wc_ucs_to_any_list(ucs, is_wide ? st->tlistw : st->tlist);
	if (!WC_CCS_IS_UNKNOWN(cc.ccs))
	    return cc;
	if (! WcOption.fix_width_conv) {
	    cc = wc_ucs_to_any_list(ucs, is_wide ? st->tlist : st->tlistw);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	}
	if (st->ces_info->id == WC_CES_GB18030) {
	    cc = wc_ucs_to_gb18030(ucs);
	    if (WC_CCS_IS_UNKNOWN(cc.ccs))
		cc.ccs = is_wide ? WC_CCS_UNKNOWN_W : WC_CCS_UNKNOWN;
	    return cc;
	}
	if (ucs == WC_C_UCS2_NBSP) {	/* NBSP -> SP */
	    cc.ccs = WC_CCS_US_ASCII;
	    cc.code = 0x20;
	    return cc;
	}
	if (st->ces_info->id & (WC_CES_T_ISO_8859|WC_CES_T_EUC) &&
	    0x80 <= ucs && ucs <= 0x9F) {
	    cc.ccs = WC_CCS_C1;
	    cc.code = ucs;
	    return cc;
	}

	ucs = wc_ucs_to_fullwidth(ucs);
	if (ucs != WC_C_UCS4_ERROR) {
	    cc = wc_ucs_to_any_list(ucs, is_wide ? st->tlistw : st->tlist);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	    if (! WcOption.fix_width_conv) {
		cc = wc_ucs_to_any_list(ucs, is_wide ? st->tlist : st->tlistw);
		if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		    return cc;
	    }
	}
    }
    cc.ccs = is_wide ? WC_CCS_UNKNOWN_W : WC_CCS_UNKNOWN;
    return cc;
}