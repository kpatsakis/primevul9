htmlParseSystemLiteral(htmlParserCtxtPtr ctxt) {
    size_t len = 0, startPosition = 0;
    xmlChar *ret = NULL;

    if (CUR == '"') {
        NEXT;

        if (CUR_PTR < BASE_PTR)
            return(ret);
        startPosition = CUR_PTR - BASE_PTR;

	while ((IS_CHAR_CH(CUR)) && (CUR != '"')) {
	    NEXT;
	    len++;
	}
	if (!IS_CHAR_CH(CUR)) {
	    htmlParseErr(ctxt, XML_ERR_LITERAL_NOT_FINISHED,
			 "Unfinished SystemLiteral\n", NULL, NULL);
	} else {
	    ret = xmlStrndup((BASE_PTR+startPosition), len);
	    NEXT;
        }
    } else if (CUR == '\'') {
        NEXT;

        if (CUR_PTR < BASE_PTR)
            return(ret);
        startPosition = CUR_PTR - BASE_PTR;

	while ((IS_CHAR_CH(CUR)) && (CUR != '\'')) {
	    NEXT;
	    len++;
	}
	if (!IS_CHAR_CH(CUR)) {
	    htmlParseErr(ctxt, XML_ERR_LITERAL_NOT_FINISHED,
			 "Unfinished SystemLiteral\n", NULL, NULL);
	} else {
	    ret = xmlStrndup((BASE_PTR+startPosition), len);
	    NEXT;
        }
    } else {
	htmlParseErr(ctxt, XML_ERR_LITERAL_NOT_STARTED,
	             " or ' expected\n", NULL, NULL);
    }

    return(ret);
}