check_buf_options(buf_T *buf)
{
#if defined(FEAT_QUICKFIX)
    check_string_option(&buf->b_p_bh);
    check_string_option(&buf->b_p_bt);
#endif
#ifdef FEAT_MBYTE
    check_string_option(&buf->b_p_fenc);
#endif
    check_string_option(&buf->b_p_ff);
#ifdef FEAT_FIND_ID
    check_string_option(&buf->b_p_def);
    check_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    check_string_option(&buf->b_p_inex);
# endif
#endif
#if defined(FEAT_CINDENT) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#endif
#if defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#endif
#if defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#endif
#if defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
    check_string_option(&buf->b_p_key);
#endif
    check_string_option(&buf->b_p_kp);
    check_string_option(&buf->b_p_mps);
    check_string_option(&buf->b_p_fo);
    check_string_option(&buf->b_p_flp);
    check_string_option(&buf->b_p_isk);
#ifdef FEAT_COMMENTS
    check_string_option(&buf->b_p_com);
#endif
#ifdef FEAT_FOLDING
    check_string_option(&buf->b_p_cms);
#endif
    check_string_option(&buf->b_p_nf);
#ifdef FEAT_TEXTOBJ
    check_string_option(&buf->b_p_qe);
#endif
#ifdef FEAT_SYN_HL
    check_string_option(&buf->b_p_syn);
    check_string_option(&buf->b_s.b_syn_isk);
#endif
#ifdef FEAT_SPELL
    check_string_option(&buf->b_s.b_p_spc);
    check_string_option(&buf->b_s.b_p_spf);
    check_string_option(&buf->b_s.b_p_spl);
#endif
#ifdef FEAT_SEARCHPATH
    check_string_option(&buf->b_p_sua);
#endif
#ifdef FEAT_CINDENT
    check_string_option(&buf->b_p_cink);
    check_string_option(&buf->b_p_cino);
    parse_cino(buf);
#endif
#ifdef FEAT_AUTOCMD
    check_string_option(&buf->b_p_ft);
#endif
#if defined(FEAT_SMARTINDENT) || defined(FEAT_CINDENT)
    check_string_option(&buf->b_p_cinw);
#endif
#ifdef FEAT_INS_EXPAND
    check_string_option(&buf->b_p_cpt);
#endif
#ifdef FEAT_COMPL_FUNC
    check_string_option(&buf->b_p_cfu);
    check_string_option(&buf->b_p_ofu);
#endif
#ifdef FEAT_KEYMAP
    check_string_option(&buf->b_p_keymap);
#endif
#ifdef FEAT_QUICKFIX
    check_string_option(&buf->b_p_gp);
    check_string_option(&buf->b_p_mp);
    check_string_option(&buf->b_p_efm);
#endif
    check_string_option(&buf->b_p_ep);
    check_string_option(&buf->b_p_path);
    check_string_option(&buf->b_p_tags);
    check_string_option(&buf->b_p_tc);
#ifdef FEAT_INS_EXPAND
    check_string_option(&buf->b_p_dict);
    check_string_option(&buf->b_p_tsr);
#endif
#ifdef FEAT_LISP
    check_string_option(&buf->b_p_lw);
#endif
    check_string_option(&buf->b_p_bkc);
}