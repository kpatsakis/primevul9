S_regbranch(pTHX_ RExC_state_t *pRExC_state, I32 *flagp, I32 first, U32 depth)
{
    regnode *ret;
    regnode *chain = NULL;
    regnode *latest;
    I32 flags = 0, c = 0;
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REGBRANCH;

    DEBUG_PARSE("brnc");

    if (first)
	ret = NULL;
    else {
	if (!SIZE_ONLY && RExC_extralen)
	    ret = reganode(pRExC_state, BRANCHJ,0);
	else {
	    ret = reg_node(pRExC_state, BRANCH);
            Set_Node_Length(ret, 1);
        }
    }

    if (!first && SIZE_ONLY)
	RExC_extralen += 1;			/* BRANCHJ */

    *flagp = WORST;			/* Tentatively. */

    skip_to_be_ignored_text(pRExC_state, &RExC_parse,
                            FALSE /* Don't force to /x */ );
    while (RExC_parse < RExC_end && *RExC_parse != '|' && *RExC_parse != ')') {
	flags &= ~TRYAGAIN;
        latest = regpiece(pRExC_state, &flags,depth+1);
	if (latest == NULL) {
	    if (flags & TRYAGAIN)
		continue;
            if (flags & (RESTART_PASS1|NEED_UTF8)) {
                *flagp = flags & (RESTART_PASS1|NEED_UTF8);
                return NULL;
            }
            FAIL2("panic: regpiece returned NULL, flags=%#" UVxf, (UV) flags);
	}
	else if (ret == NULL)
            ret = latest;
	*flagp |= flags&(HASWIDTH|POSTPONED);
	if (chain == NULL) 	/* First piece. */
	    *flagp |= flags&SPSTART;
	else {
	    /* FIXME adding one for every branch after the first is probably
	     * excessive now we have TRIE support. (hv) */
	    MARK_NAUGHTY(1);
            REGTAIL(pRExC_state, chain, latest);
	}
	chain = latest;
	c++;
    }
    if (chain == NULL) {	/* Loop ran zero times. */
	chain = reg_node(pRExC_state, NOTHING);
	if (ret == NULL)
	    ret = chain;
    }
    if (c == 1) {
	*flagp |= flags&SIMPLE;
    }

    return ret;
}