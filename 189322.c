copy_winopt(winopt_T *from, winopt_T *to)
{
#ifdef FEAT_ARABIC
    to->wo_arab = from->wo_arab;
#endif
    to->wo_list = from->wo_list;
    to->wo_nu = from->wo_nu;
    to->wo_rnu = from->wo_rnu;
#ifdef FEAT_LINEBREAK
    to->wo_nuw = from->wo_nuw;
#endif
#ifdef FEAT_RIGHTLEFT
    to->wo_rl  = from->wo_rl;
    to->wo_rlc = vim_strsave(from->wo_rlc);
#endif
#ifdef FEAT_STL_OPT
    to->wo_stl = vim_strsave(from->wo_stl);
#endif
    to->wo_wrap = from->wo_wrap;
#ifdef FEAT_DIFF
    to->wo_wrap_save = from->wo_wrap_save;
#endif
#ifdef FEAT_LINEBREAK
    to->wo_lbr = from->wo_lbr;
    to->wo_bri = from->wo_bri;
    to->wo_briopt = vim_strsave(from->wo_briopt);
#endif
#ifdef FEAT_SCROLLBIND
    to->wo_scb = from->wo_scb;
    to->wo_scb_save = from->wo_scb_save;
#endif
#ifdef FEAT_CURSORBIND
    to->wo_crb = from->wo_crb;
    to->wo_crb_save = from->wo_crb_save;
#endif
#ifdef FEAT_SPELL
    to->wo_spell = from->wo_spell;
#endif
#ifdef FEAT_SYN_HL
    to->wo_cuc = from->wo_cuc;
    to->wo_cul = from->wo_cul;
    to->wo_cc = vim_strsave(from->wo_cc);
#endif
#ifdef FEAT_DIFF
    to->wo_diff = from->wo_diff;
    to->wo_diff_saved = from->wo_diff_saved;
#endif
#ifdef FEAT_CONCEAL
    to->wo_cocu = vim_strsave(from->wo_cocu);
    to->wo_cole = from->wo_cole;
#endif
#ifdef FEAT_FOLDING
    to->wo_fdc = from->wo_fdc;
    to->wo_fdc_save = from->wo_fdc_save;
    to->wo_fen = from->wo_fen;
    to->wo_fen_save = from->wo_fen_save;
    to->wo_fdi = vim_strsave(from->wo_fdi);
    to->wo_fml = from->wo_fml;
    to->wo_fdl = from->wo_fdl;
    to->wo_fdl_save = from->wo_fdl_save;
    to->wo_fdm = vim_strsave(from->wo_fdm);
    to->wo_fdm_save = from->wo_diff_saved
			      ? vim_strsave(from->wo_fdm_save) : empty_option;
    to->wo_fdn = from->wo_fdn;
# ifdef FEAT_EVAL
    to->wo_fde = vim_strsave(from->wo_fde);
    to->wo_fdt = vim_strsave(from->wo_fdt);
# endif
    to->wo_fmr = vim_strsave(from->wo_fmr);
#endif
#ifdef FEAT_SIGNS
    to->wo_scl = vim_strsave(from->wo_scl);
#endif
    check_winopt(to);		/* don't want NULL pointers */
}