xmlExpParseSeq(xmlExpCtxtPtr ctxt) {
    xmlExpNodePtr ret, right;

    ret = xmlExpParseOr(ctxt);
    SKIP_BLANKS
    while (CUR == '|') {
        NEXT
	right = xmlExpParseOr(ctxt);
	if (right == NULL) {
	    xmlExpFree(ctxt, ret);
	    return(NULL);
	}
	ret = xmlExpHashGetEntry(ctxt, XML_EXP_OR, ret, right, NULL, 0, 0);
	if (ret == NULL)
	    return(NULL);
    }
    return(ret);
}