wc_conv_from_iso2022(Str is, wc_ces ces)
{
    Str os;
    wc_uchar *sp = (wc_uchar *)is->ptr;
    wc_uchar *ep = sp + is->length;
    wc_uchar *p, *q = NULL;
    int state = WC_ISO_NOSTATE;
    wc_status st;
    wc_ccs gl_ccs, gr_ccs;

    for (p = sp; p < ep && !(WC_ISO_MAP[*p] & WC_ISO_MAP_DETECT); p++)
	;
    if (p == ep)
	return is;
    os = Strnew_size(is->length);
    if (p > sp)
	Strcat_charp_n(os, is->ptr, (int)(p - sp));

    wc_input_init(ces, &st);
    gl_ccs = st.design[st.gl];
    gr_ccs = st.design[st.gr];

    for (; p < ep; p++) {
	switch (state) {
	case WC_ISO_NOSTATE:
	    switch (WC_ISO_MAP[*p]) {
	    case GL2:
		gl_ccs = st.ss ? st.design[st.ss]
			: st.design[st.gl];
		if (!(WC_CCS_TYPE(gl_ccs) & WC_CCS_A_CS96)) {
		    Strcat_char(os, (char)*p);
		    break;
		}
	    case GL:
		gl_ccs = st.ss ? st.design[st.ss]
			: st.design[st.gl];
		if (WC_CCS_IS_WIDE(gl_ccs)) {
		    q = p;
		    state = WC_ISO_MBYTE1;
		    continue;
		} else if (gl_ccs == WC_CES_US_ASCII)
		    Strcat_char(os, (char)*p);
		else
		    wtf_push_iso2022(os, gl_ccs, (wc_uint32)*p);
		break;
	    case GR2:
		gr_ccs = st.ss ? st.design[st.ss]
			: st.design[st.gr];
		if (!(WC_CCS_TYPE(gr_ccs) & WC_CCS_A_CS96)) {
		    wtf_push_unknown(os, p, 1);
		    break;
		}
	    case GR:
		gr_ccs = st.ss ? st.design[st.ss]
			: st.design[st.gr];
		if (WC_CCS_IS_WIDE(gr_ccs)) {
		    q = p;
		    state = WC_EUC_MBYTE1;
		    continue;
		} else if (gr_ccs)
		    wtf_push_iso2022(os, gr_ccs, (wc_uint32)*p);
		else
		    wtf_push_unknown(os, p, 1);
		break;
	    case C0:
		Strcat_char(os, (char)*p);
		break;
	    case C1:
		wtf_push(os, WC_CCS_C1, (wc_uint32)*p);
		break;
	    case ESC:
		st.ss = 0;
		if (wc_parse_iso2022_esc(&p, &st))
		    state = st.state;
		else
		    Strcat_char(os, (char)*p);
		continue;
	    case SI:
		st.gl = 0;
		break;
	    case SO:
		st.gl = 1;
		break;
	    case SS2:
		if (! st.design[2]) {
		    wtf_push_unknown(os, p, 1);
		    break;
		}
		st.ss = 2;
		continue;
	    case SS3:
		if (! st.design[3]) {
		    wtf_push_unknown(os, p, 1);
		    break;
		}
		st.ss = 3;
		continue;
	    }
	    break;
	case WC_ISO_MBYTE1:
	    switch (WC_ISO_MAP[*p]) {
	    case GL2:
		if (!(WC_CCS_TYPE(gl_ccs) & WC_CCS_A_CS96)) {
		    Strcat_char(os, (char)*q);
		    Strcat_char(os, (char)*p);
		    break;
		}
	    case GL:
		wtf_push_iso2022(os, gl_ccs, ((wc_uint32)*q << 8) | *p);
		break;
	    default:
		wtf_push_unknown(os, q, 2);
		break;
	    }
	    break;
	case WC_EUC_MBYTE1:
	    switch (WC_ISO_MAP[*p]) {
	    case GR2:
		if (!(WC_CCS_TYPE(gr_ccs) & WC_CCS_A_CS96)) {
		    wtf_push_unknown(os, q, 2);
		    break;
		}
	    case GR:
		if (gr_ccs == WC_CCS_CNS_11643_X) {
		    state = WC_EUC_TW_MBYTE2;
		    continue;
		}
		wtf_push_iso2022(os, gr_ccs, ((wc_uint32)*q << 8) | *p);
		break;
	    default:
		wtf_push_unknown(os, q, 2);
		break;
	    }
	    break;
	case WC_EUC_TW_MBYTE2:
	    if (WC_ISO_MAP[*p] == GR) {
		if (0xa1 <= *q && *q <= 0xa7) {
		    wtf_push_iso2022(os, WC_CCS_CNS_11643_1 + (*q - 0xa1),
			((wc_uint32)*(q+1) << 8) | *p);
		    break;
		}
		if (0xa8 <= *q && *q <= 0xb0) {
		    wtf_push_iso2022(os, WC_CCS_CNS_11643_8 + (*q - 0xa8),
			((wc_uint32)*(q+1) << 8) | *p);
		    break;
		}
	    }
	    wtf_push_unknown(os, q, 3);
	    break;
	case WC_ISO_CSWSR:
	    if (*p == WC_C_ESC && *(p+1) == WC_C_CSWSR) {
		if (*(p+2) == WC_F_ISO_BASE) {
		    state = st.state = WC_ISO_NOSTATE;
		    p += 2;
		    continue;
		} else if (*(p+2) > WC_F_ISO_BASE && *(p+2) <= 0x7e) {
		    p += 2;
		    continue;
		}
	    }
	    wtf_push_unknown(os, p, 1);
	    continue;
	case WC_ISO_CSWOSR:
	    wtf_push_unknown(os, p, ep - p);
	    return os;
	    break;
	}
	st.ss = 0;
	state = WC_ISO_NOSTATE;
    }
    switch (state) {
    case WC_ISO_MBYTE1:
    case WC_EUC_MBYTE1:
	wtf_push_unknown(os, p-1, 1);
	break;
    case WC_EUC_TW_MBYTE1:
	wtf_push_unknown(os, p-2, 2);
	break;
    }
    return os;
}