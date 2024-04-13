Perl_re_compile(pTHX_ SV * const pattern, U32 rx_flags)
{
    SV *pat = pattern; /* defeat constness! */
    PERL_ARGS_ASSERT_RE_COMPILE;
    return Perl_re_op_compile(aTHX_ &pat, 1, NULL,
#ifdef PERL_IN_XSUB_RE
                                &my_reg_engine,
#else
                                &PL_core_reg_engine,
#endif
                                NULL, NULL, rx_flags, 0);
}