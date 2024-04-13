unset_global_local_option(char_u *name, void *from)
{
    struct vimoption *p;
    int		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    if (opt_idx < 0)
	return;
    p = &(options[opt_idx]);

    switch ((int)p->indir)
    {
	/* global option with local value: use local value if it's been set */
	case PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    break;
	case PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    break;
	case PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    break;
	case PV_AR:
	    buf->b_p_ar = -1;
	    break;
	case PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    break;
	case PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    break;
	case PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    break;
#ifdef FEAT_FIND_ID
	case PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    break;
	case PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    break;
#endif
#ifdef FEAT_INS_EXPAND
	case PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    break;
	case PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    break;
#endif
#ifdef FEAT_QUICKFIX
	case PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    break;
	case PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    break;
	case PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    break;
#endif
#if defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	case PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    break;
#endif
#if defined(FEAT_CRYPT)
	case PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    break;
#endif
#ifdef FEAT_STL_OPT
	case PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    break;
#endif
	case PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    break;
#ifdef FEAT_LISP
	case PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    break;
#endif
    }
}