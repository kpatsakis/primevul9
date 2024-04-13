xmlExpParseOr(xmlExpCtxtPtr ctxt) {
    const char *base;
    xmlExpNodePtr ret;
    const xmlChar *val;

    SKIP_BLANKS
    base = ctxt->cur;
    if (*ctxt->cur == '(') {
        NEXT
	ret = xmlExpParseExpr(ctxt);
	SKIP_BLANKS
	if (*ctxt->cur != ')') {
	    fprintf(stderr, "unbalanced '(' : %s\n", base);
	    xmlExpFree(ctxt, ret);
	    return(NULL);
	}
	NEXT;
	SKIP_BLANKS
	goto parse_quantifier;
    }
    while ((CUR != 0) && (!(IS_BLANK(CUR))) && (CUR != '(') &&
           (CUR != ')') && (CUR != '|') && (CUR != ',') && (CUR != '{') &&
	   (CUR != '*') && (CUR != '+') && (CUR != '?') && (CUR != '}'))
	NEXT;
    val = xmlDictLookup(ctxt->dict, BAD_CAST base, ctxt->cur - base);
    if (val == NULL)
        return(NULL);
    ret = xmlExpHashGetEntry(ctxt, XML_EXP_ATOM, NULL, NULL, val, 0, 0);
    if (ret == NULL)
        return(NULL);
    SKIP_BLANKS
parse_quantifier:
    if (CUR == '{') {
        int min, max;

        NEXT
	min = xmlExpParseNumber(ctxt);
	if (min < 0) {
	    xmlExpFree(ctxt, ret);
	    return(NULL);
	}
	SKIP_BLANKS
	if (CUR == ',') {
	    NEXT
	    max = xmlExpParseNumber(ctxt);
	    SKIP_BLANKS
	} else
	    max = min;
	if (CUR != '}') {
	    xmlExpFree(ctxt, ret);
	    return(NULL);
	}
        NEXT
	ret = xmlExpHashGetEntry(ctxt, XML_EXP_COUNT, ret, NULL, NULL,
	                         min, max);
	SKIP_BLANKS
    } else if (CUR == '?') {
        NEXT
	ret = xmlExpHashGetEntry(ctxt, XML_EXP_COUNT, ret, NULL, NULL,
	                         0, 1);
	SKIP_BLANKS
    } else if (CUR == '+') {
        NEXT
	ret = xmlExpHashGetEntry(ctxt, XML_EXP_COUNT, ret, NULL, NULL,
	                         1, -1);
	SKIP_BLANKS
    } else if (CUR == '*') {
        NEXT
	ret = xmlExpHashGetEntry(ctxt, XML_EXP_COUNT, ret, NULL, NULL,
	                         0, -1);
	SKIP_BLANKS
    }
    return(ret);
}