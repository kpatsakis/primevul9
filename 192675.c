S_construct_ahocorasick_from_trie(pTHX_ RExC_state_t *pRExC_state, regnode *source, U32 depth)
{
/* The Trie is constructed and compressed now so we can build a fail array if
 * it's needed

   This is basically the Aho-Corasick algorithm. Its from exercise 3.31 and
   3.32 in the
   "Red Dragon" -- Compilers, principles, techniques, and tools. Aho, Sethi,
   Ullman 1985/88
   ISBN 0-201-10088-6

   We find the fail state for each state in the trie, this state is the longest
   proper suffix of the current state's 'word' that is also a proper prefix of
   another word in our trie. State 1 represents the word '' and is thus the
   default fail state. This allows the DFA not to have to restart after its
   tried and failed a word at a given point, it simply continues as though it
   had been matching the other word in the first place.
   Consider
      'abcdgu'=~/abcdefg|cdgu/
   When we get to 'd' we are still matching the first word, we would encounter
   'g' which would fail, which would bring us to the state representing 'd' in
   the second word where we would try 'g' and succeed, proceeding to match
   'cdgu'.
 */
 /* add a fail transition */
    const U32 trie_offset = ARG(source);
    reg_trie_data *trie=(reg_trie_data *)RExC_rxi->data->data[trie_offset];
    U32 *q;
    const U32 ucharcount = trie->uniquecharcount;
    const U32 numstates = trie->statecount;
    const U32 ubound = trie->lasttrans + ucharcount;
    U32 q_read = 0;
    U32 q_write = 0;
    U32 charid;
    U32 base = trie->states[ 1 ].trans.base;
    U32 *fail;
    reg_ac_data *aho;
    const U32 data_slot = add_data( pRExC_state, STR_WITH_LEN("T"));
    regnode *stclass;
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_CONSTRUCT_AHOCORASICK_FROM_TRIE;
    PERL_UNUSED_CONTEXT;
#ifndef DEBUGGING
    PERL_UNUSED_ARG(depth);
#endif

    if ( OP(source) == TRIE ) {
        struct regnode_1 *op = (struct regnode_1 *)
            PerlMemShared_calloc(1, sizeof(struct regnode_1));
        StructCopy(source,op,struct regnode_1);
        stclass = (regnode *)op;
    } else {
        struct regnode_charclass *op = (struct regnode_charclass *)
            PerlMemShared_calloc(1, sizeof(struct regnode_charclass));
        StructCopy(source,op,struct regnode_charclass);
        stclass = (regnode *)op;
    }
    OP(stclass)+=2; /* convert the TRIE type to its AHO-CORASICK equivalent */

    ARG_SET( stclass, data_slot );
    aho = (reg_ac_data *) PerlMemShared_calloc( 1, sizeof(reg_ac_data) );
    RExC_rxi->data->data[ data_slot ] = (void*)aho;
    aho->trie=trie_offset;
    aho->states=(reg_trie_state *)PerlMemShared_malloc( numstates * sizeof(reg_trie_state) );
    Copy( trie->states, aho->states, numstates, reg_trie_state );
    Newxz( q, numstates, U32);
    aho->fail = (U32 *) PerlMemShared_calloc( numstates, sizeof(U32) );
    aho->refcount = 1;
    fail = aho->fail;
    /* initialize fail[0..1] to be 1 so that we always have
       a valid final fail state */
    fail[ 0 ] = fail[ 1 ] = 1;

    for ( charid = 0; charid < ucharcount ; charid++ ) {
	const U32 newstate = TRIE_TRANS_STATE( 1, base, ucharcount, charid, 0 );
	if ( newstate ) {
            q[ q_write ] = newstate;
            /* set to point at the root */
            fail[ q[ q_write++ ] ]=1;
        }
    }
    while ( q_read < q_write) {
	const U32 cur = q[ q_read++ % numstates ];
        base = trie->states[ cur ].trans.base;

        for ( charid = 0 ; charid < ucharcount ; charid++ ) {
	    const U32 ch_state = TRIE_TRANS_STATE( cur, base, ucharcount, charid, 1 );
	    if (ch_state) {
                U32 fail_state = cur;
                U32 fail_base;
                do {
                    fail_state = fail[ fail_state ];
                    fail_base = aho->states[ fail_state ].trans.base;
                } while ( !TRIE_TRANS_STATE( fail_state, fail_base, ucharcount, charid, 1 ) );

                fail_state = TRIE_TRANS_STATE( fail_state, fail_base, ucharcount, charid, 1 );
                fail[ ch_state ] = fail_state;
                if ( !aho->states[ ch_state ].wordnum && aho->states[ fail_state ].wordnum )
                {
                        aho->states[ ch_state ].wordnum =  aho->states[ fail_state ].wordnum;
                }
                q[ q_write++ % numstates] = ch_state;
            }
        }
    }
    /* restore fail[0..1] to 0 so that we "fall out" of the AC loop
       when we fail in state 1, this allows us to use the
       charclass scan to find a valid start char. This is based on the principle
       that theres a good chance the string being searched contains lots of stuff
       that cant be a start char.
     */
    fail[ 0 ] = fail[ 1 ] = 0;
    DEBUG_TRIE_COMPILE_r({
        Perl_re_indentf( aTHX_  "Stclass Failtable (%" UVuf " states): 0",
                      depth, (UV)numstates
        );
        for( q_read=1; q_read<numstates; q_read++ ) {
            Perl_re_printf( aTHX_  ", %" UVuf, (UV)fail[q_read]);
        }
        Perl_re_printf( aTHX_  "\n");
    });
    Safefree(q);
    /*RExC_seen |= REG_TRIEDFA_SEEN;*/
    return stclass;
}