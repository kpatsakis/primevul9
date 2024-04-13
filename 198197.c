xmlExpStringDeriveInt(xmlExpCtxtPtr ctxt, xmlExpNodePtr exp, const xmlChar *str)
{
    xmlExpNodePtr ret;

    switch (exp->type) {
	case XML_EXP_EMPTY:
	    return(forbiddenExp);
	case XML_EXP_FORBID:
	    return(forbiddenExp);
	case XML_EXP_ATOM:
	    if (exp->exp_str == str) {
#ifdef DEBUG_DERIV
		printf("deriv atom: equal => Empty\n");
#endif
	        ret = emptyExp;
	    } else {
#ifdef DEBUG_DERIV
		printf("deriv atom: mismatch => forbid\n");
#endif
	        /* TODO wildcards here */
		ret = forbiddenExp;
	    }
	    return(ret);
	case XML_EXP_OR: {
	    xmlExpNodePtr tmp;

#ifdef DEBUG_DERIV
	    printf("deriv or: => or(derivs)\n");
#endif
	    tmp = xmlExpStringDeriveInt(ctxt, exp->exp_left, str);
	    if (tmp == NULL) {
		return(NULL);
	    }
	    ret = xmlExpStringDeriveInt(ctxt, exp->exp_right, str);
	    if (ret == NULL) {
	        xmlExpFree(ctxt, tmp);
		return(NULL);
	    }
            ret = xmlExpHashGetEntry(ctxt, XML_EXP_OR, tmp, ret,
			     NULL, 0, 0);
	    return(ret);
	}
	case XML_EXP_SEQ:
#ifdef DEBUG_DERIV
	    printf("deriv seq: starting with left\n");
#endif
	    ret = xmlExpStringDeriveInt(ctxt, exp->exp_left, str);
	    if (ret == NULL) {
	        return(NULL);
	    } else if (ret == forbiddenExp) {
	        if (IS_NILLABLE(exp->exp_left)) {
#ifdef DEBUG_DERIV
		    printf("deriv seq: left failed but nillable\n");
#endif
		    ret = xmlExpStringDeriveInt(ctxt, exp->exp_right, str);
		}
	    } else {
#ifdef DEBUG_DERIV
		printf("deriv seq: left match => sequence\n");
#endif
	        exp->exp_right->ref++;
	        ret = xmlExpHashGetEntry(ctxt, XML_EXP_SEQ, ret, exp->exp_right,
		                         NULL, 0, 0);
	    }
	    return(ret);
	case XML_EXP_COUNT: {
	    int min, max;
	    xmlExpNodePtr tmp;

	    if (exp->exp_max == 0)
		return(forbiddenExp);
	    ret = xmlExpStringDeriveInt(ctxt, exp->exp_left, str);
	    if (ret == NULL)
	        return(NULL);
	    if (ret == forbiddenExp) {
#ifdef DEBUG_DERIV
		printf("deriv count: pattern mismatch => forbid\n");
#endif
	        return(ret);
	    }
	    if (exp->exp_max == 1)
		return(ret);
	    if (exp->exp_max < 0) /* unbounded */
		max = -1;
	    else
		max = exp->exp_max - 1;
	    if (exp->exp_min > 0)
		min = exp->exp_min - 1;
	    else
		min = 0;
	    exp->exp_left->ref++;
	    tmp = xmlExpHashGetEntry(ctxt, XML_EXP_COUNT, exp->exp_left, NULL,
				     NULL, min, max);
	    if (ret == emptyExp) {
#ifdef DEBUG_DERIV
		printf("deriv count: match to empty => new count\n");
#endif
	        return(tmp);
	    }
#ifdef DEBUG_DERIV
	    printf("deriv count: match => sequence with new count\n");
#endif
	    return(xmlExpHashGetEntry(ctxt, XML_EXP_SEQ, ret, tmp,
	                              NULL, 0, 0));
	}
    }
    return(NULL);
}