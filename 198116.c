xmlExpExpDeriveInt(xmlExpCtxtPtr ctxt, xmlExpNodePtr exp, xmlExpNodePtr sub) {
    xmlExpNodePtr ret, tmp, tmp2, tmp3;
    const xmlChar **tab;
    int len, i;

    /*
     * In case of equality and if the expression can only consume a finite
     * amount, then the derivation is empty
     */
    if ((exp == sub) && (exp->c_max >= 0)) {
#ifdef DEBUG_DERIV
        printf("Equal(exp, sub) and finite -> Empty\n");
#endif
        return(emptyExp);
    }
    /*
     * decompose sub sequence first
     */
    if (sub->type == XML_EXP_EMPTY) {
#ifdef DEBUG_DERIV
        printf("Empty(sub) -> Empty\n");
#endif
	exp->ref++;
        return(exp);
    }
    if (sub->type == XML_EXP_SEQ) {
#ifdef DEBUG_DERIV
        printf("Seq(sub) -> decompose\n");
#endif
        tmp = xmlExpExpDeriveInt(ctxt, exp, sub->exp_left);
	if (tmp == NULL)
	    return(NULL);
	if (tmp == forbiddenExp)
	    return(tmp);
	ret = xmlExpExpDeriveInt(ctxt, tmp, sub->exp_right);
	xmlExpFree(ctxt, tmp);
	return(ret);
    }
    if (sub->type == XML_EXP_OR) {
#ifdef DEBUG_DERIV
        printf("Or(sub) -> decompose\n");
#endif
        tmp = xmlExpExpDeriveInt(ctxt, exp, sub->exp_left);
	if (tmp == forbiddenExp)
	    return(tmp);
	if (tmp == NULL)
	    return(NULL);
	ret = xmlExpExpDeriveInt(ctxt, exp, sub->exp_right);
	if ((ret == NULL) || (ret == forbiddenExp)) {
	    xmlExpFree(ctxt, tmp);
	    return(ret);
	}
	return(xmlExpHashGetEntry(ctxt, XML_EXP_OR, tmp, ret, NULL, 0, 0));
    }
    if (!xmlExpCheckCard(exp, sub)) {
#ifdef DEBUG_DERIV
        printf("CheckCard(exp, sub) failed -> Forbid\n");
#endif
        return(forbiddenExp);
    }
    switch (exp->type) {
        case XML_EXP_EMPTY:
	    if (sub == emptyExp)
	        return(emptyExp);
#ifdef DEBUG_DERIV
	    printf("Empty(exp) -> Forbid\n");
#endif
	    return(forbiddenExp);
        case XML_EXP_FORBID:
#ifdef DEBUG_DERIV
	    printf("Forbid(exp) -> Forbid\n");
#endif
	    return(forbiddenExp);
        case XML_EXP_ATOM:
	    if (sub->type == XML_EXP_ATOM) {
	        /* TODO: handle wildcards */
	        if (exp->exp_str == sub->exp_str) {
#ifdef DEBUG_DERIV
		    printf("Atom match -> Empty\n");
#endif
		    return(emptyExp);
                }
#ifdef DEBUG_DERIV
		printf("Atom mismatch -> Forbid\n");
#endif
	        return(forbiddenExp);
	    }
	    if ((sub->type == XML_EXP_COUNT) &&
	        (sub->exp_max == 1) &&
	        (sub->exp_left->type == XML_EXP_ATOM)) {
	        /* TODO: handle wildcards */
	        if (exp->exp_str == sub->exp_left->exp_str) {
#ifdef DEBUG_DERIV
		    printf("Atom match -> Empty\n");
#endif
		    return(emptyExp);
		}
#ifdef DEBUG_DERIV
		printf("Atom mismatch -> Forbid\n");
#endif
	        return(forbiddenExp);
	    }
#ifdef DEBUG_DERIV
	    printf("Compex exp vs Atom -> Forbid\n");
#endif
	    return(forbiddenExp);
        case XML_EXP_SEQ:
	    /* try to get the sequence consumed only if possible */
	    if (xmlExpCheckCard(exp->exp_left, sub)) {
		/* See if the sequence can be consumed directly */
#ifdef DEBUG_DERIV
		printf("Seq trying left only\n");
#endif
		ret = xmlExpExpDeriveInt(ctxt, exp->exp_left, sub);
		if ((ret != forbiddenExp) && (ret != NULL)) {
#ifdef DEBUG_DERIV
		    printf("Seq trying left only worked\n");
#endif
		    /*
		     * TODO: assumption here that we are determinist
		     *       i.e. we won't get to a nillable exp left
		     *       subset which could be matched by the right
		     *       part too.
		     * e.g.: (a | b)+,(a | c) and 'a+,a'
		     */
		    exp->exp_right->ref++;
		    return(xmlExpHashGetEntry(ctxt, XML_EXP_SEQ, ret,
					      exp->exp_right, NULL, 0, 0));
		}
#ifdef DEBUG_DERIV
	    } else {
		printf("Seq: left too short\n");
#endif
	    }
	    /* Try instead to decompose */
	    if (sub->type == XML_EXP_COUNT) {
		int min, max;

#ifdef DEBUG_DERIV
		printf("Seq: sub is a count\n");
#endif
	        ret = xmlExpExpDeriveInt(ctxt, exp->exp_left, sub->exp_left);
		if (ret == NULL)
		    return(NULL);
		if (ret != forbiddenExp) {
#ifdef DEBUG_DERIV
		    printf("Seq , Count match on left\n");
#endif
		    if (sub->exp_max < 0)
		        max = -1;
	            else
		        max = sub->exp_max -1;
		    if (sub->exp_min > 0)
		        min = sub->exp_min -1;
		    else
		        min = 0;
		    exp->exp_right->ref++;
		    tmp = xmlExpHashGetEntry(ctxt, XML_EXP_SEQ, ret,
		                             exp->exp_right, NULL, 0, 0);
		    if (tmp == NULL)
		        return(NULL);

		    sub->exp_left->ref++;
		    tmp2 = xmlExpHashGetEntry(ctxt, XML_EXP_COUNT,
				      sub->exp_left, NULL, NULL, min, max);
		    if (tmp2 == NULL) {
		        xmlExpFree(ctxt, tmp);
			return(NULL);
		    }
		    ret = xmlExpExpDeriveInt(ctxt, tmp, tmp2);
		    xmlExpFree(ctxt, tmp);
		    xmlExpFree(ctxt, tmp2);
		    return(ret);
		}
	    }
	    /* we made no progress on structured operations */
	    break;
        case XML_EXP_OR:
#ifdef DEBUG_DERIV
	    printf("Or , trying both side\n");
#endif
	    ret = xmlExpExpDeriveInt(ctxt, exp->exp_left, sub);
	    if (ret == NULL)
	        return(NULL);
	    tmp = xmlExpExpDeriveInt(ctxt, exp->exp_right, sub);
	    if (tmp == NULL) {
		xmlExpFree(ctxt, ret);
	        return(NULL);
	    }
	    return(xmlExpHashGetEntry(ctxt, XML_EXP_OR, ret, tmp, NULL, 0, 0));
        case XML_EXP_COUNT: {
	    int min, max;

	    if (sub->type == XML_EXP_COUNT) {
	        /*
		 * Try to see if the loop is completely subsumed
		 */
	        tmp = xmlExpExpDeriveInt(ctxt, exp->exp_left, sub->exp_left);
		if (tmp == NULL)
		    return(NULL);
		if (tmp == forbiddenExp) {
		    int mult;

#ifdef DEBUG_DERIV
		    printf("Count, Count inner don't subsume\n");
#endif
		    mult = xmlExpDivide(ctxt, sub->exp_left, exp->exp_left,
		                        NULL, &tmp);
		    if (mult <= 0) {
#ifdef DEBUG_DERIV
			printf("Count, Count not multiple => forbidden\n");
#endif
                        return(forbiddenExp);
		    }
		    if (sub->exp_max == -1) {
		        max = -1;
			if (exp->exp_max == -1) {
			    if (exp->exp_min <= sub->exp_min * mult)
			        min = 0;
			    else
			        min = exp->exp_min - sub->exp_min * mult;
			} else {
#ifdef DEBUG_DERIV
			    printf("Count, Count finite can't subsume infinite\n");
#endif
                            xmlExpFree(ctxt, tmp);
			    return(forbiddenExp);
			}
		    } else {
			if (exp->exp_max == -1) {
#ifdef DEBUG_DERIV
			    printf("Infinite loop consume mult finite loop\n");
#endif
			    if (exp->exp_min > sub->exp_min * mult) {
				max = -1;
				min = exp->exp_min - sub->exp_min * mult;
			    } else {
				max = -1;
				min = 0;
			    }
			} else {
			    if (exp->exp_max < sub->exp_max * mult) {
#ifdef DEBUG_DERIV
				printf("loops max mult mismatch => forbidden\n");
#endif
				xmlExpFree(ctxt, tmp);
				return(forbiddenExp);
			    }
			    if (sub->exp_max * mult > exp->exp_min)
				min = 0;
			    else
				min = exp->exp_min - sub->exp_max * mult;
			    max = exp->exp_max - sub->exp_max * mult;
			}
		    }
		} else if (!IS_NILLABLE(tmp)) {
		    /*
		     * TODO: loop here to try to grow if working on finite
		     *       blocks.
		     */
#ifdef DEBUG_DERIV
		    printf("Count, Count remain not nillable => forbidden\n");
#endif
		    xmlExpFree(ctxt, tmp);
		    return(forbiddenExp);
		} else if (sub->exp_max == -1) {
		    if (exp->exp_max == -1) {
		        if (exp->exp_min <= sub->exp_min) {
#ifdef DEBUG_DERIV
			    printf("Infinite loops Okay => COUNT(0,Inf)\n");
#endif
                            max = -1;
			    min = 0;
			} else {
#ifdef DEBUG_DERIV
			    printf("Infinite loops min => Count(X,Inf)\n");
#endif
                            max = -1;
			    min = exp->exp_min - sub->exp_min;
			}
		    } else if (exp->exp_min > sub->exp_min) {
#ifdef DEBUG_DERIV
			printf("loops min mismatch 1 => forbidden ???\n");
#endif
		        xmlExpFree(ctxt, tmp);
		        return(forbiddenExp);
		    } else {
			max = -1;
			min = 0;
		    }
		} else {
		    if (exp->exp_max == -1) {
#ifdef DEBUG_DERIV
			printf("Infinite loop consume finite loop\n");
#endif
		        if (exp->exp_min > sub->exp_min) {
			    max = -1;
			    min = exp->exp_min - sub->exp_min;
			} else {
			    max = -1;
			    min = 0;
			}
		    } else {
		        if (exp->exp_max < sub->exp_max) {
#ifdef DEBUG_DERIV
			    printf("loops max mismatch => forbidden\n");
#endif
			    xmlExpFree(ctxt, tmp);
			    return(forbiddenExp);
			}
			if (sub->exp_max > exp->exp_min)
			    min = 0;
			else
			    min = exp->exp_min - sub->exp_max;
			max = exp->exp_max - sub->exp_max;
		    }
		}
#ifdef DEBUG_DERIV
		printf("loops match => SEQ(COUNT())\n");
#endif
		exp->exp_left->ref++;
		tmp2 = xmlExpHashGetEntry(ctxt, XML_EXP_COUNT, exp->exp_left,
		                          NULL, NULL, min, max);
		if (tmp2 == NULL) {
		    return(NULL);
		}
                ret = xmlExpHashGetEntry(ctxt, XML_EXP_SEQ, tmp, tmp2,
		                         NULL, 0, 0);
		return(ret);
	    }
	    tmp = xmlExpExpDeriveInt(ctxt, exp->exp_left, sub);
	    if (tmp == NULL)
		return(NULL);
	    if (tmp == forbiddenExp) {
#ifdef DEBUG_DERIV
		printf("loop mismatch => forbidden\n");
#endif
		return(forbiddenExp);
	    }
	    if (exp->exp_min > 0)
		min = exp->exp_min - 1;
	    else
		min = 0;
	    if (exp->exp_max < 0)
		max = -1;
	    else
		max = exp->exp_max - 1;

#ifdef DEBUG_DERIV
	    printf("loop match => SEQ(COUNT())\n");
#endif
	    exp->exp_left->ref++;
	    tmp2 = xmlExpHashGetEntry(ctxt, XML_EXP_COUNT, exp->exp_left,
				      NULL, NULL, min, max);
	    if (tmp2 == NULL)
		return(NULL);
	    ret = xmlExpHashGetEntry(ctxt, XML_EXP_SEQ, tmp, tmp2,
				     NULL, 0, 0);
	    return(ret);
	}
    }

#ifdef DEBUG_DERIV
    printf("Fallback to derivative\n");
#endif
    if (IS_NILLABLE(sub)) {
        if (!(IS_NILLABLE(exp)))
	    return(forbiddenExp);
	else
	    ret = emptyExp;
    } else
	ret = NULL;
    /*
     * here the structured derivation made no progress so
     * we use the default token based derivation to force one more step
     */
    if (ctxt->tabSize == 0)
        ctxt->tabSize = 40;

    tab = (const xmlChar **) xmlMalloc(ctxt->tabSize *
	                               sizeof(const xmlChar *));
    if (tab == NULL) {
	return(NULL);
    }

    /*
     * collect all the strings accepted by the subexpression on input
     */
    len = xmlExpGetStartInt(ctxt, sub, tab, ctxt->tabSize, 0);
    while (len < 0) {
        const xmlChar **temp;
	temp = (const xmlChar **) xmlRealloc((xmlChar **) tab, ctxt->tabSize * 2 *
	                                     sizeof(const xmlChar *));
	if (temp == NULL) {
	    xmlFree((xmlChar **) tab);
	    return(NULL);
	}
	tab = temp;
	ctxt->tabSize *= 2;
	len = xmlExpGetStartInt(ctxt, sub, tab, ctxt->tabSize, 0);
    }
    for (i = 0;i < len;i++) {
        tmp = xmlExpStringDeriveInt(ctxt, exp, tab[i]);
	if ((tmp == NULL) || (tmp == forbiddenExp)) {
	    xmlExpFree(ctxt, ret);
	    xmlFree((xmlChar **) tab);
	    return(tmp);
	}
	tmp2 = xmlExpStringDeriveInt(ctxt, sub, tab[i]);
	if ((tmp2 == NULL) || (tmp2 == forbiddenExp)) {
	    xmlExpFree(ctxt, tmp);
	    xmlExpFree(ctxt, ret);
	    xmlFree((xmlChar **) tab);
	    return(tmp);
	}
	tmp3 = xmlExpExpDeriveInt(ctxt, tmp, tmp2);
	xmlExpFree(ctxt, tmp);
	xmlExpFree(ctxt, tmp2);

	if ((tmp3 == NULL) || (tmp3 == forbiddenExp)) {
	    xmlExpFree(ctxt, ret);
	    xmlFree((xmlChar **) tab);
	    return(tmp3);
	}

	if (ret == NULL)
	    ret = tmp3;
	else {
	    ret = xmlExpHashGetEntry(ctxt, XML_EXP_OR, ret, tmp3, NULL, 0, 0);
	    if (ret == NULL) {
		xmlFree((xmlChar **) tab);
	        return(NULL);
	    }
	}
    }
    xmlFree((xmlChar **) tab);
    return(ret);
}