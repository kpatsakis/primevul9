wtf_push_iso2022(Str os, wc_ccs ccs, wc_uint32 code)
{
    switch (ccs) {
    case WC_CCS_JIS_C_6226:
    case WC_CCS_JIS_X_0208:
    case WC_CCS_JIS_X_0213_1:
	ccs = wc_jisx0208_or_jisx02131(code);
	break;
    case WC_CCS_JIS_X_0212:
    case WC_CCS_JIS_X_0213_2:
	ccs = wc_jisx0212_or_jisx02132(code);
	break;
    case WC_CCS_JIS_X_0201:
    case WC_CCS_GB_1988:
	ccs = WC_CCS_US_ASCII;
	break;
    }
    wtf_push(os, ccs, code);
}