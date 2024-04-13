xsltNewCompMatch(void) {
    xsltCompMatchPtr cur;

    cur = (xsltCompMatchPtr) xmlMalloc(sizeof(xsltCompMatch));
    if (cur == NULL) {
	xsltTransformError(NULL, NULL, NULL,
		"xsltNewCompMatch : out of memory error\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xsltCompMatch));
    cur->maxStep = 10;
    cur->nbStep = 0;
    cur-> steps = (xsltStepOpPtr) xmlMalloc(sizeof(xsltStepOp) *
                                            cur->maxStep);
    if (cur->steps == NULL) {
	xsltTransformError(NULL, NULL, NULL,
		"xsltNewCompMatch : out of memory error\n");
	xmlFree(cur);
	return(NULL);
    }
    cur->nsNr = 0;
    cur->nsList = NULL;
    cur->direct = 0;
    return(cur);
}