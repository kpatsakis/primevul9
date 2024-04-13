wc_char_conv_from_iso2022(wc_uchar c, wc_status *st)
{
    static Str os;
    static wc_uchar buf[4];
    static size_t nbuf;
    wc_uchar *p;
    wc_ccs gl_ccs, gr_ccs;

    if (st->state == -1) {
	st->state = WC_ISO_NOSTATE;
	os = Strnew_size(8);
	nbuf = 0;
    }

    gl_ccs = st->ss ? st->design[st->ss] : st->design[st->gl];
    gr_ccs = st->ss ? st->design[st->ss] : st->design[st->gr];

    switch (st->state) {
    case WC_ISO_NOSTATE:
	switch (WC_ISO_MAP[c]) {
	case GL2:
	    if (!(WC_CCS_TYPE(gl_ccs) & WC_CCS_A_CS96)) {
		Strcat_char(os, (char)c);
		break;
	    }
	case GL:
	    if (WC_CCS_IS_WIDE(gl_ccs)) {
		buf[nbuf++] = c;
		st->state = WC_ISO_MBYTE1;
		return NULL;
	    } else if (gl_ccs == WC_CES_US_ASCII)
		Strcat_char(os, (char)c);
	    else
		wtf_push_iso2022(os, gl_ccs, (wc_uint32)c);
	    break;
	case GR2:
	    if (!(WC_CCS_TYPE(gr_ccs) & WC_CCS_A_CS96))
		break;
	case GR:
	    if (WC_CCS_IS_WIDE(gr_ccs)) {
		buf[nbuf++] = c;
		st->state = WC_EUC_MBYTE1;
		return NULL;
	    } else if (gr_ccs)
		wtf_push_iso2022(os, gr_ccs, (wc_uint32)c);
	    break;
	case C0:
	    Strcat_char(os, (char)c);
	    break;
	case C1:
	    break;
	case ESC:
	    buf[nbuf++] = c;
	    st->state = WC_C_ESC;
	    return NULL;
	case SI:
	    st->gl = 0;
	    break;
	case SO:
	    st->gl = 1;
	    break;
	case SS2:
	    if (! st->design[2])
		return os;
	    st->ss = 2;
	    return NULL;
	case SS3:
	    if (! st->design[3])
		return os;
	    st->ss = 3;
	    return NULL;
	}
	break;
    case WC_ISO_MBYTE1:
	switch (WC_ISO_MAP[c]) {
	case GL2:
	    if (!(WC_CCS_TYPE(gl_ccs) & WC_CCS_A_CS96))
		break;
	case GL:
	    buf[nbuf++] = c;
	    wtf_push_iso2022(os, gl_ccs, ((wc_uint32)buf[0] << 8) | buf[1]);
	    break;
	}
	st->state = WC_ISO_NOSTATE;
	break;
    case WC_EUC_MBYTE1:
	switch (WC_ISO_MAP[c]) {
	case GR2:
	    if (!(WC_CCS_TYPE(gr_ccs) & WC_CCS_A_CS96))
		break;
	case GR:
	    if (gr_ccs == WC_CCS_CNS_11643_X) {
		buf[nbuf++] = c;
		st->state = WC_EUC_TW_MBYTE2;
		return NULL;
	    }
	    buf[nbuf++] = c;
	    wtf_push_iso2022(os, gr_ccs, ((wc_uint32)buf[0] << 8) | buf[1]);
	    break;
	}
	st->state = WC_ISO_NOSTATE;
	break;
    case WC_EUC_TW_MBYTE2:
	if (WC_ISO_MAP[c] == GR) {
	    buf[nbuf++] = c;
	    c = buf[0];
	    if (0xa1 <= c && c <= 0xa7) {
		wtf_push_iso2022(os, WC_CCS_CNS_11643_1 + (c - 0xa1),
			((wc_uint32)buf[1] << 8) | buf[2]);
		break;
	    }
	    if (0xa8 <= c && c <= 0xb0) {
		wtf_push_iso2022(os, WC_CCS_CNS_11643_8 + (c - 0xa8),
			((wc_uint32)buf[1] << 8) | buf[2]);
		break;
	    }
	}
	st->state = WC_ISO_NOSTATE;
	break;
    case WC_C_ESC:
	switch (c) {
	case WC_C_G0_CS94:
	case WC_C_G1_CS94:
	case WC_C_G2_CS94:
	case WC_C_G3_CS94:
	    buf[nbuf++] = c;
	    st->state = WC_C_G0_CS94;
	    return NULL;
	case WC_C_G0_CS96:
	case WC_C_G1_CS96:
	case WC_C_G2_CS96:
	case WC_C_G3_CS96:
	case WC_C_C0:
	case WC_C_C1:
	case WC_C_REP:
	    buf[nbuf++] = c;
	    st->state = WC_C_G0_CS96;
	    return NULL;
	case WC_C_MBCS:
	case WC_C_CSWSR:
	    buf[nbuf++] = c;
	    st->state = c;
	    return NULL;
	case WC_C_SS2:
	    st->ss = 2;
	    st->state = WC_ISO_NOSTATE;
	    return NULL;
	case WC_C_SS3:
	    st->ss = 3;
	    st->state = WC_ISO_NOSTATE;
	    return NULL;
	case WC_C_LS2:
	    st->gl = 2;
	    break;
	case WC_C_LS3:
	    st->gl = 3;
	    break;
	case WC_C_LS2R:
	    st->gr = 2;
	    break;
	case WC_C_LS3R:
	    st->gr = 3;
	    break;
	default:
	    break;
	}
	break;
    case WC_C_MBCS:
	switch (c) {
	case WC_F_JIS_C_6226:
	case WC_F_JIS_X_0208:
	case WC_F_GB_2312:
	    buf[nbuf++] = c;
	    p = buf;
	    wc_parse_iso2022_esc(&p, st);
	    break;
	case WC_C_G0_CS94:
	case WC_C_G1_CS94:
	case WC_C_G2_CS94:
	case WC_C_G3_CS94:
	case WC_C_G0_CS96:
	case WC_C_G1_CS96:
	case WC_C_G2_CS96:
	case WC_C_G3_CS96:
	    buf[nbuf++] = c;
	    st->state = WC_C_G0_CS96;
	    return NULL;
	}
	break;
    case WC_C_CSWSR:
	switch (c) {
	case WC_C_CSWOSR:
	    buf[nbuf++] = c;
	    st->state = WC_C_G1_CS94;
	    return NULL;
	}
	buf[nbuf++] = c;
	p = buf;
	wc_parse_iso2022_esc(&p, st);
	break;
    case WC_C_G0_CS94:
	switch (c) {
	case WC_C_CS942:
	    buf[nbuf++] = c;
	    st->state = WC_C_G0_CS96;
	    return NULL;
	}
    case WC_C_G0_CS96:
	buf[nbuf++] = c;
	p = buf;
	wc_parse_iso2022_esc(&p, st);
	break;
    }
    st->ss = 0;
    st->state = -1;
    return os;
}