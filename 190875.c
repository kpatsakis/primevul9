wc_push_iso2022_esc(Str os, wc_ccs ccs, wc_uchar g, wc_uint8 invoke, wc_status *st)
{
    wc_uint8 g_invoke = g & 0x03;

    if (st->design[g_invoke] != ccs) {
	Strcat_char(os, WC_C_ESC);
	if (WC_CCS_IS_WIDE(ccs)) {
	    Strcat_char(os, WC_C_MBCS);
	    if (g_invoke != 0 ||
		(ccs != WC_CCS_JIS_C_6226 &&
		 ccs != WC_CCS_JIS_X_0208 &&
		 ccs != WC_CCS_GB_2312))
		Strcat_char(os, (char)g);
	} else {
	    Strcat_char(os, (char)g);
	    if ((ccs & WC_CCS_A_ISO_2022) == WC_CCS_A_CS942)
		Strcat_char(os, WC_C_CS942);
	}
	Strcat_char(os, (char)WC_CCS_GET_F(ccs));
	st->design[g_invoke] = ccs;
    }
    if (! invoke)
	return;

    switch (g_invoke) {
    case 0:
	if (st->gl != 0) {
	    Strcat_char(os, WC_C_SI);
	    st->gl = 0;
	}
	break;
    case 1:
	if (st->gl != 1) {
	    Strcat_char(os, WC_C_SO);
	    st->gl = 1;
	}
	break;
    case 2:
	Strcat_char(os, WC_C_ESC);
	Strcat_char(os, WC_C_SS2);
	break;
    case 3:
	Strcat_char(os, WC_C_ESC);
	Strcat_char(os, WC_C_SS3);
	break;
    }
}