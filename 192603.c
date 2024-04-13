Perl_reg_named_buff_all(pTHX_ REGEXP * const r, const U32 flags)
{
    struct regexp *const rx = ReANY(r);
    AV *av = newAV();

    PERL_ARGS_ASSERT_REG_NAMED_BUFF_ALL;

    if (rx && RXp_PAREN_NAMES(rx)) {
        HV *hv= RXp_PAREN_NAMES(rx);
        HE *temphe;
        (void)hv_iterinit(hv);
        while ( (temphe = hv_iternext_flags(hv,0)) ) {
            IV i;
            IV parno = 0;
            SV* sv_dat = HeVAL(temphe);
            I32 *nums = (I32*)SvPVX(sv_dat);
            for ( i = 0; i < SvIVX(sv_dat); i++ ) {
                if ((I32)(rx->lastparen) >= nums[i] &&
                    rx->offs[nums[i]].start != -1 &&
                    rx->offs[nums[i]].end != -1)
                {
                    parno = nums[i];
                    break;
                }
            }
            if (parno || flags & RXapif_ALL) {
                av_push(av, newSVhek(HeKEY_hek(temphe)));
            }
        }
    }

    return newRV_noinc(MUTABLE_SV(av));
}