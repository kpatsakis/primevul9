S_debug_peep(pTHX_ const char *str, const RExC_state_t *pRExC_state,
                regnode *scan, U32 depth, U32 flags)
{
    GET_RE_DEBUG_FLAGS_DECL;

    DEBUG_OPTIMISE_r({
        regnode *Next;

        if (!scan)
            return;
        Next = regnext(scan);
        regprop(RExC_rx, RExC_mysv, scan, NULL, pRExC_state);
        Perl_re_indentf( aTHX_   "%s>%3d: %s (%d)",
            depth,
            str,
            REG_NODE_NUM(scan), SvPV_nolen_const(RExC_mysv),
            Next ? (REG_NODE_NUM(Next)) : 0 );
        S_debug_show_study_flags(aTHX_ flags," [ ","]");
        Perl_re_printf( aTHX_  "\n");
   });
}