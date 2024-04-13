S_dump_trie_interim_list(pTHX_ const struct _reg_trie_data *trie,
			 HV *widecharmap, AV *revcharmap, U32 next_alloc,
			 U32 depth)
{
    U32 state;
    SV *sv=sv_newmortal();
    int colwidth= widecharmap ? 6 : 4;
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_DUMP_TRIE_INTERIM_LIST;

    /* print out the table precompression.  */
    Perl_re_indentf( aTHX_  "State :Word | Transition Data\n",
            depth+1 );
    Perl_re_indentf( aTHX_  "%s",
            depth+1, "------:-----+-----------------\n" );

    for( state=1 ; state < next_alloc ; state ++ ) {
        U16 charid;

        Perl_re_indentf( aTHX_  " %4" UVXf " :",
            depth+1, (UV)state  );
        if ( ! trie->states[ state ].wordnum ) {
            Perl_re_printf( aTHX_  "%5s| ","");
        } else {
            Perl_re_printf( aTHX_  "W%4x| ",
                trie->states[ state ].wordnum
            );
        }
        for( charid = 1 ; charid <= TRIE_LIST_USED( state ) ; charid++ ) {
	    SV ** const tmp = av_fetch( revcharmap,
                                        TRIE_LIST_ITEM(state,charid).forid, 0);
	    if ( tmp ) {
                Perl_re_printf( aTHX_  "%*s:%3X=%4" UVXf " | ",
                    colwidth,
                    pv_pretty(sv, SvPV_nolen_const(*tmp), SvCUR(*tmp),
                              colwidth,
                              PL_colors[0], PL_colors[1],
                              (SvUTF8(*tmp) ? PERL_PV_ESCAPE_UNI : 0)
                              | PERL_PV_ESCAPE_FIRSTCHAR
                    ) ,
                    TRIE_LIST_ITEM(state,charid).forid,
                    (UV)TRIE_LIST_ITEM(state,charid).newstate
                );
                if (!(charid % 10))
                    Perl_re_printf( aTHX_  "\n%*s| ",
                        (int)((depth * 2) + 14), "");
            }
        }
        Perl_re_printf( aTHX_  "\n");
    }
}