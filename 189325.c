clear_winopt(winopt_T *wop UNUSED)
{
#ifdef FEAT_FOLDING
    clear_string_option(&wop->wo_fdi);
    clear_string_option(&wop->wo_fdm);
    clear_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    clear_string_option(&wop->wo_fde);
    clear_string_option(&wop->wo_fdt);
# endif
    clear_string_option(&wop->wo_fmr);
#endif
#ifdef FEAT_SIGNS
    clear_string_option(&wop->wo_scl);
#endif
#ifdef FEAT_LINEBREAK
    clear_string_option(&wop->wo_briopt);
#endif
#ifdef FEAT_RIGHTLEFT
    clear_string_option(&wop->wo_rlc);
#endif
#ifdef FEAT_STL_OPT
    clear_string_option(&wop->wo_stl);
#endif
#ifdef FEAT_SYN_HL
    clear_string_option(&wop->wo_cc);
#endif
#ifdef FEAT_CONCEAL
    clear_string_option(&wop->wo_cocu);
#endif
}