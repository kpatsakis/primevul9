get_varp(struct vimoption *p)
{
    /* hidden option, always return NULL */
    if (p->var == NULL)
	return NULL;

    switch ((int)p->indir)
    {
	case PV_NONE:	return p->var;

	/* global option with local value: use local value if it's been set */
	case PV_EP:	return *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	case PV_KP:	return *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	case PV_PATH:	return *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	case PV_AR:	return curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	case PV_TAGS:	return *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	case PV_TC:	return *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	case PV_BKC:	return *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
#ifdef FEAT_FIND_ID
	case PV_DEF:	return *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	case PV_INC:	return *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
#ifdef FEAT_INS_EXPAND
	case PV_DICT:	return *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
	case PV_TSR:	return *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	case PV_EFM:	return *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	case PV_GP:	return *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	case PV_MP:	return *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#if defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	case PV_BEXPR:	return *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#if defined(FEAT_CRYPT)
	case PV_CM:	return *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_STL_OPT
	case PV_STL:	return *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
#endif
	case PV_UL:	return curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
#ifdef FEAT_LISP
	case PV_LW:	return *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
#endif

#ifdef FEAT_ARABIC
	case PV_ARAB:	return (char_u *)&(curwin->w_p_arab);
#endif
	case PV_LIST:	return (char_u *)&(curwin->w_p_list);
#ifdef FEAT_SPELL
	case PV_SPELL:	return (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	case PV_CUC:	return (char_u *)&(curwin->w_p_cuc);
	case PV_CUL:	return (char_u *)&(curwin->w_p_cul);
	case PV_CC:	return (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	case PV_DIFF:	return (char_u *)&(curwin->w_p_diff);
#endif
#ifdef FEAT_FOLDING
	case PV_FDC:	return (char_u *)&(curwin->w_p_fdc);
	case PV_FEN:	return (char_u *)&(curwin->w_p_fen);
	case PV_FDI:	return (char_u *)&(curwin->w_p_fdi);
	case PV_FDL:	return (char_u *)&(curwin->w_p_fdl);
	case PV_FDM:	return (char_u *)&(curwin->w_p_fdm);
	case PV_FML:	return (char_u *)&(curwin->w_p_fml);
	case PV_FDN:	return (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	case PV_FDE:	return (char_u *)&(curwin->w_p_fde);
	case PV_FDT:	return (char_u *)&(curwin->w_p_fdt);
# endif
	case PV_FMR:	return (char_u *)&(curwin->w_p_fmr);
#endif
	case PV_NU:	return (char_u *)&(curwin->w_p_nu);
	case PV_RNU:	return (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	case PV_NUW:	return (char_u *)&(curwin->w_p_nuw);
#endif
#ifdef FEAT_WINDOWS
	case PV_WFH:	return (char_u *)&(curwin->w_p_wfh);
	case PV_WFW:	return (char_u *)&(curwin->w_p_wfw);
#endif
#if defined(FEAT_WINDOWS) && defined(FEAT_QUICKFIX)
	case PV_PVW:	return (char_u *)&(curwin->w_p_pvw);
#endif
#ifdef FEAT_RIGHTLEFT
	case PV_RL:	return (char_u *)&(curwin->w_p_rl);
	case PV_RLC:	return (char_u *)&(curwin->w_p_rlc);
#endif
	case PV_SCROLL:	return (char_u *)&(curwin->w_p_scr);
	case PV_WRAP:	return (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	case PV_LBR:	return (char_u *)&(curwin->w_p_lbr);
	case PV_BRI:	return (char_u *)&(curwin->w_p_bri);
	case PV_BRIOPT: return (char_u *)&(curwin->w_p_briopt);
#endif
#ifdef FEAT_SCROLLBIND
	case PV_SCBIND: return (char_u *)&(curwin->w_p_scb);
#endif
#ifdef FEAT_CURSORBIND
	case PV_CRBIND: return (char_u *)&(curwin->w_p_crb);
#endif
#ifdef FEAT_CONCEAL
	case PV_COCU:    return (char_u *)&(curwin->w_p_cocu);
	case PV_COLE:    return (char_u *)&(curwin->w_p_cole);
#endif

	case PV_AI:	return (char_u *)&(curbuf->b_p_ai);
	case PV_BIN:	return (char_u *)&(curbuf->b_p_bin);
#ifdef FEAT_MBYTE
	case PV_BOMB:	return (char_u *)&(curbuf->b_p_bomb);
#endif
#if defined(FEAT_QUICKFIX)
	case PV_BH:	return (char_u *)&(curbuf->b_p_bh);
	case PV_BT:	return (char_u *)&(curbuf->b_p_bt);
#endif
	case PV_BL:	return (char_u *)&(curbuf->b_p_bl);
	case PV_CI:	return (char_u *)&(curbuf->b_p_ci);
#ifdef FEAT_CINDENT
	case PV_CIN:	return (char_u *)&(curbuf->b_p_cin);
	case PV_CINK:	return (char_u *)&(curbuf->b_p_cink);
	case PV_CINO:	return (char_u *)&(curbuf->b_p_cino);
#endif
#if defined(FEAT_SMARTINDENT) || defined(FEAT_CINDENT)
	case PV_CINW:	return (char_u *)&(curbuf->b_p_cinw);
#endif
#ifdef FEAT_COMMENTS
	case PV_COM:	return (char_u *)&(curbuf->b_p_com);
#endif
#ifdef FEAT_FOLDING
	case PV_CMS:	return (char_u *)&(curbuf->b_p_cms);
#endif
#ifdef FEAT_INS_EXPAND
	case PV_CPT:	return (char_u *)&(curbuf->b_p_cpt);
#endif
#ifdef FEAT_COMPL_FUNC
	case PV_CFU:	return (char_u *)&(curbuf->b_p_cfu);
	case PV_OFU:	return (char_u *)&(curbuf->b_p_ofu);
#endif
	case PV_EOL:	return (char_u *)&(curbuf->b_p_eol);
	case PV_FIXEOL:	return (char_u *)&(curbuf->b_p_fixeol);
	case PV_ET:	return (char_u *)&(curbuf->b_p_et);
#ifdef FEAT_MBYTE
	case PV_FENC:	return (char_u *)&(curbuf->b_p_fenc);
#endif
	case PV_FF:	return (char_u *)&(curbuf->b_p_ff);
#ifdef FEAT_AUTOCMD
	case PV_FT:	return (char_u *)&(curbuf->b_p_ft);
#endif
	case PV_FO:	return (char_u *)&(curbuf->b_p_fo);
	case PV_FLP:	return (char_u *)&(curbuf->b_p_flp);
	case PV_IMI:	return (char_u *)&(curbuf->b_p_iminsert);
	case PV_IMS:	return (char_u *)&(curbuf->b_p_imsearch);
	case PV_INF:	return (char_u *)&(curbuf->b_p_inf);
	case PV_ISK:	return (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	case PV_INEX:	return (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#if defined(FEAT_CINDENT) && defined(FEAT_EVAL)
	case PV_INDE:	return (char_u *)&(curbuf->b_p_inde);
	case PV_INDK:	return (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	case PV_FEX:	return (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	case PV_KEY:	return (char_u *)&(curbuf->b_p_key);
#endif
#ifdef FEAT_LISP
	case PV_LISP:	return (char_u *)&(curbuf->b_p_lisp);
#endif
	case PV_ML:	return (char_u *)&(curbuf->b_p_ml);
	case PV_MPS:	return (char_u *)&(curbuf->b_p_mps);
	case PV_MA:	return (char_u *)&(curbuf->b_p_ma);
	case PV_MOD:	return (char_u *)&(curbuf->b_changed);
	case PV_NF:	return (char_u *)&(curbuf->b_p_nf);
	case PV_PI:	return (char_u *)&(curbuf->b_p_pi);
#ifdef FEAT_TEXTOBJ
	case PV_QE:	return (char_u *)&(curbuf->b_p_qe);
#endif
	case PV_RO:	return (char_u *)&(curbuf->b_p_ro);
#ifdef FEAT_SMARTINDENT
	case PV_SI:	return (char_u *)&(curbuf->b_p_si);
#endif
	case PV_SN:	return (char_u *)&(curbuf->b_p_sn);
	case PV_STS:	return (char_u *)&(curbuf->b_p_sts);
#ifdef FEAT_SEARCHPATH
	case PV_SUA:	return (char_u *)&(curbuf->b_p_sua);
#endif
	case PV_SWF:	return (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	case PV_SMC:	return (char_u *)&(curbuf->b_p_smc);
	case PV_SYN:	return (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	case PV_SPC:	return (char_u *)&(curwin->w_s->b_p_spc);
	case PV_SPF:	return (char_u *)&(curwin->w_s->b_p_spf);
	case PV_SPL:	return (char_u *)&(curwin->w_s->b_p_spl);
#endif
	case PV_SW:	return (char_u *)&(curbuf->b_p_sw);
	case PV_TS:	return (char_u *)&(curbuf->b_p_ts);
	case PV_TW:	return (char_u *)&(curbuf->b_p_tw);
	case PV_TX:	return (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	case PV_UDF:	return (char_u *)&(curbuf->b_p_udf);
#endif
	case PV_WM:	return (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	case PV_KMAP:	return (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	case PV_SCL:	return (char_u *)&(curwin->w_p_scl);
#endif
	default:	EMSG(_("E356: get_varp ERROR"));
    }
    /* always return a valid pointer to avoid a crash! */
    return (char_u *)&(curbuf->b_p_wm);
}