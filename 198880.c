wc_ucs_to_ccs(wc_uint32 ucs)
{
    if (0x80 <= ucs && ucs <= 0x9F)
	return WC_CCS_C1;
    return ((ucs <= WC_C_UCS2_END) ? WC_CCS_UCS2 : WC_CCS_UCS4)
	| ((WcOption.east_asian_width && wc_is_ucs_ambiguous_width(ucs))
		    ? WC_CCS_A_WIDE : 0)
	| (wc_is_ucs_wide(ucs) ? WC_CCS_A_WIDE : 0)
	| (wc_is_ucs_combining(ucs) ? WC_CCS_A_COMB : 0);
}