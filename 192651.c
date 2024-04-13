Perl_reg_numbered_buff_store(pTHX_ REGEXP * const rx, const I32 paren,
							 SV const * const value)
{
    PERL_ARGS_ASSERT_REG_NUMBERED_BUFF_STORE;

    PERL_UNUSED_ARG(rx);
    PERL_UNUSED_ARG(paren);
    PERL_UNUSED_ARG(value);

    if (!PL_localizing)
        Perl_croak_no_modify();
}