xmlExpSubsume(xmlExpCtxtPtr ctxt, xmlExpNodePtr exp, xmlExpNodePtr sub) {
    xmlExpNodePtr tmp;

    if ((exp == NULL) || (ctxt == NULL) || (sub == NULL))
        return(-1);

    /*
     * TODO: speedup by checking the language of sub is a subset of the
     *       language of exp
     */
    /*
     * O(1) speedups
     */
    if (IS_NILLABLE(sub) && (!IS_NILLABLE(exp))) {
#ifdef DEBUG_DERIV
	printf("Sub nillable and not exp : can't subsume\n");
#endif
        return(0);
    }
    if (xmlExpCheckCard(exp, sub) == 0) {
#ifdef DEBUG_DERIV
	printf("sub generate longuer sequances than exp : can't subsume\n");
#endif
        return(0);
    }
    tmp = xmlExpExpDeriveInt(ctxt, exp, sub);
#ifdef DEBUG_DERIV
    printf("Result derivation :\n");
    PRINT_EXP(tmp);
#endif
    if (tmp == NULL)
        return(-1);
    if (tmp == forbiddenExp)
	return(0);
    if (tmp == emptyExp)
	return(1);
    if ((tmp != NULL) && (IS_NILLABLE(tmp))) {
        xmlExpFree(ctxt, tmp);
        return(1);
    }
    xmlExpFree(ctxt, tmp);
    return(0);
}