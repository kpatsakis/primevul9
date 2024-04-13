wc_push_to_iso8859(Str os, wc_wchar_t cc, wc_status *st)
{
    wc_ccs g1_ccs = st->ces_info->gset[1].ccs;

  while (1) {
    if (cc.ccs == g1_ccs) {
	Strcat_char(os, (char)(cc.code | 0x80));
	return;
    }
    switch (cc.ccs) {
    case WC_CCS_US_ASCII:
	Strcat_char(os, (char)cc.code);
	return;
    case WC_CCS_C1:
	Strcat_char(os, (char)(cc.code | 0x80));
	return;
    case WC_CCS_UNKNOWN_W:
	if (!WcOption.no_replace)
	    Strcat_charp(os, WC_REPLACE_W);
	return;
    case WC_CCS_UNKNOWN:
	if (!WcOption.no_replace)
	    Strcat_charp(os, WC_REPLACE);
	return;
    default:
#ifdef USE_UNICODE
	if (WcOption.ucs_conv)
	    cc = wc_any_to_any_ces(cc, st);
	else
#endif
	    cc.ccs = WC_CCS_IS_WIDE(cc.ccs) ? WC_CCS_UNKNOWN_W : WC_CCS_UNKNOWN;
	continue;
    }
  }
}