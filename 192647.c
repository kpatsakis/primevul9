Perl_reg_named_buff_fetch(pTHX_ REGEXP * const r, SV * const namesv,
			  const U32 flags)
{
    SV *ret;
    struct regexp *const rx = ReANY(r);

    PERL_ARGS_ASSERT_REG_NAMED_BUFF_FETCH;

    if (rx && RXp_PAREN_NAMES(rx)) {
        HE *he_str = hv_fetch_ent( RXp_PAREN_NAMES(rx), namesv, 0, 0 );
        if (he_str) {
            IV i;
            SV* sv_dat=HeVAL(he_str);
            I32 *nums=(I32*)SvPVX(sv_dat);
            AV * const retarray = (flags & RXapif_ALL) ? newAV() : NULL;
            for ( i=0; i<SvIVX(sv_dat); i++ ) {
                if ((I32)(rx->nparens) >= nums[i]
                    && rx->offs[nums[i]].start != -1
                    && rx->offs[nums[i]].end != -1)
                {
                    ret = newSVpvs("");
                    CALLREG_NUMBUF_FETCH(r,nums[i],ret);
                    if (!retarray)
                        return ret;
                } else {
                    if (retarray)
                        ret = newSVsv(&PL_sv_undef);
                }
                if (retarray)
                    av_push(retarray, ret);
            }
            if (retarray)
                return newRV_noinc(MUTABLE_SV(retarray));
        }
    }
    return NULL;
}