xmlExpExpDerive(xmlExpCtxtPtr ctxt, xmlExpNodePtr exp, xmlExpNodePtr sub) {
    if ((exp == NULL) || (ctxt == NULL) || (sub == NULL))
        return(NULL);

    /*
     * O(1) speedups
     */
    if (IS_NILLABLE(sub) && (!IS_NILLABLE(exp))) {
#ifdef DEBUG_DERIV
	printf("Sub nillable and not exp : can't subsume\n");
#endif
        return(forbiddenExp);
    }
    if (xmlExpCheckCard(exp, sub) == 0) {
#ifdef DEBUG_DERIV
	printf("sub generate longuer sequances than exp : can't subsume\n");
#endif
        return(forbiddenExp);
    }
    return(xmlExpExpDeriveInt(ctxt, exp, sub));
}