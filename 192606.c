Perl_reg_named_buff_scalar(pTHX_ REGEXP * const r, const U32 flags)
{
    SV *ret;
    AV *av;
    SSize_t length;
    struct regexp *const rx = ReANY(r);

    PERL_ARGS_ASSERT_REG_NAMED_BUFF_SCALAR;

    if (rx && RXp_PAREN_NAMES(rx)) {
        if (flags & (RXapif_ALL | RXapif_REGNAMES_COUNT)) {
            return newSViv(HvTOTALKEYS(RXp_PAREN_NAMES(rx)));
        } else if (flags & RXapif_ONE) {
            ret = CALLREG_NAMED_BUFF_ALL(r, (flags | RXapif_REGNAMES));
            av = MUTABLE_AV(SvRV(ret));
            length = av_tindex(av);
	    SvREFCNT_dec_NN(ret);
            return newSViv(length + 1);
        } else {
            Perl_croak(aTHX_ "panic: Unknown flags %d in named_buff_scalar",
                                                (int)flags);
            return NULL;
        }
    }
    return &PL_sv_undef;
}