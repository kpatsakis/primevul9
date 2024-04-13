S_compile_runtime_code(pTHX_ RExC_state_t * const pRExC_state,
    char *pat, STRLEN plen)
{
    SV *qr;

    GET_RE_DEBUG_FLAGS_DECL;

    if (pRExC_state->runtime_code_qr) {
	/* this is the second time we've been called; this should
	 * only happen if the main pattern got upgraded to utf8
	 * during compilation; re-use the qr we compiled first time
	 * round (which should be utf8 too)
	 */
	qr = pRExC_state->runtime_code_qr;
	pRExC_state->runtime_code_qr = NULL;
	assert(RExC_utf8 && SvUTF8(qr));
    }
    else {
	int n = 0;
	STRLEN s;
	char *p, *newpat;
	int newlen = plen + 7; /* allow for "qr''xx\0" extra chars */
	SV *sv, *qr_ref;
	dSP;

	/* determine how many extra chars we need for ' and \ escaping */
	for (s = 0; s < plen; s++) {
	    if (pat[s] == '\'' || pat[s] == '\\')
		newlen++;
	}

	Newx(newpat, newlen, char);
	p = newpat;
	*p++ = 'q'; *p++ = 'r'; *p++ = '\'';

	for (s = 0; s < plen; s++) {
	    if (   pRExC_state->code_blocks
	        && n < pRExC_state->code_blocks->count
		&& s == pRExC_state->code_blocks->cb[n].start)
	    {
		/* blank out literal code block */
		assert(pat[s] == '(');
		while (s <= pRExC_state->code_blocks->cb[n].end) {
		    *p++ = '_';
		    s++;
		}
		s--;
		n++;
		continue;
	    }
	    if (pat[s] == '\'' || pat[s] == '\\')
		*p++ = '\\';
	    *p++ = pat[s];
	}
	*p++ = '\'';
	if (pRExC_state->pm_flags & RXf_PMf_EXTENDED) {
	    *p++ = 'x';
            if (pRExC_state->pm_flags & RXf_PMf_EXTENDED_MORE) {
                *p++ = 'x';
            }
        }
	*p++ = '\0';
	DEBUG_COMPILE_r({
            Perl_re_printf( aTHX_
		"%sre-parsing pattern for runtime code:%s %s\n",
		PL_colors[4],PL_colors[5],newpat);
	});

	sv = newSVpvn_flags(newpat, p-newpat-1, RExC_utf8 ? SVf_UTF8 : 0);
	Safefree(newpat);

	ENTER;
	SAVETMPS;
	save_re_context();
	PUSHSTACKi(PERLSI_REQUIRE);
        /* G_RE_REPARSING causes the toker to collapse \\ into \ when
         * parsing qr''; normally only q'' does this. It also alters
         * hints handling */
	eval_sv(sv, G_SCALAR|G_RE_REPARSING);
	SvREFCNT_dec_NN(sv);
	SPAGAIN;
	qr_ref = POPs;
	PUTBACK;
	{
	    SV * const errsv = ERRSV;
	    if (SvTRUE_NN(errsv))
                /* use croak_sv ? */
		Perl_croak_nocontext("%" SVf, SVfARG(errsv));
	}
	assert(SvROK(qr_ref));
	qr = SvRV(qr_ref);
	assert(SvTYPE(qr) == SVt_REGEXP && RX_ENGINE((REGEXP*)qr)->op_comp);
	/* the leaving below frees the tmp qr_ref.
	 * Give qr a life of its own */
	SvREFCNT_inc(qr);
	POPSTACK;
	FREETMPS;
	LEAVE;

    }

    if (!RExC_utf8 && SvUTF8(qr)) {
	/* first time through; the pattern got upgraded; save the
	 * qr for the next time through */
	assert(!pRExC_state->runtime_code_qr);
	pRExC_state->runtime_code_qr = qr;
	return 0;
    }


    /* extract any code blocks within the returned qr//  */


    /* merge the main (r1) and run-time (r2) code blocks into one */
    {
	RXi_GET_DECL(ReANY((REGEXP *)qr), r2);
	struct reg_code_block *new_block, *dst;
	RExC_state_t * const r1 = pRExC_state; /* convenient alias */
	int i1 = 0, i2 = 0;
        int r1c, r2c;

	if (!r2->code_blocks || !r2->code_blocks->count) /* we guessed wrong */
	{
	    SvREFCNT_dec_NN(qr);
	    return 1;
	}

        if (!r1->code_blocks)
            r1->code_blocks = S_alloc_code_blocks(aTHX_ 0);

        r1c = r1->code_blocks->count;
        r2c = r2->code_blocks->count;

	Newx(new_block, r1c + r2c, struct reg_code_block);

	dst = new_block;

	while (i1 < r1c || i2 < r2c) {
	    struct reg_code_block *src;
	    bool is_qr = 0;

	    if (i1 == r1c) {
		src = &r2->code_blocks->cb[i2++];
		is_qr = 1;
	    }
	    else if (i2 == r2c)
		src = &r1->code_blocks->cb[i1++];
	    else if (  r1->code_blocks->cb[i1].start
	             < r2->code_blocks->cb[i2].start)
	    {
		src = &r1->code_blocks->cb[i1++];
		assert(src->end < r2->code_blocks->cb[i2].start);
	    }
	    else {
		assert(  r1->code_blocks->cb[i1].start
		       > r2->code_blocks->cb[i2].start);
		src = &r2->code_blocks->cb[i2++];
		is_qr = 1;
		assert(src->end < r1->code_blocks->cb[i1].start);
	    }

	    assert(pat[src->start] == '(');
	    assert(pat[src->end]   == ')');
	    dst->start	    = src->start;
	    dst->end	    = src->end;
	    dst->block	    = src->block;
	    dst->src_regex  = is_qr ? (REGEXP*) SvREFCNT_inc( (SV*) qr)
				    : src->src_regex;
	    dst++;
	}
	r1->code_blocks->count += r2c;
	Safefree(r1->code_blocks->cb);
	r1->code_blocks->cb = new_block;
    }

    SvREFCNT_dec_NN(qr);
    return 1;
}