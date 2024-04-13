S_dump_trie_interim_table(pTHX_ const struct _reg_trie_data *trie,
			  HV *widecharmap, AV *revcharmap, U32 next_alloc,
			  U32 depth)
{
    U32 state;
    U16 charid;
    SV *sv=sv_newmortal();
    int colwidth= widecharmap ? 6 : 4;
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_DUMP_TRIE_INTERIM_TABLE;

    /*
       print out the table precompression so that we can do a visual check
       that they are identical.
     */

    Perl_re_indentf( aTHX_  "Char : ", depth+1 );

    for( charid = 0 ; charid < trie->uniquecharcount ; charid++ ) {
	SV ** const tmp = av_fetch( revcharmap, charid, 0);
        if ( tmp ) {
            Perl_re_printf( aTHX_  "%*s",
                colwidth,
                pv_pretty(sv, SvPV_nolen_const(*tmp), SvCUR(*tmp), colwidth,
	                    PL_colors[0], PL_colors[1],
	                    (SvUTF8(*tmp) ? PERL_PV_ESCAPE_UNI : 0) |
	                    PERL_PV_ESCAPE_FIRSTCHAR
                )
            );
        }
    }

    Perl_re_printf( aTHX_ "\n");
    Perl_re_indentf( aTHX_  "State+-", depth+1 );

    for( charid=0 ; charid < trie->uniquecharcount ; charid++ ) {
        Perl_re_printf( aTHX_  "%.*s", colwidth,"--------");
    }

    Perl_re_printf( aTHX_  "\n" );

    for( state=1 ; state < next_alloc ; state += trie->uniquecharcount ) {

        Perl_re_indentf( aTHX_  "%4" UVXf " : ",
            depth+1,
            (UV)TRIE_NODENUM( state ) );

        for( charid = 0 ; charid < trie->uniquecharcount ; charid++ ) {
            UV v=(UV)SAFE_TRIE_NODENUM( trie->trans[ state + charid ].next );
            if (v)
                Perl_re_printf( aTHX_  "%*" UVXf, colwidth, v );
            else
                Perl_re_printf( aTHX_  "%*s", colwidth, "." );
        }
        if ( ! trie->states[ TRIE_NODENUM( state ) ].wordnum ) {
            Perl_re_printf( aTHX_  " (%4" UVXf ")\n",
                                            (UV)trie->trans[ state ].check );
        } else {
            Perl_re_printf( aTHX_  " (%4" UVXf ") W%4X\n",
                                            (UV)trie->trans[ state ].check,
            trie->states[ TRIE_NODENUM( state ) ].wordnum );
        }
    }
}