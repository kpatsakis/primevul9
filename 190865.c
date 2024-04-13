wc_parse_iso2022_esc(wc_uchar **ptr, wc_status *st)
{
    wc_uchar *p = *ptr, state, f = 0, g = 0, cs = 0;

    if (*p != WC_C_ESC)
	return 0;
    state = *p;
    for (p++; *p && state; p++) {
	switch (state) {
	case WC_C_ESC:		/* ESC */
	    switch (*p) {
	    case WC_C_MBCS:	/* ESC '$' */
		state = *p;
		continue;
	    case WC_C_G0_CS94:	/* ESC '(' */
	    case WC_C_G1_CS94:	/* ESC ')' */
	    case WC_C_G2_CS94:	/* ESC '*' */
	    case WC_C_G3_CS94:	/* ESC '+' */
		state = cs = WC_C_G0_CS94;
		g = *p & 0x03;
		continue;
	    case WC_C_G0_CS96:	/* ESC ',' */ /* ISO 2022 does not permit */
	    case WC_C_G1_CS96:	/* ESC '-' */
	    case WC_C_G2_CS96:	/* ESC '.' */
	    case WC_C_G3_CS96:	/* ESC '/' */
		state = cs = WC_C_G0_CS96;
		g = *p & 0x03;
		continue;
	    case WC_C_C0:	/* ESC '!' */ /* not suported */
	    case WC_C_C1:	/* ESC '"' */ /* not suported */
	    case WC_C_REP:	/* ESC '&' */ /* not suported */
		state = cs = WC_C_C0;
		continue;
	    case WC_C_CSWSR:	/* ESC '%' */ /* not suported */
		state = cs = WC_C_CSWSR;
		continue;
	    case WC_C_SS2:	/* ESC 'N' */
		st->ss = 2; *ptr = p; return 1;
	    case WC_C_SS3:	/* ESC 'O' */
		st->ss = 3; *ptr = p; return 1;
	    case WC_C_LS2:	/* ESC 'n' */
		st->gl = 2; *ptr = p; return 1;
	    case WC_C_LS3:	/* ESC 'o' */
		st->gl = 3; *ptr = p; return 1;
	    case WC_C_LS1R:	/* ESC '~' */
		st->gr = 1; *ptr = p; return 1;
	    case WC_C_LS2R:	/* ESC '}' */
		st->gr = 2; *ptr = p; return 1;
	    case WC_C_LS3R:	/* ESC '|' */
		st->gr = 3; *ptr = p; return 1;
	    default:
		return 0;
	    }
	    break;
	case WC_C_MBCS:		/* ESC '$' */
	    switch (*p) {
	    case WC_F_JIS_C_6226:	/* ESC '$' @ */
	    case WC_F_JIS_X_0208:	/* ESC '$' B */
	    case WC_F_GB_2312:	/* ESC '$' A */
		state = 0;
		cs = WC_C_G0_CS94 | 0x80;
		g = 0;
		f = *p;
		break;
	    case WC_C_G0_CS94:	/* ESC '$' '(' */
	    case WC_C_G1_CS94:	/* ESC '$' ')' */
	    case WC_C_G2_CS94:	/* ESC '$' '*' */
	    case WC_C_G3_CS94:	/* ESC '$' '+' */
		state = cs = WC_C_G0_CS94 | 0x80;
		g = *p & 0x03;
		continue;
	    case WC_C_G0_CS96:	/* ESC '$' ',' */ /* ISO 2022 does not permit */
	    case WC_C_G1_CS96:	/* ESC '$' '-' */
	    case WC_C_G2_CS96:	/* ESC '$' '.' */
	    case WC_C_G3_CS96:	/* ESC '$' '/' */
		state = cs = WC_C_G0_CS96 | 0x80;
		g = *p & 0x03;
		continue;
	    default:
		return 0;
	    }
	    break;
	case WC_C_G0_CS94:	/* ESC [()*+] F */
	    if (*p == WC_C_CS942) {	/* ESC [()*+] '!' */
		state = cs = WC_C_CS942 | 0x80;
		g = *p & 0x03;
		continue;
	    }
	case WC_C_G0_CS96:	/* ESC [,-./] F */
	case WC_C_G0_CS94 | 0x80:	/* ESC '$' [()*+] F */
	case WC_C_G0_CS96 | 0x80:	/* ESC '$' [,-./] F */
	case WC_C_CS942 | 0x80:	/* ESC [()*+] '!' F */
	case WC_C_C0:		/* ESC [!"&] F */
	case WC_C_CSWSR | 0x80:	/* ESC '%' '/' F */
	    state = 0;
	    f = *p;
	    break;
	case WC_C_CSWSR:	/* ESC '%' F */
	    if (*p == WC_C_CSWOSR) {	/* ESC '%' '/' */
		state = cs = WC_C_CSWSR | 0x80;
		continue;
	    }
	    state = 0;
	    f = *p;
	    break;
	default:
	    return 0;
	}
    }
    if (f < WC_F_ISO_BASE || f > 0x7e)
	return 0;
    switch (cs) {
    case WC_C_G0_CS94:
	st->design[g] = WC_CCS_SET_CS94(f);
	break;
    case WC_C_G0_CS94 | 0x80:
	st->design[g] = WC_CCS_SET_CS94W(f);
	break;
    case WC_C_G0_CS96:
	st->design[g] = WC_CCS_SET_CS96(f);
	break;
    case WC_C_G0_CS96 | 0x80:
	st->design[g] = WC_CCS_SET_CS96W(f);
	break;
    case WC_C_CS942 | 0x80:
	st->design[g] = WC_CCS_SET_CS942(f);
	break;
    case WC_C_CSWSR:
	if (f == WC_F_ISO_BASE)
	    st->state = WC_ISO_NOSTATE;
	else
	    st->state = WC_ISO_CSWSR;
	break;
    case WC_C_CSWOSR:
	st->state = WC_ISO_CSWOSR;
	break;
    }
    *ptr = p - 1;
    return 1;
}