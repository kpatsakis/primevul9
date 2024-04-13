xsltScanNCName(xsltParserContextPtr ctxt) {
    const xmlChar *q, *cur;
    xmlChar *ret = NULL;
    int val, len;

    SKIP_BLANKS;

    cur = q = CUR_PTR;
    val = xmlStringCurrentChar(NULL, cur, &len);
    if (!IS_LETTER(val) && (val != '_'))
	return(NULL);

    while ((IS_LETTER(val)) || (IS_DIGIT(val)) ||
           (val == '.') || (val == '-') ||
	   (val == '_') ||
	   (IS_COMBINING(val)) ||
	   (IS_EXTENDER(val))) {
	cur += len;
	val = xmlStringCurrentChar(NULL, cur, &len);
    }
    ret = xmlStrndup(q, cur - q);
    CUR_PTR = cur;
    return(ret);
}