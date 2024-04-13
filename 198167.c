xmlExpParseExpr(xmlExpCtxtPtr ctxt) {
    xmlExpNodePtr ret, right;

    ret = xmlExpParseSeq(ctxt);
    SKIP_BLANKS
    while (CUR == ',') {
        NEXT
	right = xmlExpParseSeq(ctxt);
	if (right == NULL) {
	    xmlExpFree(ctxt, ret);
	    return(NULL);
	}
	ret = xmlExpHashGetEntry(ctxt, XML_EXP_SEQ, ret, right, NULL, 0, 0);
	if (ret == NULL)
	    return(NULL);
    }
    return(ret);
}