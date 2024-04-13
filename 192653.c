S_reg_scan_name(pTHX_ RExC_state_t *pRExC_state, U32 flags)
{
    char *name_start = RExC_parse;

    PERL_ARGS_ASSERT_REG_SCAN_NAME;

    assert (RExC_parse <= RExC_end);
    if (RExC_parse == RExC_end) NOOP;
    else if (isIDFIRST_lazy_if_safe(RExC_parse, RExC_end, UTF)) {
         /* Note that the code here assumes well-formed UTF-8.  Skip IDFIRST by
          * using do...while */
	if (UTF)
	    do {
		RExC_parse += UTF8SKIP(RExC_parse);
	    } while (   RExC_parse < RExC_end
                     && isWORDCHAR_utf8_safe((U8*)RExC_parse, (U8*) RExC_end));
	else
	    do {
		RExC_parse++;
	    } while (RExC_parse < RExC_end && isWORDCHAR(*RExC_parse));
    } else {
        RExC_parse++; /* so the <- from the vFAIL is after the offending
                         character */
        vFAIL("Group name must start with a non-digit word character");
    }
    if ( flags ) {
        SV* sv_name
	    = newSVpvn_flags(name_start, (int)(RExC_parse - name_start),
			     SVs_TEMP | (UTF ? SVf_UTF8 : 0));
        if ( flags == REG_RSN_RETURN_NAME)
            return sv_name;
        else if (flags==REG_RSN_RETURN_DATA) {
            HE *he_str = NULL;
            SV *sv_dat = NULL;
            if ( ! sv_name )      /* should not happen*/
                Perl_croak(aTHX_ "panic: no svname in reg_scan_name");
            if (RExC_paren_names)
                he_str = hv_fetch_ent( RExC_paren_names, sv_name, 0, 0 );
            if ( he_str )
                sv_dat = HeVAL(he_str);
            if ( ! sv_dat )
                vFAIL("Reference to nonexistent named group");
            return sv_dat;
        }
        else {
            Perl_croak(aTHX_ "panic: bad flag %lx in reg_scan_name",
		       (unsigned long) flags);
        }
        NOT_REACHED; /* NOTREACHED */
    }
    return NULL;
}