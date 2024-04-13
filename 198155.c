xmlExpDivide(xmlExpCtxtPtr ctxt, xmlExpNodePtr exp, xmlExpNodePtr sub,
             xmlExpNodePtr *mult, xmlExpNodePtr *remain) {
    int i;
    xmlExpNodePtr tmp, tmp2;

    if (mult != NULL) *mult = NULL;
    if (remain != NULL) *remain = NULL;
    if (exp->c_max == -1) return(0);
    if (IS_NILLABLE(exp) && (!IS_NILLABLE(sub))) return(0);

    for (i = 1;i <= exp->c_max;i++) {
        sub->ref++;
        tmp = xmlExpHashGetEntry(ctxt, XML_EXP_COUNT,
				 sub, NULL, NULL, i, i);
	if (tmp == NULL) {
	    return(-1);
	}
	if (!xmlExpCheckCard(tmp, exp)) {
	    xmlExpFree(ctxt, tmp);
	    continue;
	}
	tmp2 = xmlExpExpDeriveInt(ctxt, tmp, exp);
	if (tmp2 == NULL) {
	    xmlExpFree(ctxt, tmp);
	    return(-1);
	}
	if ((tmp2 != forbiddenExp) && (IS_NILLABLE(tmp2))) {
	    if (remain != NULL)
	        *remain = tmp2;
	    else
	        xmlExpFree(ctxt, tmp2);
	    if (mult != NULL)
	        *mult = tmp;
	    else
	        xmlExpFree(ctxt, tmp);
#ifdef DEBUG_DERIV
	    printf("Divide succeeded %d\n", i);
#endif
	    return(i);
	}
	xmlExpFree(ctxt, tmp);
	xmlExpFree(ctxt, tmp2);
    }
#ifdef DEBUG_DERIV
    printf("Divide failed\n");
#endif
    return(0);
}