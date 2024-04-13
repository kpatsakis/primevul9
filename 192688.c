S_concat_pat(pTHX_ RExC_state_t * const pRExC_state,
                SV *pat, SV ** const patternp, int pat_count,
                OP *oplist, bool *recompile_p, SV *delim)
{
    SV **svp;
    int n = 0;
    bool use_delim = FALSE;
    bool alloced = FALSE;

    /* if we know we have at least two args, create an empty string,
     * then concatenate args to that. For no args, return an empty string */
    if (!pat && pat_count != 1) {
        pat = newSVpvs("");
        SAVEFREESV(pat);
        alloced = TRUE;
    }

    for (svp = patternp; svp < patternp + pat_count; svp++) {
        SV *sv;
        SV *rx  = NULL;
        STRLEN orig_patlen = 0;
        bool code = 0;
        SV *msv = use_delim ? delim : *svp;
        if (!msv) msv = &PL_sv_undef;

        /* if we've got a delimiter, we go round the loop twice for each
         * svp slot (except the last), using the delimiter the second
         * time round */
        if (use_delim) {
            svp--;
            use_delim = FALSE;
        }
        else if (delim)
            use_delim = TRUE;

        if (SvTYPE(msv) == SVt_PVAV) {
            /* we've encountered an interpolated array within
             * the pattern, e.g. /...@a..../. Expand the list of elements,
             * then recursively append elements.
             * The code in this block is based on S_pushav() */

            AV *const av = (AV*)msv;
            const SSize_t maxarg = AvFILL(av) + 1;
            SV **array;

            if (oplist) {
                assert(oplist->op_type == OP_PADAV
                    || oplist->op_type == OP_RV2AV);
                oplist = OpSIBLING(oplist);
            }

            if (SvRMAGICAL(av)) {
                SSize_t i;

                Newx(array, maxarg, SV*);
                SAVEFREEPV(array);
                for (i=0; i < maxarg; i++) {
                    SV ** const svp = av_fetch(av, i, FALSE);
                    array[i] = svp ? *svp : &PL_sv_undef;
                }
            }
            else
                array = AvARRAY(av);

            pat = S_concat_pat(aTHX_ pRExC_state, pat,
                                array, maxarg, NULL, recompile_p,
                                /* $" */
                                GvSV((gv_fetchpvs("\"", GV_ADDMULTI, SVt_PV))));

            continue;
        }


        /* we make the assumption here that each op in the list of
         * op_siblings maps to one SV pushed onto the stack,
         * except for code blocks, with have both an OP_NULL and
         * and OP_CONST.
         * This allows us to match up the list of SVs against the
         * list of OPs to find the next code block.
         *
         * Note that       PUSHMARK PADSV PADSV ..
         * is optimised to
         *                 PADRANGE PADSV  PADSV  ..
         * so the alignment still works. */

        if (oplist) {
            if (oplist->op_type == OP_NULL
                && (oplist->op_flags & OPf_SPECIAL))
            {
                assert(n < pRExC_state->code_blocks->count);
                pRExC_state->code_blocks->cb[n].start = pat ? SvCUR(pat) : 0;
                pRExC_state->code_blocks->cb[n].block = oplist;
                pRExC_state->code_blocks->cb[n].src_regex = NULL;
                n++;
                code = 1;
                oplist = OpSIBLING(oplist); /* skip CONST */
                assert(oplist);
            }
            oplist = OpSIBLING(oplist);;
        }

	/* apply magic and QR overloading to arg */

        SvGETMAGIC(msv);
        if (SvROK(msv) && SvAMAGIC(msv)) {
            SV *sv = AMG_CALLunary(msv, regexp_amg);
            if (sv) {
                if (SvROK(sv))
                    sv = SvRV(sv);
                if (SvTYPE(sv) != SVt_REGEXP)
                    Perl_croak(aTHX_ "Overloaded qr did not return a REGEXP");
                msv = sv;
            }
        }

        /* try concatenation overload ... */
        if (pat && (SvAMAGIC(pat) || SvAMAGIC(msv)) &&
                (sv = amagic_call(pat, msv, concat_amg, AMGf_assign)))
        {
            sv_setsv(pat, sv);
            /* overloading involved: all bets are off over literal
             * code. Pretend we haven't seen it */
            if (n)
                pRExC_state->code_blocks->count -= n;
            n = 0;
        }
        else  {
            /* ... or failing that, try "" overload */
            while (SvAMAGIC(msv)
                    && (sv = AMG_CALLunary(msv, string_amg))
                    && sv != msv
                    &&  !(   SvROK(msv)
                          && SvROK(sv)
                          && SvRV(msv) == SvRV(sv))
            ) {
                msv = sv;
                SvGETMAGIC(msv);
            }
            if (SvROK(msv) && SvTYPE(SvRV(msv)) == SVt_REGEXP)
                msv = SvRV(msv);

            if (pat) {
                /* this is a partially unrolled
                 *     sv_catsv_nomg(pat, msv);
                 * that allows us to adjust code block indices if
                 * needed */
                STRLEN dlen;
                char *dst = SvPV_force_nomg(pat, dlen);
                orig_patlen = dlen;
                if (SvUTF8(msv) && !SvUTF8(pat)) {
                    S_pat_upgrade_to_utf8(aTHX_ pRExC_state, &dst, &dlen, n);
                    sv_setpvn(pat, dst, dlen);
                    SvUTF8_on(pat);
                }
                sv_catsv_nomg(pat, msv);
                rx = msv;
            }
            else {
                /* We have only one SV to process, but we need to verify
                 * it is properly null terminated or we will fail asserts
                 * later. In theory we probably shouldn't get such SV's,
                 * but if we do we should handle it gracefully. */
                if ( SvTYPE(msv) != SVt_PV || (SvLEN(msv) > SvCUR(msv) && *(SvEND(msv)) == 0) ) {
                    /* not a string, or a string with a trailing null */
                    pat = msv;
                } else {
                    /* a string with no trailing null, we need to copy it
                     * so it we have a trailing null */
                    pat = newSVsv(msv);
                }
            }

            if (code)
                pRExC_state->code_blocks->cb[n-1].end = SvCUR(pat)-1;
        }

        /* extract any code blocks within any embedded qr//'s */
        if (rx && SvTYPE(rx) == SVt_REGEXP
            && RX_ENGINE((REGEXP*)rx)->op_comp)
        {

            RXi_GET_DECL(ReANY((REGEXP *)rx), ri);
            if (ri->code_blocks && ri->code_blocks->count) {
                int i;
                /* the presence of an embedded qr// with code means
                 * we should always recompile: the text of the
                 * qr// may not have changed, but it may be a
                 * different closure than last time */
                *recompile_p = 1;
                if (pRExC_state->code_blocks) {
                    int new_count = pRExC_state->code_blocks->count
                            + ri->code_blocks->count;
                    Renew(pRExC_state->code_blocks->cb,
                            new_count, struct reg_code_block);
                    pRExC_state->code_blocks->count = new_count;
                }
                else
                    pRExC_state->code_blocks = S_alloc_code_blocks(aTHX_
                                                    ri->code_blocks->count);

                for (i=0; i < ri->code_blocks->count; i++) {
                    struct reg_code_block *src, *dst;
                    STRLEN offset =  orig_patlen
                        + ReANY((REGEXP *)rx)->pre_prefix;
                    assert(n < pRExC_state->code_blocks->count);
                    src = &ri->code_blocks->cb[i];
                    dst = &pRExC_state->code_blocks->cb[n];
                    dst->start	    = src->start + offset;
                    dst->end	    = src->end   + offset;
                    dst->block	    = src->block;
                    dst->src_regex  = (REGEXP*) SvREFCNT_inc( (SV*)
                                            src->src_regex
                                                ? src->src_regex
                                                : (REGEXP*)rx);
                    n++;
                }
            }
        }
    }
    /* avoid calling magic multiple times on a single element e.g. =~ $qr */
    if (alloced)
        SvSETMAGIC(pat);

    return pat;
}