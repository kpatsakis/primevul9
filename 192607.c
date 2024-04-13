S_make_trie(pTHX_ RExC_state_t *pRExC_state, regnode *startbranch,
                  regnode *first, regnode *last, regnode *tail,
                  U32 word_count, U32 flags, U32 depth)
{
    /* first pass, loop through and scan words */
    reg_trie_data *trie;
    HV *widecharmap = NULL;
    AV *revcharmap = newAV();
    regnode *cur;
    STRLEN len = 0;
    UV uvc = 0;
    U16 curword = 0;
    U32 next_alloc = 0;
    regnode *jumper = NULL;
    regnode *nextbranch = NULL;
    regnode *convert = NULL;
    U32 *prev_states; /* temp array mapping each state to previous one */
    /* we just use folder as a flag in utf8 */
    const U8 * folder = NULL;

    /* in the below add_data call we are storing either 'tu' or 'tuaa'
     * which stands for one trie structure, one hash, optionally followed
     * by two arrays */
#ifdef DEBUGGING
    const U32 data_slot = add_data( pRExC_state, STR_WITH_LEN("tuaa"));
    AV *trie_words = NULL;
    /* along with revcharmap, this only used during construction but both are
     * useful during debugging so we store them in the struct when debugging.
     */
#else
    const U32 data_slot = add_data( pRExC_state, STR_WITH_LEN("tu"));
    STRLEN trie_charcount=0;
#endif
    SV *re_trie_maxbuff;
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_MAKE_TRIE;
#ifndef DEBUGGING
    PERL_UNUSED_ARG(depth);
#endif

    switch (flags) {
        case EXACT: case EXACTL: break;
	case EXACTFA:
        case EXACTFU_SS:
	case EXACTFU:
	case EXACTFLU8: folder = PL_fold_latin1; break;
	case EXACTF:  folder = PL_fold; break;
        default: Perl_croak( aTHX_ "panic! In trie construction, unknown node type %u %s", (unsigned) flags, PL_reg_name[flags] );
    }

    trie = (reg_trie_data *) PerlMemShared_calloc( 1, sizeof(reg_trie_data) );
    trie->refcount = 1;
    trie->startstate = 1;
    trie->wordcount = word_count;
    RExC_rxi->data->data[ data_slot ] = (void*)trie;
    trie->charmap = (U16 *) PerlMemShared_calloc( 256, sizeof(U16) );
    if (flags == EXACT || flags == EXACTL)
	trie->bitmap = (char *) PerlMemShared_calloc( ANYOF_BITMAP_SIZE, 1 );
    trie->wordinfo = (reg_trie_wordinfo *) PerlMemShared_calloc(
                       trie->wordcount+1, sizeof(reg_trie_wordinfo));

    DEBUG_r({
        trie_words = newAV();
    });

    re_trie_maxbuff = get_sv(RE_TRIE_MAXBUF_NAME, 1);
    assert(re_trie_maxbuff);
    if (!SvIOK(re_trie_maxbuff)) {
        sv_setiv(re_trie_maxbuff, RE_TRIE_MAXBUF_INIT);
    }
    DEBUG_TRIE_COMPILE_r({
        Perl_re_indentf( aTHX_
          "make_trie start==%d, first==%d, last==%d, tail==%d depth=%d\n",
          depth+1,
          REG_NODE_NUM(startbranch),REG_NODE_NUM(first),
          REG_NODE_NUM(last), REG_NODE_NUM(tail), (int)depth);
    });

   /* Find the node we are going to overwrite */
    if ( first == startbranch && OP( last ) != BRANCH ) {
        /* whole branch chain */
        convert = first;
    } else {
        /* branch sub-chain */
        convert = NEXTOPER( first );
    }

    /*  -- First loop and Setup --

       We first traverse the branches and scan each word to determine if it
       contains widechars, and how many unique chars there are, this is
       important as we have to build a table with at least as many columns as we
       have unique chars.

       We use an array of integers to represent the character codes 0..255
       (trie->charmap) and we use a an HV* to store Unicode characters. We use
       the native representation of the character value as the key and IV's for
       the coded index.

       *TODO* If we keep track of how many times each character is used we can
       remap the columns so that the table compression later on is more
       efficient in terms of memory by ensuring the most common value is in the
       middle and the least common are on the outside.  IMO this would be better
       than a most to least common mapping as theres a decent chance the most
       common letter will share a node with the least common, meaning the node
       will not be compressible. With a middle is most common approach the worst
       case is when we have the least common nodes twice.

     */

    for ( cur = first ; cur < last ; cur = regnext( cur ) ) {
        regnode *noper = NEXTOPER( cur );
        const U8 *uc;
        const U8 *e;
        int foldlen = 0;
        U32 wordlen      = 0;         /* required init */
        STRLEN minchars = 0;
        STRLEN maxchars = 0;
        bool set_bit = trie->bitmap ? 1 : 0; /*store the first char in the
                                               bitmap?*/

        if (OP(noper) == NOTHING) {
            /* skip past a NOTHING at the start of an alternation
             * eg, /(?:)a|(?:b)/ should be the same as /a|b/
             */
            regnode *noper_next= regnext(noper);
            if (noper_next < tail)
                noper= noper_next;
        }

        if ( noper < tail &&
                (
                    OP(noper) == flags ||
                    (
                        flags == EXACTFU &&
                        OP(noper) == EXACTFU_SS
                    )
                )
        ) {
            uc= (U8*)STRING(noper);
            e= uc + STR_LEN(noper);
        } else {
            trie->minlen= 0;
            continue;
        }


        if ( set_bit ) { /* bitmap only alloced when !(UTF&&Folding) */
            TRIE_BITMAP_SET(trie,*uc); /* store the raw first byte
                                          regardless of encoding */
            if (OP( noper ) == EXACTFU_SS) {
                /* false positives are ok, so just set this */
                TRIE_BITMAP_SET(trie, LATIN_SMALL_LETTER_SHARP_S);
            }
        }

        for ( ; uc < e ; uc += len ) {  /* Look at each char in the current
                                           branch */
            TRIE_CHARCOUNT(trie)++;
            TRIE_READ_CHAR;

            /* TRIE_READ_CHAR returns the current character, or its fold if /i
             * is in effect.  Under /i, this character can match itself, or
             * anything that folds to it.  If not under /i, it can match just
             * itself.  Most folds are 1-1, for example k, K, and KELVIN SIGN
             * all fold to k, and all are single characters.   But some folds
             * expand to more than one character, so for example LATIN SMALL
             * LIGATURE FFI folds to the three character sequence 'ffi'.  If
             * the string beginning at 'uc' is 'ffi', it could be matched by
             * three characters, or just by the one ligature character. (It
             * could also be matched by two characters: LATIN SMALL LIGATURE FF
             * followed by 'i', or by 'f' followed by LATIN SMALL LIGATURE FI).
             * (Of course 'I' and/or 'F' instead of 'i' and 'f' can also
             * match.)  The trie needs to know the minimum and maximum number
             * of characters that could match so that it can use size alone to
             * quickly reject many match attempts.  The max is simple: it is
             * the number of folded characters in this branch (since a fold is
             * never shorter than what folds to it. */

            maxchars++;

            /* And the min is equal to the max if not under /i (indicated by
             * 'folder' being NULL), or there are no multi-character folds.  If
             * there is a multi-character fold, the min is incremented just
             * once, for the character that folds to the sequence.  Each
             * character in the sequence needs to be added to the list below of
             * characters in the trie, but we count only the first towards the
             * min number of characters needed.  This is done through the
             * variable 'foldlen', which is returned by the macros that look
             * for these sequences as the number of bytes the sequence
             * occupies.  Each time through the loop, we decrement 'foldlen' by
             * how many bytes the current char occupies.  Only when it reaches
             * 0 do we increment 'minchars' or look for another multi-character
             * sequence. */
            if (folder == NULL) {
                minchars++;
            }
            else if (foldlen > 0) {
                foldlen -= (UTF) ? UTF8SKIP(uc) : 1;
            }
            else {
                minchars++;

                /* See if *uc is the beginning of a multi-character fold.  If
                 * so, we decrement the length remaining to look at, to account
                 * for the current character this iteration.  (We can use 'uc'
                 * instead of the fold returned by TRIE_READ_CHAR because for
                 * non-UTF, the latin1_safe macro is smart enough to account
                 * for all the unfolded characters, and because for UTF, the
                 * string will already have been folded earlier in the
                 * compilation process */
                if (UTF) {
                    if ((foldlen = is_MULTI_CHAR_FOLD_utf8_safe(uc, e))) {
                        foldlen -= UTF8SKIP(uc);
                    }
                }
                else if ((foldlen = is_MULTI_CHAR_FOLD_latin1_safe(uc, e))) {
                    foldlen--;
                }
            }

            /* The current character (and any potential folds) should be added
             * to the possible matching characters for this position in this
             * branch */
            if ( uvc < 256 ) {
                if ( folder ) {
                    U8 folded= folder[ (U8) uvc ];
                    if ( !trie->charmap[ folded ] ) {
                        trie->charmap[ folded ]=( ++trie->uniquecharcount );
                        TRIE_STORE_REVCHAR( folded );
                    }
                }
                if ( !trie->charmap[ uvc ] ) {
                    trie->charmap[ uvc ]=( ++trie->uniquecharcount );
                    TRIE_STORE_REVCHAR( uvc );
                }
                if ( set_bit ) {
		    /* store the codepoint in the bitmap, and its folded
		     * equivalent. */
                    TRIE_BITMAP_SET_FOLDED(trie, uvc, folder);
                    set_bit = 0; /* We've done our bit :-) */
                }
            } else {

                /* XXX We could come up with the list of code points that fold
                 * to this using PL_utf8_foldclosures, except not for
                 * multi-char folds, as there may be multiple combinations
                 * there that could work, which needs to wait until runtime to
                 * resolve (The comment about LIGATURE FFI above is such an
                 * example */

                SV** svpp;
                if ( !widecharmap )
                    widecharmap = newHV();

                svpp = hv_fetch( widecharmap, (char*)&uvc, sizeof( UV ), 1 );

                if ( !svpp )
                    Perl_croak( aTHX_ "error creating/fetching widecharmap entry for 0x%" UVXf, uvc );

                if ( !SvTRUE( *svpp ) ) {
                    sv_setiv( *svpp, ++trie->uniquecharcount );
                    TRIE_STORE_REVCHAR(uvc);
                }
            }
        } /* end loop through characters in this branch of the trie */

        /* We take the min and max for this branch and combine to find the min
         * and max for all branches processed so far */
        if( cur == first ) {
            trie->minlen = minchars;
            trie->maxlen = maxchars;
        } else if (minchars < trie->minlen) {
            trie->minlen = minchars;
        } else if (maxchars > trie->maxlen) {
            trie->maxlen = maxchars;
        }
    } /* end first pass */
    DEBUG_TRIE_COMPILE_r(
        Perl_re_indentf( aTHX_
                "TRIE(%s): W:%d C:%d Uq:%d Min:%d Max:%d\n",
                depth+1,
                ( widecharmap ? "UTF8" : "NATIVE" ), (int)word_count,
		(int)TRIE_CHARCOUNT(trie), trie->uniquecharcount,
		(int)trie->minlen, (int)trie->maxlen )
    );

    /*
        We now know what we are dealing with in terms of unique chars and
        string sizes so we can calculate how much memory a naive
        representation using a flat table  will take. If it's over a reasonable
        limit (as specified by ${^RE_TRIE_MAXBUF}) we use a more memory
        conservative but potentially much slower representation using an array
        of lists.

        At the end we convert both representations into the same compressed
        form that will be used in regexec.c for matching with. The latter
        is a form that cannot be used to construct with but has memory
        properties similar to the list form and access properties similar
        to the table form making it both suitable for fast searches and
        small enough that its feasable to store for the duration of a program.

        See the comment in the code where the compressed table is produced
        inplace from the flat tabe representation for an explanation of how
        the compression works.

    */


    Newx(prev_states, TRIE_CHARCOUNT(trie) + 2, U32);
    prev_states[1] = 0;

    if ( (IV)( ( TRIE_CHARCOUNT(trie) + 1 ) * trie->uniquecharcount + 1)
                                                    > SvIV(re_trie_maxbuff) )
    {
        /*
            Second Pass -- Array Of Lists Representation

            Each state will be represented by a list of charid:state records
            (reg_trie_trans_le) the first such element holds the CUR and LEN
            points of the allocated array. (See defines above).

            We build the initial structure using the lists, and then convert
            it into the compressed table form which allows faster lookups
            (but cant be modified once converted).
        */

        STRLEN transcount = 1;

        DEBUG_TRIE_COMPILE_MORE_r( Perl_re_indentf( aTHX_  "Compiling trie using list compiler\n",
            depth+1));

	trie->states = (reg_trie_state *)
	    PerlMemShared_calloc( TRIE_CHARCOUNT(trie) + 2,
				  sizeof(reg_trie_state) );
        TRIE_LIST_NEW(1);
        next_alloc = 2;

        for ( cur = first ; cur < last ; cur = regnext( cur ) ) {

            regnode *noper   = NEXTOPER( cur );
	    U32 state        = 1;         /* required init */
	    U16 charid       = 0;         /* sanity init */
            U32 wordlen      = 0;         /* required init */

            if (OP(noper) == NOTHING) {
                regnode *noper_next= regnext(noper);
                if (noper_next < tail)
                    noper= noper_next;
            }

            if ( noper < tail && ( OP(noper) == flags || ( flags == EXACTFU && OP(noper) == EXACTFU_SS ) ) ) {
                const U8 *uc= (U8*)STRING(noper);
                const U8 *e= uc + STR_LEN(noper);

                for ( ; uc < e ; uc += len ) {

                    TRIE_READ_CHAR;

                    if ( uvc < 256 ) {
                        charid = trie->charmap[ uvc ];
		    } else {
                        SV** const svpp = hv_fetch( widecharmap,
                                                    (char*)&uvc,
                                                    sizeof( UV ),
                                                    0);
                        if ( !svpp ) {
                            charid = 0;
                        } else {
                            charid=(U16)SvIV( *svpp );
                        }
		    }
                    /* charid is now 0 if we dont know the char read, or
                     * nonzero if we do */
                    if ( charid ) {

                        U16 check;
                        U32 newstate = 0;

                        charid--;
                        if ( !trie->states[ state ].trans.list ) {
                            TRIE_LIST_NEW( state );
			}
                        for ( check = 1;
                              check <= TRIE_LIST_USED( state );
                              check++ )
                        {
                            if ( TRIE_LIST_ITEM( state, check ).forid
                                                                    == charid )
                            {
                                newstate = TRIE_LIST_ITEM( state, check ).newstate;
                                break;
                            }
                        }
                        if ( ! newstate ) {
                            newstate = next_alloc++;
			    prev_states[newstate] = state;
                            TRIE_LIST_PUSH( state, charid, newstate );
                            transcount++;
                        }
                        state = newstate;
                    } else {
                        Perl_croak( aTHX_ "panic! In trie construction, no char mapping for %" IVdf, uvc );
		    }
		}
	    }
            TRIE_HANDLE_WORD(state);

        } /* end second pass */

        /* next alloc is the NEXT state to be allocated */
        trie->statecount = next_alloc;
        trie->states = (reg_trie_state *)
	    PerlMemShared_realloc( trie->states,
				   next_alloc
				   * sizeof(reg_trie_state) );

        /* and now dump it out before we compress it */
        DEBUG_TRIE_COMPILE_MORE_r(dump_trie_interim_list(trie, widecharmap,
							 revcharmap, next_alloc,
							 depth+1)
        );

        trie->trans = (reg_trie_trans *)
	    PerlMemShared_calloc( transcount, sizeof(reg_trie_trans) );
        {
            U32 state;
            U32 tp = 0;
            U32 zp = 0;


            for( state=1 ; state < next_alloc ; state ++ ) {
                U32 base=0;

                /*
                DEBUG_TRIE_COMPILE_MORE_r(
                    Perl_re_printf( aTHX_  "tp: %d zp: %d ",tp,zp)
                );
                */

                if (trie->states[state].trans.list) {
                    U16 minid=TRIE_LIST_ITEM( state, 1).forid;
                    U16 maxid=minid;
		    U16 idx;

                    for( idx = 2 ; idx <= TRIE_LIST_USED( state ) ; idx++ ) {
			const U16 forid = TRIE_LIST_ITEM( state, idx).forid;
			if ( forid < minid ) {
			    minid=forid;
			} else if ( forid > maxid ) {
			    maxid=forid;
			}
                    }
                    if ( transcount < tp + maxid - minid + 1) {
                        transcount *= 2;
			trie->trans = (reg_trie_trans *)
			    PerlMemShared_realloc( trie->trans,
						     transcount
						     * sizeof(reg_trie_trans) );
                        Zero( trie->trans + (transcount / 2),
                              transcount / 2,
                              reg_trie_trans );
                    }
                    base = trie->uniquecharcount + tp - minid;
                    if ( maxid == minid ) {
                        U32 set = 0;
                        for ( ; zp < tp ; zp++ ) {
                            if ( ! trie->trans[ zp ].next ) {
                                base = trie->uniquecharcount + zp - minid;
                                trie->trans[ zp ].next = TRIE_LIST_ITEM( state,
                                                                   1).newstate;
                                trie->trans[ zp ].check = state;
                                set = 1;
                                break;
                            }
                        }
                        if ( !set ) {
                            trie->trans[ tp ].next = TRIE_LIST_ITEM( state,
                                                                   1).newstate;
                            trie->trans[ tp ].check = state;
                            tp++;
                            zp = tp;
                        }
                    } else {
                        for ( idx=1; idx <= TRIE_LIST_USED( state ) ; idx++ ) {
                            const U32 tid = base
                                           - trie->uniquecharcount
                                           + TRIE_LIST_ITEM( state, idx ).forid;
                            trie->trans[ tid ].next = TRIE_LIST_ITEM( state,
                                                                idx ).newstate;
                            trie->trans[ tid ].check = state;
                        }
                        tp += ( maxid - minid + 1 );
                    }
                    Safefree(trie->states[ state ].trans.list);
                }
                /*
                DEBUG_TRIE_COMPILE_MORE_r(
                    Perl_re_printf( aTHX_  " base: %d\n",base);
                );
                */
                trie->states[ state ].trans.base=base;
            }
            trie->lasttrans = tp + 1;
        }
    } else {
        /*
           Second Pass -- Flat Table Representation.

           we dont use the 0 slot of either trans[] or states[] so we add 1 to
           each.  We know that we will need Charcount+1 trans at most to store
           the data (one row per char at worst case) So we preallocate both
           structures assuming worst case.

           We then construct the trie using only the .next slots of the entry
           structs.

           We use the .check field of the first entry of the node temporarily
           to make compression both faster and easier by keeping track of how
           many non zero fields are in the node.

           Since trans are numbered from 1 any 0 pointer in the table is a FAIL
           transition.

           There are two terms at use here: state as a TRIE_NODEIDX() which is
           a number representing the first entry of the node, and state as a
           TRIE_NODENUM() which is the trans number. state 1 is TRIE_NODEIDX(1)
           and TRIE_NODENUM(1), state 2 is TRIE_NODEIDX(2) and TRIE_NODENUM(3)
           if there are 2 entrys per node. eg:

             A B       A B
          1. 2 4    1. 3 7
          2. 0 3    3. 0 5
          3. 0 0    5. 0 0
          4. 0 0    7. 0 0

           The table is internally in the right hand, idx form. However as we
           also have to deal with the states array which is indexed by nodenum
           we have to use TRIE_NODENUM() to convert.

        */
        DEBUG_TRIE_COMPILE_MORE_r( Perl_re_indentf( aTHX_  "Compiling trie using table compiler\n",
            depth+1));

	trie->trans = (reg_trie_trans *)
	    PerlMemShared_calloc( ( TRIE_CHARCOUNT(trie) + 1 )
				  * trie->uniquecharcount + 1,
				  sizeof(reg_trie_trans) );
        trie->states = (reg_trie_state *)
	    PerlMemShared_calloc( TRIE_CHARCOUNT(trie) + 2,
				  sizeof(reg_trie_state) );
        next_alloc = trie->uniquecharcount + 1;


        for ( cur = first ; cur < last ; cur = regnext( cur ) ) {

            regnode *noper   = NEXTOPER( cur );

            U32 state        = 1;         /* required init */

            U16 charid       = 0;         /* sanity init */
            U32 accept_state = 0;         /* sanity init */

            U32 wordlen      = 0;         /* required init */

            if (OP(noper) == NOTHING) {
                regnode *noper_next= regnext(noper);
                if (noper_next < tail)
                    noper= noper_next;
            }

            if ( noper < tail && ( OP(noper) == flags || ( flags == EXACTFU && OP(noper) == EXACTFU_SS ) ) ) {
                const U8 *uc= (U8*)STRING(noper);
                const U8 *e= uc + STR_LEN(noper);

                for ( ; uc < e ; uc += len ) {

                    TRIE_READ_CHAR;

                    if ( uvc < 256 ) {
                        charid = trie->charmap[ uvc ];
                    } else {
                        SV* const * const svpp = hv_fetch( widecharmap,
                                                           (char*)&uvc,
                                                           sizeof( UV ),
                                                           0);
                        charid = svpp ? (U16)SvIV(*svpp) : 0;
                    }
                    if ( charid ) {
                        charid--;
                        if ( !trie->trans[ state + charid ].next ) {
                            trie->trans[ state + charid ].next = next_alloc;
                            trie->trans[ state ].check++;
			    prev_states[TRIE_NODENUM(next_alloc)]
				    = TRIE_NODENUM(state);
                            next_alloc += trie->uniquecharcount;
                        }
                        state = trie->trans[ state + charid ].next;
                    } else {
                        Perl_croak( aTHX_ "panic! In trie construction, no char mapping for %" IVdf, uvc );
                    }
                    /* charid is now 0 if we dont know the char read, or
                     * nonzero if we do */
                }
            }
            accept_state = TRIE_NODENUM( state );
            TRIE_HANDLE_WORD(accept_state);

        } /* end second pass */

        /* and now dump it out before we compress it */
        DEBUG_TRIE_COMPILE_MORE_r(dump_trie_interim_table(trie, widecharmap,
							  revcharmap,
							  next_alloc, depth+1));

        {
        /*
           * Inplace compress the table.*

           For sparse data sets the table constructed by the trie algorithm will
           be mostly 0/FAIL transitions or to put it another way mostly empty.
           (Note that leaf nodes will not contain any transitions.)

           This algorithm compresses the tables by eliminating most such
           transitions, at the cost of a modest bit of extra work during lookup:

           - Each states[] entry contains a .base field which indicates the
           index in the state[] array wheres its transition data is stored.

           - If .base is 0 there are no valid transitions from that node.

           - If .base is nonzero then charid is added to it to find an entry in
           the trans array.

           -If trans[states[state].base+charid].check!=state then the
           transition is taken to be a 0/Fail transition. Thus if there are fail
           transitions at the front of the node then the .base offset will point
           somewhere inside the previous nodes data (or maybe even into a node
           even earlier), but the .check field determines if the transition is
           valid.

           XXX - wrong maybe?
           The following process inplace converts the table to the compressed
           table: We first do not compress the root node 1,and mark all its
           .check pointers as 1 and set its .base pointer as 1 as well. This
           allows us to do a DFA construction from the compressed table later,
           and ensures that any .base pointers we calculate later are greater
           than 0.

           - We set 'pos' to indicate the first entry of the second node.

           - We then iterate over the columns of the node, finding the first and
           last used entry at l and m. We then copy l..m into pos..(pos+m-l),
           and set the .check pointers accordingly, and advance pos
           appropriately and repreat for the next node. Note that when we copy
           the next pointers we have to convert them from the original
           NODEIDX form to NODENUM form as the former is not valid post
           compression.

           - If a node has no transitions used we mark its base as 0 and do not
           advance the pos pointer.

           - If a node only has one transition we use a second pointer into the
           structure to fill in allocated fail transitions from other states.
           This pointer is independent of the main pointer and scans forward
           looking for null transitions that are allocated to a state. When it
           finds one it writes the single transition into the "hole".  If the
           pointer doesnt find one the single transition is appended as normal.

           - Once compressed we can Renew/realloc the structures to release the
           excess space.

           See "Table-Compression Methods" in sec 3.9 of the Red Dragon,
           specifically Fig 3.47 and the associated pseudocode.

           demq
        */
        const U32 laststate = TRIE_NODENUM( next_alloc );
	U32 state, charid;
        U32 pos = 0, zp=0;
        trie->statecount = laststate;

        for ( state = 1 ; state < laststate ; state++ ) {
            U8 flag = 0;
	    const U32 stateidx = TRIE_NODEIDX( state );
	    const U32 o_used = trie->trans[ stateidx ].check;
	    U32 used = trie->trans[ stateidx ].check;
            trie->trans[ stateidx ].check = 0;

            for ( charid = 0;
                  used && charid < trie->uniquecharcount;
                  charid++ )
            {
                if ( flag || trie->trans[ stateidx + charid ].next ) {
                    if ( trie->trans[ stateidx + charid ].next ) {
                        if (o_used == 1) {
                            for ( ; zp < pos ; zp++ ) {
                                if ( ! trie->trans[ zp ].next ) {
                                    break;
                                }
                            }
                            trie->states[ state ].trans.base
                                                    = zp
                                                      + trie->uniquecharcount
                                                      - charid ;
                            trie->trans[ zp ].next
                                = SAFE_TRIE_NODENUM( trie->trans[ stateidx
                                                             + charid ].next );
                            trie->trans[ zp ].check = state;
                            if ( ++zp > pos ) pos = zp;
                            break;
                        }
                        used--;
                    }
                    if ( !flag ) {
                        flag = 1;
                        trie->states[ state ].trans.base
                                       = pos + trie->uniquecharcount - charid ;
                    }
                    trie->trans[ pos ].next
                        = SAFE_TRIE_NODENUM(
                                       trie->trans[ stateidx + charid ].next );
                    trie->trans[ pos ].check = state;
                    pos++;
                }
            }
        }
        trie->lasttrans = pos + 1;
        trie->states = (reg_trie_state *)
	    PerlMemShared_realloc( trie->states, laststate
				   * sizeof(reg_trie_state) );
        DEBUG_TRIE_COMPILE_MORE_r(
            Perl_re_indentf( aTHX_  "Alloc: %d Orig: %" IVdf " elements, Final:%" IVdf ". Savings of %%%5.2f\n",
                depth+1,
                (int)( ( TRIE_CHARCOUNT(trie) + 1 ) * trie->uniquecharcount
                       + 1 ),
                (IV)next_alloc,
                (IV)pos,
                ( ( next_alloc - pos ) * 100 ) / (double)next_alloc );
            );

        } /* end table compress */
    }
    DEBUG_TRIE_COMPILE_MORE_r(
            Perl_re_indentf( aTHX_  "Statecount:%" UVxf " Lasttrans:%" UVxf "\n",
                depth+1,
                (UV)trie->statecount,
                (UV)trie->lasttrans)
    );
    /* resize the trans array to remove unused space */
    trie->trans = (reg_trie_trans *)
	PerlMemShared_realloc( trie->trans, trie->lasttrans
			       * sizeof(reg_trie_trans) );

    {   /* Modify the program and insert the new TRIE node */
        U8 nodetype =(U8)(flags & 0xFF);
        char *str=NULL;

#ifdef DEBUGGING
        regnode *optimize = NULL;
#ifdef RE_TRACK_PATTERN_OFFSETS

        U32 mjd_offset = 0;
        U32 mjd_nodelen = 0;
#endif /* RE_TRACK_PATTERN_OFFSETS */
#endif /* DEBUGGING */
        /*
           This means we convert either the first branch or the first Exact,
           depending on whether the thing following (in 'last') is a branch
           or not and whther first is the startbranch (ie is it a sub part of
           the alternation or is it the whole thing.)
           Assuming its a sub part we convert the EXACT otherwise we convert
           the whole branch sequence, including the first.
         */
        /* Find the node we are going to overwrite */
        if ( first != startbranch || OP( last ) == BRANCH ) {
            /* branch sub-chain */
            NEXT_OFF( first ) = (U16)(last - first);
#ifdef RE_TRACK_PATTERN_OFFSETS
            DEBUG_r({
                mjd_offset= Node_Offset((convert));
                mjd_nodelen= Node_Length((convert));
            });
#endif
            /* whole branch chain */
        }
#ifdef RE_TRACK_PATTERN_OFFSETS
        else {
            DEBUG_r({
                const  regnode *nop = NEXTOPER( convert );
                mjd_offset= Node_Offset((nop));
                mjd_nodelen= Node_Length((nop));
            });
        }
        DEBUG_OPTIMISE_r(
            Perl_re_indentf( aTHX_  "MJD offset:%" UVuf " MJD length:%" UVuf "\n",
                depth+1,
                (UV)mjd_offset, (UV)mjd_nodelen)
        );
#endif
        /* But first we check to see if there is a common prefix we can
           split out as an EXACT and put in front of the TRIE node.  */
        trie->startstate= 1;
        if ( trie->bitmap && !widecharmap && !trie->jump  ) {
            /* we want to find the first state that has more than
             * one transition, if that state is not the first state
             * then we have a common prefix which we can remove.
             */
            U32 state;
            for ( state = 1 ; state < trie->statecount-1 ; state++ ) {
                U32 ofs = 0;
                I32 first_ofs = -1; /* keeps track of the ofs of the first
                                       transition, -1 means none */
                U32 count = 0;
                const U32 base = trie->states[ state ].trans.base;

                /* does this state terminate an alternation? */
                if ( trie->states[state].wordnum )
                        count = 1;

                for ( ofs = 0 ; ofs < trie->uniquecharcount ; ofs++ ) {
                    if ( ( base + ofs >= trie->uniquecharcount ) &&
                         ( base + ofs - trie->uniquecharcount < trie->lasttrans ) &&
                         trie->trans[ base + ofs - trie->uniquecharcount ].check == state )
                    {
                        if ( ++count > 1 ) {
                            /* we have more than one transition */
                            SV **tmp;
                            U8 *ch;
                            /* if this is the first state there is no common prefix
                             * to extract, so we can exit */
                            if ( state == 1 ) break;
                            tmp = av_fetch( revcharmap, ofs, 0);
                            ch = (U8*)SvPV_nolen_const( *tmp );

                            /* if we are on count 2 then we need to initialize the
                             * bitmap, and store the previous char if there was one
                             * in it*/
                            if ( count == 2 ) {
                                /* clear the bitmap */
                                Zero(trie->bitmap, ANYOF_BITMAP_SIZE, char);
                                DEBUG_OPTIMISE_r(
                                    Perl_re_indentf( aTHX_  "New Start State=%" UVuf " Class: [",
                                        depth+1,
                                        (UV)state));
                                if (first_ofs >= 0) {
                                    SV ** const tmp = av_fetch( revcharmap, first_ofs, 0);
				    const U8 * const ch = (U8*)SvPV_nolen_const( *tmp );

                                    TRIE_BITMAP_SET_FOLDED(trie,*ch,folder);
                                    DEBUG_OPTIMISE_r(
                                        Perl_re_printf( aTHX_  "%s", (char*)ch)
                                    );
				}
			    }
                            /* store the current firstchar in the bitmap */
                            TRIE_BITMAP_SET_FOLDED(trie,*ch,folder);
                            DEBUG_OPTIMISE_r(Perl_re_printf( aTHX_ "%s", ch));
			}
                        first_ofs = ofs;
		    }
                }
                if ( count == 1 ) {
                    /* This state has only one transition, its transition is part
                     * of a common prefix - we need to concatenate the char it
                     * represents to what we have so far. */
                    SV **tmp = av_fetch( revcharmap, first_ofs, 0);
                    STRLEN len;
                    char *ch = SvPV( *tmp, len );
                    DEBUG_OPTIMISE_r({
                        SV *sv=sv_newmortal();
                        Perl_re_indentf( aTHX_  "Prefix State: %" UVuf " Ofs:%" UVuf " Char='%s'\n",
                            depth+1,
                            (UV)state, (UV)first_ofs,
                            pv_pretty(sv, SvPV_nolen_const(*tmp), SvCUR(*tmp), 6,
	                        PL_colors[0], PL_colors[1],
	                        (SvUTF8(*tmp) ? PERL_PV_ESCAPE_UNI : 0) |
	                        PERL_PV_ESCAPE_FIRSTCHAR
                            )
                        );
                    });
                    if ( state==1 ) {
                        OP( convert ) = nodetype;
                        str=STRING(convert);
                        STR_LEN(convert)=0;
                    }
                    STR_LEN(convert) += len;
                    while (len--)
                        *str++ = *ch++;
		} else {
#ifdef DEBUGGING
		    if (state>1)
                        DEBUG_OPTIMISE_r(Perl_re_printf( aTHX_ "]\n"));
#endif
		    break;
		}
	    }
	    trie->prefixlen = (state-1);
            if (str) {
                regnode *n = convert+NODE_SZ_STR(convert);
                NEXT_OFF(convert) = NODE_SZ_STR(convert);
                trie->startstate = state;
                trie->minlen -= (state - 1);
                trie->maxlen -= (state - 1);
#ifdef DEBUGGING
               /* At least the UNICOS C compiler choked on this
                * being argument to DEBUG_r(), so let's just have
                * it right here. */
               if (
#ifdef PERL_EXT_RE_BUILD
                   1
#else
                   DEBUG_r_TEST
#endif
                   ) {
                   regnode *fix = convert;
                   U32 word = trie->wordcount;
                   mjd_nodelen++;
                   Set_Node_Offset_Length(convert, mjd_offset, state - 1);
                   while( ++fix < n ) {
                       Set_Node_Offset_Length(fix, 0, 0);
                   }
                   while (word--) {
                       SV ** const tmp = av_fetch( trie_words, word, 0 );
                       if (tmp) {
                           if ( STR_LEN(convert) <= SvCUR(*tmp) )
                               sv_chop(*tmp, SvPV_nolen(*tmp) + STR_LEN(convert));
                           else
                               sv_chop(*tmp, SvPV_nolen(*tmp) + SvCUR(*tmp));
                       }
                   }
               }
#endif
                if (trie->maxlen) {
                    convert = n;
		} else {
                    NEXT_OFF(convert) = (U16)(tail - convert);
                    DEBUG_r(optimize= n);
                }
            }
        }
        if (!jumper)
            jumper = last;
        if ( trie->maxlen ) {
	    NEXT_OFF( convert ) = (U16)(tail - convert);
	    ARG_SET( convert, data_slot );
	    /* Store the offset to the first unabsorbed branch in
	       jump[0], which is otherwise unused by the jump logic.
	       We use this when dumping a trie and during optimisation. */
	    if (trie->jump)
	        trie->jump[0] = (U16)(nextbranch - convert);

            /* If the start state is not accepting (meaning there is no empty string/NOTHING)
	     *   and there is a bitmap
	     *   and the first "jump target" node we found leaves enough room
	     * then convert the TRIE node into a TRIEC node, with the bitmap
	     * embedded inline in the opcode - this is hypothetically faster.
	     */
            if ( !trie->states[trie->startstate].wordnum
		 && trie->bitmap
		 && ( (char *)jumper - (char *)convert) >= (int)sizeof(struct regnode_charclass) )
            {
                OP( convert ) = TRIEC;
                Copy(trie->bitmap, ((struct regnode_charclass *)convert)->bitmap, ANYOF_BITMAP_SIZE, char);
                PerlMemShared_free(trie->bitmap);
                trie->bitmap= NULL;
            } else
                OP( convert ) = TRIE;

            /* store the type in the flags */
            convert->flags = nodetype;
            DEBUG_r({
            optimize = convert
                      + NODE_STEP_REGNODE
                      + regarglen[ OP( convert ) ];
            });
            /* XXX We really should free up the resource in trie now,
                   as we won't use them - (which resources?) dmq */
        }
        /* needed for dumping*/
        DEBUG_r(if (optimize) {
            regnode *opt = convert;

            while ( ++opt < optimize) {
                Set_Node_Offset_Length(opt,0,0);
            }
            /*
                Try to clean up some of the debris left after the
                optimisation.
             */
            while( optimize < jumper ) {
                mjd_nodelen += Node_Length((optimize));
                OP( optimize ) = OPTIMIZED;
                Set_Node_Offset_Length(optimize,0,0);
                optimize++;
            }
            Set_Node_Offset_Length(convert,mjd_offset,mjd_nodelen);
        });
    } /* end node insert */

    /*  Finish populating the prev field of the wordinfo array.  Walk back
     *  from each accept state until we find another accept state, and if
     *  so, point the first word's .prev field at the second word. If the
     *  second already has a .prev field set, stop now. This will be the
     *  case either if we've already processed that word's accept state,
     *  or that state had multiple words, and the overspill words were
     *  already linked up earlier.
     */
    {
	U16 word;
	U32 state;
	U16 prev;

	for (word=1; word <= trie->wordcount; word++) {
	    prev = 0;
	    if (trie->wordinfo[word].prev)
		continue;
	    state = trie->wordinfo[word].accept;
	    while (state) {
		state = prev_states[state];
		if (!state)
		    break;
		prev = trie->states[state].wordnum;
		if (prev)
		    break;
	    }
	    trie->wordinfo[word].prev = prev;
	}
	Safefree(prev_states);
    }


    /* and now dump out the compressed format */
    DEBUG_TRIE_COMPILE_r(dump_trie(trie, widecharmap, revcharmap, depth+1));

    RExC_rxi->data->data[ data_slot + 1 ] = (void*)widecharmap;
#ifdef DEBUGGING
    RExC_rxi->data->data[ data_slot + TRIE_WORDS_OFFSET ] = (void*)trie_words;
    RExC_rxi->data->data[ data_slot + 3 ] = (void*)revcharmap;
#else
    SvREFCNT_dec_NN(revcharmap);
#endif
    return trie->jump
           ? MADE_JUMP_TRIE
           : trie->startstate>1
             ? MADE_EXACT_TRIE
             : MADE_TRIE;
}