xmlExpParseNumber(xmlExpCtxtPtr ctxt) {
    int ret = 0;

    SKIP_BLANKS
    if (CUR == '*') {
	NEXT
	return(-1);
    }
    if ((CUR < '0') || (CUR > '9'))
        return(-1);
    while ((CUR >= '0') && (CUR <= '9')) {
        ret = ret * 10 + (CUR - '0');
	NEXT
    }
    return(ret);
}