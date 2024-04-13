S_reg(pTHX_ RExC_state_t *pRExC_state, I32 paren, I32 *flagp,U32 depth)
    /* paren: Parenthesized? 0=top; 1,2=inside '(': changed to letter.
     * 2 is like 1, but indicates that nextchar() has been called to advance
     * RExC_parse beyond the '('.  Things like '(?' are indivisible tokens, and
     * this flag alerts us to the need to check for that */
{
    regnode *ret;		/* Will be the head of the group. */
    regnode *br;
    regnode *lastbr;
    regnode *ender = NULL;
    I32 parno = 0;
    I32 flags;
    U32 oregflags = RExC_flags;
    bool have_branch = 0;
    bool is_open = 0;
    I32 freeze_paren = 0;
    I32 after_freeze = 0;
    I32 num; /* numeric backreferences */

    char * parse_start = RExC_parse; /* MJD */
    char * const oregcomp_parse = RExC_parse;

    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REG;
    DEBUG_PARSE("reg ");

    *flagp = 0;				/* Tentatively. */

    /* Having this true makes it feasible to have a lot fewer tests for the
     * parse pointer being in scope.  For example, we can write
     *      while(isFOO(*RExC_parse)) RExC_parse++;
     * instead of
     *      while(RExC_parse < RExC_end && isFOO(*RExC_parse)) RExC_parse++;
     */
    assert(*RExC_end == '\0');

    /* Make an OPEN node, if parenthesized. */
    if (paren) {

        /* Under /x, space and comments can be gobbled up between the '(' and
         * here (if paren ==2).  The forms '(*VERB' and '(?...' disallow such
         * intervening space, as the sequence is a token, and a token should be
         * indivisible */
        bool has_intervening_patws = paren == 2 && *(RExC_parse - 1) != '(';

        if (RExC_parse >= RExC_end) {
	    vFAIL("Unmatched (");
        }

        if ( *RExC_parse == '*') { /* (*VERB:ARG) */
	    char *start_verb = RExC_parse + 1;
	    STRLEN verb_len;
	    char *start_arg = NULL;
	    unsigned char op = 0;
            int arg_required = 0;
            int internal_argval = -1; /* if >-1 we are not allowed an argument*/

            if (has_intervening_patws) {
                RExC_parse++;   /* past the '*' */
                vFAIL("In '(*VERB...)', the '(' and '*' must be adjacent");
            }
	    while (RExC_parse < RExC_end && *RExC_parse != ')' ) {
	        if ( *RExC_parse == ':' ) {
	            start_arg = RExC_parse + 1;
	            break;
	        }
	        RExC_parse += UTF ? UTF8SKIP(RExC_parse) : 1;
	    }
	    verb_len = RExC_parse - start_verb;
	    if ( start_arg ) {
                if (RExC_parse >= RExC_end) {
                    goto unterminated_verb_pattern;
                }
	        RExC_parse += UTF ? UTF8SKIP(RExC_parse) : 1;
	        while ( RExC_parse < RExC_end && *RExC_parse != ')' )
                    RExC_parse += UTF ? UTF8SKIP(RExC_parse) : 1;
	        if ( RExC_parse >= RExC_end || *RExC_parse != ')' )
                  unterminated_verb_pattern:
	            vFAIL("Unterminated verb pattern argument");
	        if ( RExC_parse == start_arg )
	            start_arg = NULL;
	    } else {
	        if ( RExC_parse >= RExC_end || *RExC_parse != ')' )
	            vFAIL("Unterminated verb pattern");
	    }

            /* Here, we know that RExC_parse < RExC_end */

	    switch ( *start_verb ) {
            case 'A':  /* (*ACCEPT) */
                if ( memEQs(start_verb,verb_len,"ACCEPT") ) {
		    op = ACCEPT;
		    internal_argval = RExC_nestroot;
		}
		break;
            case 'C':  /* (*COMMIT) */
                if ( memEQs(start_verb,verb_len,"COMMIT") )
                    op = COMMIT;
                break;
            case 'F':  /* (*FAIL) */
                if ( verb_len==1 || memEQs(start_verb,verb_len,"FAIL") ) {
		    op = OPFAIL;
		}
		break;
            case ':':  /* (*:NAME) */
	    case 'M':  /* (*MARK:NAME) */
	        if ( verb_len==0 || memEQs(start_verb,verb_len,"MARK") ) {
                    op = MARKPOINT;
                    arg_required = 1;
                }
                break;
            case 'P':  /* (*PRUNE) */
                if ( memEQs(start_verb,verb_len,"PRUNE") )
                    op = PRUNE;
                break;
            case 'S':   /* (*SKIP) */
                if ( memEQs(start_verb,verb_len,"SKIP") )
                    op = SKIP;
                break;
            case 'T':  /* (*THEN) */
                /* [19:06] <TimToady> :: is then */
                if ( memEQs(start_verb,verb_len,"THEN") ) {
                    op = CUTGROUP;
                    RExC_seen |= REG_CUTGROUP_SEEN;
                }
                break;
	    }
	    if ( ! op ) {
	        RExC_parse += UTF ? UTF8SKIP(RExC_parse) : 1;
                vFAIL2utf8f(
                    "Unknown verb pattern '%" UTF8f "'",
                    UTF8fARG(UTF, verb_len, start_verb));
	    }
            if ( arg_required && !start_arg ) {
                vFAIL3("Verb pattern '%.*s' has a mandatory argument",
                    verb_len, start_verb);
            }
            if (internal_argval == -1) {
                ret = reganode(pRExC_state, op, 0);
            } else {
                ret = reg2Lanode(pRExC_state, op, 0, internal_argval);
            }
            RExC_seen |= REG_VERBARG_SEEN;
            if ( ! SIZE_ONLY ) {
                if (start_arg) {
                    SV *sv = newSVpvn( start_arg,
                                       RExC_parse - start_arg);
                    ARG(ret) = add_data( pRExC_state,
                                         STR_WITH_LEN("S"));
                    RExC_rxi->data->data[ARG(ret)]=(void*)sv;
                    ret->flags = 1;
                } else {
                    ret->flags = 0;
                }
                if ( internal_argval != -1 )
                    ARG2L_SET(ret, internal_argval);
            }
	    nextchar(pRExC_state);
	    return ret;
        }
        else if (*RExC_parse == '?') { /* (?...) */
	    bool is_logical = 0;
	    const char * const seqstart = RExC_parse;
            const char * endptr;
            if (has_intervening_patws) {
                RExC_parse++;
                vFAIL("In '(?...)', the '(' and '?' must be adjacent");
            }

	    RExC_parse++;           /* past the '?' */
            paren = *RExC_parse;    /* might be a trailing NUL, if not
                                       well-formed */
            RExC_parse += UTF ? UTF8SKIP(RExC_parse) : 1;
            if (RExC_parse > RExC_end) {
                paren = '\0';
            }
	    ret = NULL;			/* For look-ahead/behind. */
	    switch (paren) {

	    case 'P':	/* (?P...) variants for those used to PCRE/Python */
	        paren = *RExC_parse;
		if ( paren == '<') {    /* (?P<...>) named capture */
                    RExC_parse++;
                    if (RExC_parse >= RExC_end) {
                        vFAIL("Sequence (?P<... not terminated");
                    }
		    goto named_capture;
                }
                else if (paren == '>') {   /* (?P>name) named recursion */
                    RExC_parse++;
                    if (RExC_parse >= RExC_end) {
                        vFAIL("Sequence (?P>... not terminated");
                    }
                    goto named_recursion;
                }
                else if (paren == '=') {   /* (?P=...)  named backref */
                    RExC_parse++;
                    return handle_named_backref(pRExC_state, flagp,
                                                parse_start, ')');
                }
                RExC_parse += SKIP_IF_CHAR(RExC_parse);
                /* diag_listed_as: Sequence (?%s...) not recognized in regex; marked by <-- HERE in m/%s/ */
		vFAIL3("Sequence (%.*s...) not recognized",
                                RExC_parse-seqstart, seqstart);
		NOT_REACHED; /*NOTREACHED*/
            case '<':           /* (?<...) */
		if (*RExC_parse == '!')
		    paren = ',';
		else if (*RExC_parse != '=')
              named_capture:
		{               /* (?<...>) */
		    char *name_start;
		    SV *svname;
		    paren= '>';
                /* FALLTHROUGH */
            case '\'':          /* (?'...') */
                    name_start = RExC_parse;
                    svname = reg_scan_name(pRExC_state,
                        SIZE_ONLY    /* reverse test from the others */
                        ? REG_RSN_RETURN_NAME
                        : REG_RSN_RETURN_NULL);
		    if (   RExC_parse == name_start
                        || RExC_parse >= RExC_end
                        || *RExC_parse != paren)
                    {
		        vFAIL2("Sequence (?%c... not terminated",
		            paren=='>' ? '<' : paren);
                    }
		    if (SIZE_ONLY) {
			HE *he_str;
			SV *sv_dat = NULL;
                        if (!svname) /* shouldn't happen */
                            Perl_croak(aTHX_
                                "panic: reg_scan_name returned NULL");
                        if (!RExC_paren_names) {
                            RExC_paren_names= newHV();
                            sv_2mortal(MUTABLE_SV(RExC_paren_names));
#ifdef DEBUGGING
                            RExC_paren_name_list= newAV();
                            sv_2mortal(MUTABLE_SV(RExC_paren_name_list));
#endif
                        }
                        he_str = hv_fetch_ent( RExC_paren_names, svname, 1, 0 );
                        if ( he_str )
                            sv_dat = HeVAL(he_str);
                        if ( ! sv_dat ) {
                            /* croak baby croak */
                            Perl_croak(aTHX_
                                "panic: paren_name hash element allocation failed");
                        } else if ( SvPOK(sv_dat) ) {
                            /* (?|...) can mean we have dupes so scan to check
                               its already been stored. Maybe a flag indicating
                               we are inside such a construct would be useful,
                               but the arrays are likely to be quite small, so
                               for now we punt -- dmq */
                            IV count = SvIV(sv_dat);
                            I32 *pv = (I32*)SvPVX(sv_dat);
                            IV i;
                            for ( i = 0 ; i < count ; i++ ) {
                                if ( pv[i] == RExC_npar ) {
                                    count = 0;
                                    break;
                                }
                            }
                            if ( count ) {
                                pv = (I32*)SvGROW(sv_dat,
                                                SvCUR(sv_dat) + sizeof(I32)+1);
                                SvCUR_set(sv_dat, SvCUR(sv_dat) + sizeof(I32));
                                pv[count] = RExC_npar;
                                SvIV_set(sv_dat, SvIVX(sv_dat) + 1);
                            }
                        } else {
                            (void)SvUPGRADE(sv_dat,SVt_PVNV);
                            sv_setpvn(sv_dat, (char *)&(RExC_npar),
                                                                sizeof(I32));
                            SvIOK_on(sv_dat);
                            SvIV_set(sv_dat, 1);
                        }
#ifdef DEBUGGING
                        /* Yes this does cause a memory leak in debugging Perls
                         * */
                        if (!av_store(RExC_paren_name_list,
                                      RExC_npar, SvREFCNT_inc(svname)))
                            SvREFCNT_dec_NN(svname);
#endif

                        /*sv_dump(sv_dat);*/
                    }
                    nextchar(pRExC_state);
		    paren = 1;
		    goto capturing_parens;
		}
                RExC_seen |= REG_LOOKBEHIND_SEEN;
		RExC_in_lookbehind++;
		RExC_parse++;
                if (RExC_parse >= RExC_end) {
                    vFAIL("Sequence (?... not terminated");
                }

                /* FALLTHROUGH */
	    case '=':           /* (?=...) */
		RExC_seen_zerolen++;
                break;
	    case '!':           /* (?!...) */
		RExC_seen_zerolen++;
		/* check if we're really just a "FAIL" assertion */
                skip_to_be_ignored_text(pRExC_state, &RExC_parse,
                                        FALSE /* Don't force to /x */ );
	        if (*RExC_parse == ')') {
                    ret=reganode(pRExC_state, OPFAIL, 0);
	            nextchar(pRExC_state);
	            return ret;
	        }
	        break;
	    case '|':           /* (?|...) */
	        /* branch reset, behave like a (?:...) except that
	           buffers in alternations share the same numbers */
	        paren = ':';
	        after_freeze = freeze_paren = RExC_npar;
	        break;
	    case ':':           /* (?:...) */
	    case '>':           /* (?>...) */
		break;
	    case '$':           /* (?$...) */
	    case '@':           /* (?@...) */
		vFAIL2("Sequence (?%c...) not implemented", (int)paren);
		break;
	    case '0' :           /* (?0) */
	    case 'R' :           /* (?R) */
                if (RExC_parse == RExC_end || *RExC_parse != ')')
		    FAIL("Sequence (?R) not terminated");
                num = 0;
                RExC_seen |= REG_RECURSE_SEEN;
		*flagp |= POSTPONED;
                goto gen_recurse_regop;
		/*notreached*/
            /* named and numeric backreferences */
            case '&':            /* (?&NAME) */
                parse_start = RExC_parse - 1;
              named_recursion:
                {
    		    SV *sv_dat = reg_scan_name(pRExC_state,
    		        SIZE_ONLY ? REG_RSN_RETURN_NULL : REG_RSN_RETURN_DATA);
    		     num = sv_dat ? *((I32 *)SvPVX(sv_dat)) : 0;
                }
                if (RExC_parse >= RExC_end || *RExC_parse != ')')
                    vFAIL("Sequence (?&... not terminated");
                goto gen_recurse_regop;
                /* NOTREACHED */
            case '+':
                if (!(RExC_parse[0] >= '1' && RExC_parse[0] <= '9')) {
                    RExC_parse++;
                    vFAIL("Illegal pattern");
                }
                goto parse_recursion;
                /* NOTREACHED*/
            case '-': /* (?-1) */
                if (!(RExC_parse[0] >= '1' && RExC_parse[0] <= '9')) {
                    RExC_parse--; /* rewind to let it be handled later */
                    goto parse_flags;
                }
                /* FALLTHROUGH */
            case '1': case '2': case '3': case '4': /* (?1) */
	    case '5': case '6': case '7': case '8': case '9':
	        RExC_parse = (char *) seqstart + 1;  /* Point to the digit */
              parse_recursion:
                {
                    bool is_neg = FALSE;
                    UV unum;
                    parse_start = RExC_parse - 1; /* MJD */
                    if (*RExC_parse == '-') {
                        RExC_parse++;
                        is_neg = TRUE;
                    }
                    if (grok_atoUV(RExC_parse, &unum, &endptr)
                        && unum <= I32_MAX
                    ) {
                        num = (I32)unum;
                        RExC_parse = (char*)endptr;
                    } else
                        num = I32_MAX;
                    if (is_neg) {
                        /* Some limit for num? */
                        num = -num;
                    }
                }
	        if (*RExC_parse!=')')
	            vFAIL("Expecting close bracket");

              gen_recurse_regop:
                if ( paren == '-' ) {
                    /*
                    Diagram of capture buffer numbering.
                    Top line is the normal capture buffer numbers
                    Bottom line is the negative indexing as from
                    the X (the (?-2))

                    +   1 2    3 4 5 X          6 7
                       /(a(x)y)(a(b(c(?-2)d)e)f)(g(h))/
                    -   5 4    3 2 1 X          x x

                    */
                    num = RExC_npar + num;
                    if (num < 1)  {
                        RExC_parse++;
                        vFAIL("Reference to nonexistent group");
                    }
                } else if ( paren == '+' ) {
                    num = RExC_npar + num - 1;
                }
                /* We keep track how many GOSUB items we have produced.
                   To start off the ARG2L() of the GOSUB holds its "id",
                   which is used later in conjunction with RExC_recurse
                   to calculate the offset we need to jump for the GOSUB,
                   which it will store in the final representation.
                   We have to defer the actual calculation until much later
                   as the regop may move.
                 */

                ret = reg2Lanode(pRExC_state, GOSUB, num, RExC_recurse_count);
                if (!SIZE_ONLY) {
		    if (num > (I32)RExC_rx->nparens) {
			RExC_parse++;
			vFAIL("Reference to nonexistent group");
	            }
	            RExC_recurse_count++;
                    DEBUG_OPTIMISE_MORE_r(Perl_re_printf( aTHX_
                        "%*s%*s Recurse #%" UVuf " to %" IVdf "\n",
                              22, "|    |", (int)(depth * 2 + 1), "",
                              (UV)ARG(ret), (IV)ARG2L(ret)));
                }
                RExC_seen |= REG_RECURSE_SEEN;

                Set_Node_Length(ret, 1 + regarglen[OP(ret)]); /* MJD */
		Set_Node_Offset(ret, parse_start); /* MJD */

                *flagp |= POSTPONED;
                assert(*RExC_parse == ')');
                nextchar(pRExC_state);
                return ret;

            /* NOTREACHED */

	    case '?':           /* (??...) */
		is_logical = 1;
		if (*RExC_parse != '{') {
                    RExC_parse += SKIP_IF_CHAR(RExC_parse);
                    /* diag_listed_as: Sequence (?%s...) not recognized in regex; marked by <-- HERE in m/%s/ */
                    vFAIL2utf8f(
                        "Sequence (%" UTF8f "...) not recognized",
                        UTF8fARG(UTF, RExC_parse-seqstart, seqstart));
		    NOT_REACHED; /*NOTREACHED*/
		}
		*flagp |= POSTPONED;
		paren = '{';
                RExC_parse++;
		/* FALLTHROUGH */
	    case '{':           /* (?{...}) */
	    {
		U32 n = 0;
		struct reg_code_block *cb;

		RExC_seen_zerolen++;

		if (   !pRExC_state->code_blocks
		    || pRExC_state->code_index
                                        >= pRExC_state->code_blocks->count
		    || pRExC_state->code_blocks->cb[pRExC_state->code_index].start
			!= (STRLEN)((RExC_parse -3 - (is_logical ? 1 : 0))
			    - RExC_start)
		) {
		    if (RExC_pm_flags & PMf_USE_RE_EVAL)
			FAIL("panic: Sequence (?{...}): no code block found\n");
		    FAIL("Eval-group not allowed at runtime, use re 'eval'");
		}
		/* this is a pre-compiled code block (?{...}) */
		cb = &pRExC_state->code_blocks->cb[pRExC_state->code_index];
		RExC_parse = RExC_start + cb->end;
		if (!SIZE_ONLY) {
		    OP *o = cb->block;
		    if (cb->src_regex) {
			n = add_data(pRExC_state, STR_WITH_LEN("rl"));
			RExC_rxi->data->data[n] =
			    (void*)SvREFCNT_inc((SV*)cb->src_regex);
			RExC_rxi->data->data[n+1] = (void*)o;
		    }
		    else {
			n = add_data(pRExC_state,
			       (RExC_pm_flags & PMf_HAS_CV) ? "L" : "l", 1);
			RExC_rxi->data->data[n] = (void*)o;
		    }
		}
		pRExC_state->code_index++;
		nextchar(pRExC_state);

		if (is_logical) {
                    regnode *eval;
		    ret = reg_node(pRExC_state, LOGICAL);

                    eval = reg2Lanode(pRExC_state, EVAL,
                                       n,

                                       /* for later propagation into (??{})
                                        * return value */
                                       RExC_flags & RXf_PMf_COMPILETIME
                                      );
		    if (!SIZE_ONLY) {
			ret->flags = 2;
                    }
                    REGTAIL(pRExC_state, ret, eval);
                    /* deal with the length of this later - MJD */
		    return ret;
		}
		ret = reg2Lanode(pRExC_state, EVAL, n, 0);
		Set_Node_Length(ret, RExC_parse - parse_start + 1);
		Set_Node_Offset(ret, parse_start);
		return ret;
	    }
	    case '(':           /* (?(?{...})...) and (?(?=...)...) */
	    {
	        int is_define= 0;
                const int DEFINE_len = sizeof("DEFINE") - 1;
		if (RExC_parse[0] == '?') {        /* (?(?...)) */
                    if (   RExC_parse < RExC_end - 1
                        && (   RExC_parse[1] == '='
                            || RExC_parse[1] == '!'
                            || RExC_parse[1] == '<'
                            || RExC_parse[1] == '{')
                    ) { /* Lookahead or eval. */
			I32 flag;
                        regnode *tail;

			ret = reg_node(pRExC_state, LOGICAL);
			if (!SIZE_ONLY)
			    ret->flags = 1;

                        tail = reg(pRExC_state, 1, &flag, depth+1);
                        if (flag & (RESTART_PASS1|NEED_UTF8)) {
                            *flagp = flag & (RESTART_PASS1|NEED_UTF8);
                            return NULL;
                        }
                        REGTAIL(pRExC_state, ret, tail);
			goto insert_if;
		    }
		    /* Fall through to ‘Unknown switch condition’ at the
		       end of the if/else chain. */
		}
		else if ( RExC_parse[0] == '<'     /* (?(<NAME>)...) */
		         || RExC_parse[0] == '\'' ) /* (?('NAME')...) */
	        {
	            char ch = RExC_parse[0] == '<' ? '>' : '\'';
	            char *name_start= RExC_parse++;
	            U32 num = 0;
	            SV *sv_dat=reg_scan_name(pRExC_state,
	                SIZE_ONLY ? REG_RSN_RETURN_NULL : REG_RSN_RETURN_DATA);
	            if (   RExC_parse == name_start
                        || RExC_parse >= RExC_end
                        || *RExC_parse != ch)
                    {
                        vFAIL2("Sequence (?(%c... not terminated",
                            (ch == '>' ? '<' : ch));
                    }
                    RExC_parse++;
	            if (!SIZE_ONLY) {
                        num = add_data( pRExC_state, STR_WITH_LEN("S"));
                        RExC_rxi->data->data[num]=(void*)sv_dat;
                        SvREFCNT_inc_simple_void(sv_dat);
                    }
                    ret = reganode(pRExC_state,NGROUPP,num);
                    goto insert_if_check_paren;
		}
		else if (memBEGINs(RExC_parse,
                                   (STRLEN) (RExC_end - RExC_parse),
                                   "DEFINE"))
                {
		    ret = reganode(pRExC_state,DEFINEP,0);
		    RExC_parse += DEFINE_len;
		    is_define = 1;
		    goto insert_if_check_paren;
		}
		else if (RExC_parse[0] == 'R') {
		    RExC_parse++;
                    /* parno == 0 => /(?(R)YES|NO)/  "in any form of recursion OR eval"
                     * parno == 1 => /(?(R0)YES|NO)/ "in GOSUB (?0) / (?R)"
                     * parno == 2 => /(?(R1)YES|NO)/ "in GOSUB (?1) (parno-1)"
                     */
		    parno = 0;
                    if (RExC_parse[0] == '0') {
                        parno = 1;
                        RExC_parse++;
                    }
                    else if (RExC_parse[0] >= '1' && RExC_parse[0] <= '9' ) {
                        UV uv;
                        if (grok_atoUV(RExC_parse, &uv, &endptr)
                            && uv <= I32_MAX
                        ) {
                            parno = (I32)uv + 1;
                            RExC_parse = (char*)endptr;
                        }
                        /* else "Switch condition not recognized" below */
		    } else if (RExC_parse[0] == '&') {
		        SV *sv_dat;
		        RExC_parse++;
		        sv_dat = reg_scan_name(pRExC_state,
                            SIZE_ONLY
                            ? REG_RSN_RETURN_NULL
                            : REG_RSN_RETURN_DATA);

                        /* we should only have a false sv_dat when
                         * SIZE_ONLY is true, and we always have false
                         * sv_dat when SIZE_ONLY is true.
                         * reg_scan_name() will VFAIL() if the name is
                         * unknown when SIZE_ONLY is false, and otherwise
                         * will return something, and when SIZE_ONLY is
                         * true, reg_scan_name() just parses the string,
                         * and doesnt return anything. (in theory) */
                        assert(SIZE_ONLY ? !sv_dat : !!sv_dat);

                        if (sv_dat)
                            parno = 1 + *((I32 *)SvPVX(sv_dat));
		    }
		    ret = reganode(pRExC_state,INSUBP,parno);
		    goto insert_if_check_paren;
		}
		else if (RExC_parse[0] >= '1' && RExC_parse[0] <= '9' ) {
                    /* (?(1)...) */
		    char c;
                    UV uv;
                    if (grok_atoUV(RExC_parse, &uv, &endptr)
                        && uv <= I32_MAX
                    ) {
                        parno = (I32)uv;
                        RExC_parse = (char*)endptr;
                    }
                    else {
                        vFAIL("panic: grok_atoUV returned FALSE");
                    }
                    ret = reganode(pRExC_state, GROUPP, parno);

                 insert_if_check_paren:
		    if (UCHARAT(RExC_parse) != ')') {
                        RExC_parse += UTF ? UTF8SKIP(RExC_parse) : 1;
			vFAIL("Switch condition not recognized");
		    }
		    nextchar(pRExC_state);
		  insert_if:
                    REGTAIL(pRExC_state, ret, reganode(pRExC_state, IFTHEN, 0));
                    br = regbranch(pRExC_state, &flags, 1,depth+1);
		    if (br == NULL) {
                        if (flags & (RESTART_PASS1|NEED_UTF8)) {
                            *flagp = flags & (RESTART_PASS1|NEED_UTF8);
                            return NULL;
                        }
                        FAIL2("panic: regbranch returned NULL, flags=%#" UVxf,
                              (UV) flags);
                    } else
                        REGTAIL(pRExC_state, br, reganode(pRExC_state,
                                                          LONGJMP, 0));
		    c = UCHARAT(RExC_parse);
                    nextchar(pRExC_state);
		    if (flags&HASWIDTH)
			*flagp |= HASWIDTH;
		    if (c == '|') {
		        if (is_define)
		            vFAIL("(?(DEFINE)....) does not allow branches");

                        /* Fake one for optimizer.  */
                        lastbr = reganode(pRExC_state, IFTHEN, 0);

                        if (!regbranch(pRExC_state, &flags, 1,depth+1)) {
                            if (flags & (RESTART_PASS1|NEED_UTF8)) {
                                *flagp = flags & (RESTART_PASS1|NEED_UTF8);
                                return NULL;
                            }
                            FAIL2("panic: regbranch returned NULL, flags=%#" UVxf,
                                  (UV) flags);
                        }
                        REGTAIL(pRExC_state, ret, lastbr);
		 	if (flags&HASWIDTH)
			    *flagp |= HASWIDTH;
                        c = UCHARAT(RExC_parse);
                        nextchar(pRExC_state);
		    }
		    else
			lastbr = NULL;
                    if (c != ')') {
                        if (RExC_parse >= RExC_end)
                            vFAIL("Switch (?(condition)... not terminated");
                        else
                            vFAIL("Switch (?(condition)... contains too many branches");
                    }
		    ender = reg_node(pRExC_state, TAIL);
                    REGTAIL(pRExC_state, br, ender);
		    if (lastbr) {
                        REGTAIL(pRExC_state, lastbr, ender);
                        REGTAIL(pRExC_state, NEXTOPER(NEXTOPER(lastbr)), ender);
		    }
		    else
                        REGTAIL(pRExC_state, ret, ender);
                    RExC_size++; /* XXX WHY do we need this?!!
                                    For large programs it seems to be required
                                    but I can't figure out why. -- dmq*/
		    return ret;
		}
                RExC_parse += UTF ? UTF8SKIP(RExC_parse) : 1;
                vFAIL("Unknown switch condition (?(...))");
	    }
	    case '[':           /* (?[ ... ]) */
                return handle_regex_sets(pRExC_state, NULL, flagp, depth,
                                         oregcomp_parse);
            case 0: /* A NUL */
		RExC_parse--; /* for vFAIL to print correctly */
                vFAIL("Sequence (? incomplete");
                break;
	    default: /* e.g., (?i) */
	        RExC_parse = (char *) seqstart + 1;
              parse_flags:
		parse_lparen_question_flags(pRExC_state);
                if (UCHARAT(RExC_parse) != ':') {
                    if (RExC_parse < RExC_end)
                        nextchar(pRExC_state);
                    *flagp = TRYAGAIN;
                    return NULL;
                }
                paren = ':';
                nextchar(pRExC_state);
                ret = NULL;
                goto parse_rest;
            } /* end switch */
	}
	else if (!(RExC_flags & RXf_PMf_NOCAPTURE)) {   /* (...) */
	  capturing_parens:
	    parno = RExC_npar;
	    RExC_npar++;

	    ret = reganode(pRExC_state, OPEN, parno);
	    if (!SIZE_ONLY ){
	        if (!RExC_nestroot)
	            RExC_nestroot = parno;
                if (RExC_open_parens && !RExC_open_parens[parno])
	        {
                    DEBUG_OPTIMISE_MORE_r(Perl_re_printf( aTHX_
                        "%*s%*s Setting open paren #%" IVdf " to %d\n",
                        22, "|    |", (int)(depth * 2 + 1), "",
			(IV)parno, REG_NODE_NUM(ret)));
                    RExC_open_parens[parno]= ret;
	        }
	    }
            Set_Node_Length(ret, 1); /* MJD */
            Set_Node_Offset(ret, RExC_parse); /* MJD */
	    is_open = 1;
	} else {
            /* with RXf_PMf_NOCAPTURE treat (...) as (?:...) */
            paren = ':';
	    ret = NULL;
	}
    }
    else                        /* ! paren */
	ret = NULL;

   parse_rest:
    /* Pick up the branches, linking them together. */
    parse_start = RExC_parse;   /* MJD */
    br = regbranch(pRExC_state, &flags, 1,depth+1);

    /*     branch_len = (paren != 0); */

    if (br == NULL) {
        if (flags & (RESTART_PASS1|NEED_UTF8)) {
            *flagp = flags & (RESTART_PASS1|NEED_UTF8);
            return NULL;
        }
        FAIL2("panic: regbranch returned NULL, flags=%#" UVxf, (UV) flags);
    }
    if (*RExC_parse == '|') {
	if (!SIZE_ONLY && RExC_extralen) {
	    reginsert(pRExC_state, BRANCHJ, br, depth+1);
	}
	else {                  /* MJD */
	    reginsert(pRExC_state, BRANCH, br, depth+1);
            Set_Node_Length(br, paren != 0);
            Set_Node_Offset_To_R(br-RExC_emit_start, parse_start-RExC_start);
        }
	have_branch = 1;
	if (SIZE_ONLY)
	    RExC_extralen += 1;		/* For BRANCHJ-BRANCH. */
    }
    else if (paren == ':') {
	*flagp |= flags&SIMPLE;
    }
    if (is_open) {				/* Starts with OPEN. */
        REGTAIL(pRExC_state, ret, br);          /* OPEN -> first. */
    }
    else if (paren != '?')		/* Not Conditional */
	ret = br;
    *flagp |= flags & (SPSTART | HASWIDTH | POSTPONED);
    lastbr = br;
    while (*RExC_parse == '|') {
	if (!SIZE_ONLY && RExC_extralen) {
	    ender = reganode(pRExC_state, LONGJMP,0);

            /* Append to the previous. */
            REGTAIL(pRExC_state, NEXTOPER(NEXTOPER(lastbr)), ender);
	}
	if (SIZE_ONLY)
	    RExC_extralen += 2;		/* Account for LONGJMP. */
	nextchar(pRExC_state);
	if (freeze_paren) {
	    if (RExC_npar > after_freeze)
	        after_freeze = RExC_npar;
            RExC_npar = freeze_paren;
        }
        br = regbranch(pRExC_state, &flags, 0, depth+1);

	if (br == NULL) {
            if (flags & (RESTART_PASS1|NEED_UTF8)) {
                *flagp = flags & (RESTART_PASS1|NEED_UTF8);
                return NULL;
            }
            FAIL2("panic: regbranch returned NULL, flags=%#" UVxf, (UV) flags);
        }
        REGTAIL(pRExC_state, lastbr, br);               /* BRANCH -> BRANCH. */
	lastbr = br;
	*flagp |= flags & (SPSTART | HASWIDTH | POSTPONED);
    }

    if (have_branch || paren != ':') {
	/* Make a closing node, and hook it on the end. */
	switch (paren) {
	case ':':
	    ender = reg_node(pRExC_state, TAIL);
	    break;
	case 1: case 2:
	    ender = reganode(pRExC_state, CLOSE, parno);
            if ( RExC_close_parens ) {
                DEBUG_OPTIMISE_MORE_r(Perl_re_printf( aTHX_
                        "%*s%*s Setting close paren #%" IVdf " to %d\n",
                        22, "|    |", (int)(depth * 2 + 1), "", (IV)parno, REG_NODE_NUM(ender)));
                RExC_close_parens[parno]= ender;
	        if (RExC_nestroot == parno)
	            RExC_nestroot = 0;
	    }
            Set_Node_Offset(ender,RExC_parse+1); /* MJD */
            Set_Node_Length(ender,1); /* MJD */
	    break;
	case '<':
	case ',':
	case '=':
	case '!':
	    *flagp &= ~HASWIDTH;
	    /* FALLTHROUGH */
	case '>':
	    ender = reg_node(pRExC_state, SUCCEED);
	    break;
	case 0:
	    ender = reg_node(pRExC_state, END);
	    if (!SIZE_ONLY) {
                assert(!RExC_end_op); /* there can only be one! */
                RExC_end_op = ender;
                if (RExC_close_parens) {
                    DEBUG_OPTIMISE_MORE_r(Perl_re_printf( aTHX_
                        "%*s%*s Setting close paren #0 (END) to %d\n",
                        22, "|    |", (int)(depth * 2 + 1), "", REG_NODE_NUM(ender)));

                    RExC_close_parens[0]= ender;
                }
            }
	    break;
	}
        DEBUG_PARSE_r(if (!SIZE_ONLY) {
            DEBUG_PARSE_MSG("lsbr");
            regprop(RExC_rx, RExC_mysv1, lastbr, NULL, pRExC_state);
            regprop(RExC_rx, RExC_mysv2, ender, NULL, pRExC_state);
            Perl_re_printf( aTHX_  "~ tying lastbr %s (%" IVdf ") to ender %s (%" IVdf ") offset %" IVdf "\n",
                          SvPV_nolen_const(RExC_mysv1),
                          (IV)REG_NODE_NUM(lastbr),
                          SvPV_nolen_const(RExC_mysv2),
                          (IV)REG_NODE_NUM(ender),
                          (IV)(ender - lastbr)
            );
        });
        REGTAIL(pRExC_state, lastbr, ender);

	if (have_branch && !SIZE_ONLY) {
            char is_nothing= 1;
	    if (depth==1)
                RExC_seen |= REG_TOP_LEVEL_BRANCHES_SEEN;

	    /* Hook the tails of the branches to the closing node. */
	    for (br = ret; br; br = regnext(br)) {
		const U8 op = PL_regkind[OP(br)];
		if (op == BRANCH) {
                    REGTAIL_STUDY(pRExC_state, NEXTOPER(br), ender);
                    if ( OP(NEXTOPER(br)) != NOTHING
                         || regnext(NEXTOPER(br)) != ender)
                        is_nothing= 0;
		}
		else if (op == BRANCHJ) {
                    REGTAIL_STUDY(pRExC_state, NEXTOPER(NEXTOPER(br)), ender);
                    /* for now we always disable this optimisation * /
                    if ( OP(NEXTOPER(NEXTOPER(br))) != NOTHING
                         || regnext(NEXTOPER(NEXTOPER(br))) != ender)
                    */
                        is_nothing= 0;
		}
	    }
            if (is_nothing) {
                br= PL_regkind[OP(ret)] != BRANCH ? regnext(ret) : ret;
                DEBUG_PARSE_r(if (!SIZE_ONLY) {
                    DEBUG_PARSE_MSG("NADA");
                    regprop(RExC_rx, RExC_mysv1, ret, NULL, pRExC_state);
                    regprop(RExC_rx, RExC_mysv2, ender, NULL, pRExC_state);
                    Perl_re_printf( aTHX_  "~ converting ret %s (%" IVdf ") to ender %s (%" IVdf ") offset %" IVdf "\n",
                                  SvPV_nolen_const(RExC_mysv1),
                                  (IV)REG_NODE_NUM(ret),
                                  SvPV_nolen_const(RExC_mysv2),
                                  (IV)REG_NODE_NUM(ender),
                                  (IV)(ender - ret)
                    );
                });
                OP(br)= NOTHING;
                if (OP(ender) == TAIL) {
                    NEXT_OFF(br)= 0;
                    RExC_emit= br + 1;
                } else {
                    regnode *opt;
                    for ( opt= br + 1; opt < ender ; opt++ )
                        OP(opt)= OPTIMIZED;
                    NEXT_OFF(br)= ender - br;
                }
            }
	}
    }

    {
        const char *p;
        static const char parens[] = "=!<,>";

	if (paren && (p = strchr(parens, paren))) {
	    U8 node = ((p - parens) % 2) ? UNLESSM : IFMATCH;
	    int flag = (p - parens) > 1;

	    if (paren == '>')
		node = SUSPEND, flag = 0;
	    reginsert(pRExC_state, node,ret, depth+1);
            Set_Node_Cur_Length(ret, parse_start);
	    Set_Node_Offset(ret, parse_start + 1);
	    ret->flags = flag;
            REGTAIL_STUDY(pRExC_state, ret, reg_node(pRExC_state, TAIL));
	}
    }

    /* Check for proper termination. */
    if (paren) {
        /* restore original flags, but keep (?p) and, if we've changed from /d
         * rules to /u, keep the /u */
	RExC_flags = oregflags | (RExC_flags & RXf_PMf_KEEPCOPY);
        if (DEPENDS_SEMANTICS && RExC_uni_semantics) {
            set_regex_charset(&RExC_flags, REGEX_UNICODE_CHARSET);
        }
	if (RExC_parse >= RExC_end || UCHARAT(RExC_parse) != ')') {
	    RExC_parse = oregcomp_parse;
	    vFAIL("Unmatched (");
	}
	nextchar(pRExC_state);
    }
    else if (!paren && RExC_parse < RExC_end) {
	if (*RExC_parse == ')') {
	    RExC_parse++;
	    vFAIL("Unmatched )");
	}
	else
	    FAIL("Junk on end of regexp");	/* "Can't happen". */
	NOT_REACHED; /* NOTREACHED */
    }

    if (RExC_in_lookbehind) {
	RExC_in_lookbehind--;
    }
    if (after_freeze > RExC_npar)
        RExC_npar = after_freeze;
    return(ret);
}