S_dump_trie(pTHX_ const struct _reg_trie_data *trie, HV *widecharmap,
	    AV *revcharmap, U32 depth)
{
    U32 state;
    SV *sv=sv_newmortal();
    int colwidth= widecharmap ? 6 : 4;
    U16 word;
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_DUMP_TRIE;

    Perl_re_indentf( aTHX_  "Char : %-6s%-6s%-4s ",
        depth+1, "Match","Base","Ofs" );

    for( state = 0 ; state < trie->uniquecharcount ; state++ ) {
	SV ** const tmp = av_fetch( revcharmap, state, 0);
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
    Perl_re_printf( aTHX_  "\n");
    Perl_re_indentf( aTHX_ "State|-----------------------", depth+1);

    for( state = 0 ; state < trie->uniquecharcount ; state++ )
        Perl_re_printf( aTHX_  "%.*s", colwidth, "--------");
    Perl_re_printf( aTHX_  "\n");

    for( state = 1 ; state < trie->statecount ; state++ ) {
	const U32 base = trie->states[ state ].trans.base;

        Perl_re_indentf( aTHX_  "#%4" UVXf "|", depth+1, (UV)state);

        if ( trie->states[ state ].wordnum ) {
            Perl_re_printf( aTHX_  " W%4X", trie->states[ state ].wordnum );
        } else {
            Perl_re_printf( aTHX_  "%6s", "" );
        }

        Perl_re_printf( aTHX_  " @%4" UVXf " ", (UV)base );

        if ( base ) {
            U32 ofs = 0;

            while( ( base + ofs  < trie->uniquecharcount ) ||
                   ( base + ofs - trie->uniquecharcount < trie->lasttrans
                     && trie->trans[ base + ofs - trie->uniquecharcount ].check
                                                                    != state))
                    ofs++;

            Perl_re_printf( aTHX_  "+%2" UVXf "[ ", (UV)ofs);

            for ( ofs = 0 ; ofs < trie->uniquecharcount ; ofs++ ) {
                if ( ( base + ofs >= trie->uniquecharcount )
                        && ( base + ofs - trie->uniquecharcount
                                                        < trie->lasttrans )
                        && trie->trans[ base + ofs
                                    - trie->uniquecharcount ].check == state )
                {
                   Perl_re_printf( aTHX_  "%*" UVXf, colwidth,
                    (UV)trie->trans[ base + ofs - trie->uniquecharcount ].next
                   );
                } else {
                    Perl_re_printf( aTHX_  "%*s",colwidth,"   ." );
                }
            }

            Perl_re_printf( aTHX_  "]");

        }
        Perl_re_printf( aTHX_  "\n" );
    }
    Perl_re_indentf( aTHX_  "word_info N:(prev,len)=",
                                depth);
    for (word=1; word <= trie->wordcount; word++) {
        Perl_re_printf( aTHX_  " %d:(%d,%d)",
	    (int)word, (int)(trie->wordinfo[word].prev),
	    (int)(trie->wordinfo[word].len));
    }
    Perl_re_printf( aTHX_  "\n" );
}