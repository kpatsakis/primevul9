didset_options(void)
{
    /* initialize the table for 'iskeyword' et.al. */
    (void)init_chartab();

#ifdef FEAT_MBYTE
    (void)opt_strings_flags(p_cmp, p_cmp_values, &cmp_flags, TRUE);
#endif
    (void)opt_strings_flags(p_bkc, p_bkc_values, &bkc_flags, TRUE);
    (void)opt_strings_flags(p_bo, p_bo_values, &bo_flags, TRUE);
#ifdef FEAT_SESSION
    (void)opt_strings_flags(p_ssop, p_ssop_values, &ssop_flags, TRUE);
    (void)opt_strings_flags(p_vop, p_ssop_values, &vop_flags, TRUE);
#endif
#ifdef FEAT_FOLDING
    (void)opt_strings_flags(p_fdo, p_fdo_values, &fdo_flags, TRUE);
#endif
    (void)opt_strings_flags(p_dy, p_dy_values, &dy_flags, TRUE);
    (void)opt_strings_flags(p_tc, p_tc_values, &tc_flags, FALSE);
#ifdef FEAT_VIRTUALEDIT
    (void)opt_strings_flags(p_ve, p_ve_values, &ve_flags, TRUE);
#endif
#if defined(FEAT_MOUSE) && (defined(UNIX) || defined(VMS))
    (void)opt_strings_flags(p_ttym, p_ttym_values, &ttym_flags, FALSE);
#endif
#ifdef FEAT_SPELL
    (void)spell_check_msm();
    (void)spell_check_sps();
    (void)compile_cap_prog(curwin->w_s);
    (void)did_set_spell_option(TRUE);
#endif
#if defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_W32)
    (void)opt_strings_flags(p_toolbar, p_toolbar_values, &toolbar_flags, TRUE);
#endif
#if defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (void)opt_strings_flags(p_tbis, p_tbis_values, &tbis_flags, FALSE);
#endif
#ifdef FEAT_CMDWIN
    /* set cedit_key */
    (void)check_cedit();
#endif
#ifdef FEAT_LINEBREAK
    briopt_check(curwin);
#endif
#ifdef FEAT_LINEBREAK
    /* initialize the table for 'breakat'. */
    fill_breakat_flags();
#endif

}