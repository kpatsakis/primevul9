Perl_reg_numbered_buff_fetch(pTHX_ REGEXP * const r, const I32 paren,
			     SV * const sv)
{
    struct regexp *const rx = ReANY(r);
    char *s = NULL;
    SSize_t i = 0;
    SSize_t s1, t1;
    I32 n = paren;

    PERL_ARGS_ASSERT_REG_NUMBERED_BUFF_FETCH;

    if (      n == RX_BUFF_IDX_CARET_PREMATCH
           || n == RX_BUFF_IDX_CARET_FULLMATCH
           || n == RX_BUFF_IDX_CARET_POSTMATCH
       )
    {
        bool keepcopy = cBOOL(rx->extflags & RXf_PMf_KEEPCOPY);
        if (!keepcopy) {
            /* on something like
             *    $r = qr/.../;
             *    /$qr/p;
             * the KEEPCOPY is set on the PMOP rather than the regex */
            if (PL_curpm && r == PM_GETRE(PL_curpm))
                 keepcopy = cBOOL(PL_curpm->op_pmflags & PMf_KEEPCOPY);
        }
        if (!keepcopy)
            goto ret_undef;
    }

    if (!rx->subbeg)
        goto ret_undef;

    if (n == RX_BUFF_IDX_CARET_FULLMATCH)
        /* no need to distinguish between them any more */
        n = RX_BUFF_IDX_FULLMATCH;

    if ((n == RX_BUFF_IDX_PREMATCH || n == RX_BUFF_IDX_CARET_PREMATCH)
        && rx->offs[0].start != -1)
    {
        /* $`, ${^PREMATCH} */
	i = rx->offs[0].start;
	s = rx->subbeg;
    }
    else
    if ((n == RX_BUFF_IDX_POSTMATCH || n == RX_BUFF_IDX_CARET_POSTMATCH)
        && rx->offs[0].end != -1)
    {
        /* $', ${^POSTMATCH} */
	s = rx->subbeg - rx->suboffset + rx->offs[0].end;
	i = rx->sublen + rx->suboffset - rx->offs[0].end;
    }
    else
    if ( 0 <= n && n <= (I32)rx->nparens &&
        (s1 = rx->offs[n].start) != -1 &&
        (t1 = rx->offs[n].end) != -1)
    {
        /* $&, ${^MATCH},  $1 ... */
        i = t1 - s1;
        s = rx->subbeg + s1 - rx->suboffset;
    } else {
        goto ret_undef;
    }

    assert(s >= rx->subbeg);
    assert((STRLEN)rx->sublen >= (STRLEN)((s - rx->subbeg) + i) );
    if (i >= 0) {
#ifdef NO_TAINT_SUPPORT
        sv_setpvn(sv, s, i);
#else
        const int oldtainted = TAINT_get;
        TAINT_NOT;
        sv_setpvn(sv, s, i);
        TAINT_set(oldtainted);
#endif
        if (RXp_MATCH_UTF8(rx))
            SvUTF8_on(sv);
        else
            SvUTF8_off(sv);
        if (TAINTING_get) {
            if (RXp_MATCH_TAINTED(rx)) {
                if (SvTYPE(sv) >= SVt_PVMG) {
                    MAGIC* const mg = SvMAGIC(sv);
                    MAGIC* mgt;
                    TAINT;
                    SvMAGIC_set(sv, mg->mg_moremagic);
                    SvTAINT(sv);
                    if ((mgt = SvMAGIC(sv))) {
                        mg->mg_moremagic = mgt;
                        SvMAGIC_set(sv, mg);
                    }
                } else {
                    TAINT;
                    SvTAINT(sv);
                }
            } else
                SvTAINTED_off(sv);
        }
    } else {
      ret_undef:
        sv_set_undef(sv);
        return;
    }
}