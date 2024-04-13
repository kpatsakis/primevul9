xsltScanLiteral(xsltParserContextPtr ctxt) {
    const xmlChar *q, *cur;
    xmlChar *ret = NULL;
    int val, len;

    SKIP_BLANKS;
    if (CUR == '"') {
        NEXT;
	cur = q = CUR_PTR;
	val = xmlStringCurrentChar(NULL, cur, &len);
	while ((IS_CHAR(val)) && (val != '"')) {
	    cur += len;
	    val = xmlStringCurrentChar(NULL, cur, &len);
	}
	if (!IS_CHAR(val)) {
	    ctxt->error = 1;
	    return(NULL);
	} else {
	    ret = xmlStrndup(q, cur - q);
        }
	cur += len;
	CUR_PTR = cur;
    } else if (CUR == '\'') {
        NEXT;
	cur = q = CUR_PTR;
	val = xmlStringCurrentChar(NULL, cur, &len);
	while ((IS_CHAR(val)) && (val != '\'')) {
	    cur += len;
	    val = xmlStringCurrentChar(NULL, cur, &len);
	}
	if (!IS_CHAR(val)) {
	    ctxt->error = 1;
	    return(NULL);
	} else {
	    ret = xmlStrndup(q, cur - q);
        }
	cur += len;
	CUR_PTR = cur;
    } else {
	/* XP_ERROR(XPATH_START_LITERAL_ERROR); */
	ctxt->error = 1;
	return(NULL);
    }
    return(ret);
}