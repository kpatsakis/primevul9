xmlFAIsChar(xmlRegParserCtxtPtr ctxt) {
    int cur;
    int len;

    cur = CUR_SCHAR(ctxt->cur, len);
    if ((cur == '.') || (cur == '\\') || (cur == '?') ||
	(cur == '*') || (cur == '+') || (cur == '(') ||
	(cur == ')') || (cur == '|') || (cur == 0x5B) ||
	(cur == 0x5D) || (cur == 0))
	return(-1);
    return(cur);
}