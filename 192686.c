Perl_reg_numbered_buff_length(pTHX_ REGEXP * const r, const SV * const sv,
                              const I32 paren)
{
    struct regexp *const rx = ReANY(r);
    I32 i;
    I32 s1, t1;

    PERL_ARGS_ASSERT_REG_NUMBERED_BUFF_LENGTH;

    if (   paren == RX_BUFF_IDX_CARET_PREMATCH
        || paren == RX_BUFF_IDX_CARET_FULLMATCH
        || paren == RX_BUFF_IDX_CARET_POSTMATCH
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
            goto warn_undef;
    }

    /* Some of this code was originally in C<Perl_magic_len> in F<mg.c> */
    switch (paren) {
      case RX_BUFF_IDX_CARET_PREMATCH: /* ${^PREMATCH} */
      case RX_BUFF_IDX_PREMATCH:       /* $` */
        if (rx->offs[0].start != -1) {
			i = rx->offs[0].start;
			if (i > 0) {
				s1 = 0;
				t1 = i;
				goto getlen;
			}
	    }
        return 0;

      case RX_BUFF_IDX_CARET_POSTMATCH: /* ${^POSTMATCH} */
      case RX_BUFF_IDX_POSTMATCH:       /* $' */
	    if (rx->offs[0].end != -1) {
			i = rx->sublen - rx->offs[0].end;
			if (i > 0) {
				s1 = rx->offs[0].end;
				t1 = rx->sublen;
				goto getlen;
			}
	    }
        return 0;

      default: /* $& / ${^MATCH}, $1, $2, ... */
	    if (paren <= (I32)rx->nparens &&
            (s1 = rx->offs[paren].start) != -1 &&
            (t1 = rx->offs[paren].end) != -1)
	    {
            i = t1 - s1;
            goto getlen;
        } else {
          warn_undef:
            if (ckWARN(WARN_UNINITIALIZED))
                report_uninit((const SV *)sv);
            return 0;
        }
    }
  getlen:
    if (i > 0 && RXp_MATCH_UTF8(rx)) {
        const char * const s = rx->subbeg - rx->suboffset + s1;
        const U8 *ep;
        STRLEN el;

        i = t1 - s1;
        if (is_utf8_string_loclen((U8*)s, i, &ep, &el))
			i = el;
    }
    return i;
}