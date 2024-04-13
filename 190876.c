wc_push_to_eucjp(Str os, wc_wchar_t cc, wc_status *st)
{
  while (1) {
    switch (cc.ccs) {
    case WC_CCS_US_ASCII:
	Strcat_char(os, (char)cc.code);
	return;
    case WC_CCS_JIS_X_0201K:
	if (WcOption.use_jisx0201k) {
	    Strcat_char(os, WC_C_SS2R);
	    Strcat_char(os, (char)(cc.code | 0x80));
	    return;
	} else if (WcOption.fix_width_conv)
	    cc.ccs = WC_CCS_UNKNOWN;
	else
	    cc = wc_jisx0201k_to_jisx0208(cc);
	continue;
    case WC_CCS_JIS_X_0208:
	break;
    case WC_CCS_JIS_X_0213_1:
	if (WcOption.use_jisx0213)
	    break;
#ifdef USE_UNICODE
	else if (WcOption.ucs_conv && WcOption.use_jisx0212)
	    cc = wc_jisx0213_to_jisx0212(cc);
#endif
	else
	    cc.ccs = WC_CCS_UNKNOWN_W;
	continue;
    case WC_CCS_JIS_X_0212:
	if (WcOption.use_jisx0212) {
	    Strcat_char(os, WC_C_SS3R);
	    break;
	}
#ifdef USE_UNICODE
	else if (WcOption.ucs_conv && WcOption.use_jisx0213)
	    cc = wc_jisx0212_to_jisx0213(cc);
#endif
	else
	    cc.ccs = WC_CCS_UNKNOWN_W;
	continue;
    case WC_CCS_JIS_X_0213_2:
	if (WcOption.use_jisx0213) {
	    Strcat_char(os, WC_C_SS3R);
	    break;
	}
#ifdef USE_UNICODE
	else if (WcOption.ucs_conv && WcOption.use_jisx0212)
	    cc = wc_jisx0213_to_jisx0212(cc);
#endif
	else
	    cc.ccs = WC_CCS_UNKNOWN_W;
	continue;
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
    Strcat_char(os, (char)((cc.code >> 8) | 0x80));
    Strcat_char(os, (char)((cc.code & 0xff) | 0x80));
    return;
  }
}