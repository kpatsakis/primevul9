Perl_reg_named_buff_exists(pTHX_ REGEXP * const r, SV * const key,
                           const U32 flags)
{
    struct regexp *const rx = ReANY(r);

    PERL_ARGS_ASSERT_REG_NAMED_BUFF_EXISTS;

    if (rx && RXp_PAREN_NAMES(rx)) {
        if (flags & RXapif_ALL) {
            return hv_exists_ent(RXp_PAREN_NAMES(rx), key, 0);
        } else {
	    SV *sv = CALLREG_NAMED_BUFF_FETCH(r, key, flags);
            if (sv) {
		SvREFCNT_dec_NN(sv);
                return TRUE;
            } else {
                return FALSE;
            }
        }
    } else {
        return FALSE;
    }
}