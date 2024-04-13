wc_push_to_euctw(Str os, wc_wchar_t cc, wc_status *st)
{
  while (1) {
    switch (cc.ccs) {
    case WC_CCS_US_ASCII:
	Strcat_char(os, (char)cc.code);
	return;
    case WC_CCS_CNS_11643_1:
	break;
    case WC_CCS_CNS_11643_2:
    case WC_CCS_CNS_11643_3:
    case WC_CCS_CNS_11643_4:
    case WC_CCS_CNS_11643_5:
    case WC_CCS_CNS_11643_6:
    case WC_CCS_CNS_11643_7:
	Strcat_char(os, WC_C_SS2R);
	Strcat_char(os, (char)(0xA1 + (cc.ccs - WC_CCS_CNS_11643_1)));
	break;
    case WC_CCS_CNS_11643_8:
    case WC_CCS_CNS_11643_9:
    case WC_CCS_CNS_11643_10:
    case WC_CCS_CNS_11643_11:
    case WC_CCS_CNS_11643_12:
    case WC_CCS_CNS_11643_13:
    case WC_CCS_CNS_11643_14:
    case WC_CCS_CNS_11643_15:
    case WC_CCS_CNS_11643_16:
	Strcat_char(os, WC_C_SS2R);
	Strcat_char(os, (char)(0xA8 + (cc.ccs - WC_CCS_CNS_11643_8)));
	break;
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