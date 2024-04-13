didset_options2(void)
{
    /* Initialize the highlight_attr[] table. */
    (void)highlight_changed();

    /* Parse default for 'wildmode'  */
    check_opt_wim();

    (void)set_chars_option(&p_lcs);
#if defined(FEAT_WINDOWS) || defined(FEAT_FOLDING)
    /* Parse default for 'fillchars'. */
    (void)set_chars_option(&p_fcs);
#endif

#ifdef FEAT_CLIPBOARD
    /* Parse default for 'clipboard' */
    (void)check_clipboard_option();
#endif
}