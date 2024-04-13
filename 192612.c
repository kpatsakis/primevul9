Perl_reg_named_buff_iter(pTHX_ REGEXP * const rx, const SV * const lastkey,
                         const U32 flags)
{
    PERL_ARGS_ASSERT_REG_NAMED_BUFF_ITER;
    PERL_UNUSED_ARG(lastkey);

    if (flags & RXapif_FIRSTKEY)
        return reg_named_buff_firstkey(rx, flags);
    else if (flags & RXapif_NEXTKEY)
        return reg_named_buff_nextkey(rx, flags);
    else {
        Perl_croak(aTHX_ "panic: Unknown flags %d in named_buff_iter",
                                            (int)flags);
        return NULL;
    }
}