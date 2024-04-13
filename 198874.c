wc_any_to_iso2022(wc_wchar_t cc, wc_status *st)
{
    wc_uint32 ucs = wc_any_to_ucs(cc);
    wc_ccs is_wide = WC_CCS_IS_WIDE(cc.ccs);

    if (ucs < 0x80) {
	cc.ccs = WC_CCS_US_ASCII;
	cc.code = ucs;
	return cc;
    }
    if (ucs != WC_C_UCS4_ERROR) {
	cc = wc_ucs_to_any_list(ucs, is_wide ? st->tlistw : st->tlist);
	if (!WC_CCS_IS_UNKNOWN(cc.ccs))
	    return cc;
	if (! WcOption.strict_iso2022) {
	    cc = (is_wide) ? wc_ucs_to_iso2022w(ucs) : wc_ucs_to_iso2022(ucs);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	}
	if (! WcOption.fix_width_conv) {
	    cc = wc_ucs_to_any_list(ucs, is_wide ? st->tlist : st->tlistw);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	    if (! WcOption.strict_iso2022) {
		cc = (is_wide) ? wc_ucs_to_iso2022(ucs) : wc_ucs_to_iso2022w(ucs);
		if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		    return cc;
	    }
	}
	if (ucs == WC_C_UCS2_NBSP) {	/* NBSP -> SP */
	   cc.ccs = WC_CCS_US_ASCII;
	   cc.code = 0x20;
	   return cc;
	}

	ucs = wc_ucs_to_fullwidth(ucs);
	if (ucs != WC_C_UCS4_ERROR) {
	    cc = wc_ucs_to_any_list(ucs, is_wide ? st->tlistw : st->tlist);
	    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		return cc;
	    if (! WcOption.strict_iso2022) {
		cc = (is_wide) ? wc_ucs_to_iso2022w(ucs) : wc_ucs_to_iso2022(ucs);
		if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		    return cc;
	    }
	    if (! WcOption.fix_width_conv) {
		cc = wc_ucs_to_any_list(ucs, is_wide ? st->tlist : st->tlistw);
		if (!WC_CCS_IS_UNKNOWN(cc.ccs))
		    return cc;
		if (! WcOption.strict_iso2022) {
		    cc = (is_wide) ? wc_ucs_to_iso2022(ucs) : wc_ucs_to_iso2022w(ucs);
		    if (!WC_CCS_IS_UNKNOWN(cc.ccs))
			return cc;
		}
	    }
	}
	if (ucs == WC_C_UCS2_NBSP) {	/* NBSP -> SP */
	   cc.ccs = WC_CCS_US_ASCII;
	   cc.code = 0x20;
	   return cc;
	}
    }
    cc.ccs = is_wide ? WC_CCS_UNKNOWN_W : WC_CCS_UNKNOWN;
    return cc;
}