S_ssc_clear_locale(regnode_ssc *ssc)
{
    /* Set the SSC 'ssc' to not match any locale things */
    PERL_ARGS_ASSERT_SSC_CLEAR_LOCALE;

    assert(is_ANYOF_SYNTHETIC(ssc));

    ANYOF_POSIXL_ZERO(ssc);
    ANYOF_FLAGS(ssc) &= ~ANYOF_LOCALE_FLAGS;
}