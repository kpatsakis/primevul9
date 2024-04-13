xsltParseStylesheetPI(const xmlChar *value) {
    const xmlChar *cur;
    const xmlChar *start;
    xmlChar *val;
    xmlChar tmp;
    xmlChar *href = NULL;
    int isXml = 0;

    if (value == NULL)
	return(NULL);

    cur = value;
    while (CUR != 0) {
	SKIP_BLANKS;
	if ((CUR == 't') && (NXT(1) == 'y') && (NXT(2) == 'p') &&
	    (NXT(3) == 'e')) {
	    SKIP(4);
	    SKIP_BLANKS;
	    if (CUR != '=')
		continue;
	    NEXT;
	    if ((CUR != '\'') && (CUR != '"'))
		continue;
	    tmp = CUR;
	    NEXT;
	    start = cur;
	    while ((CUR != 0) && (CUR != tmp))
		NEXT;
	    if (CUR != tmp)
		continue;
	    val = xmlStrndup(start, cur - start);
	    NEXT;
	    if (val == NULL) 
		return(NULL);
	    if ((xmlStrcasecmp(val, BAD_CAST "text/xml")) &&
		(xmlStrcasecmp(val, BAD_CAST "text/xsl"))) {
                xmlFree(val);
		break;
	    }
	    isXml = 1;
	    xmlFree(val);
	} else if ((CUR == 'h') && (NXT(1) == 'r') && (NXT(2) == 'e') &&
	    (NXT(3) == 'f')) {
	    SKIP(4);
	    SKIP_BLANKS;
	    if (CUR != '=')
		continue;
	    NEXT;
	    if ((CUR != '\'') && (CUR != '"'))
		continue;
	    tmp = CUR;
	    NEXT;
	    start = cur;
	    while ((CUR != 0) && (CUR != tmp))
		NEXT;
	    if (CUR != tmp)
		continue;
	    if (href == NULL)
		href = xmlStrndup(start, cur - start);
	    NEXT;
	} else {
	    while ((CUR != 0) && (!IS_BLANK(CUR)))
		NEXT;
	}
            
    }

    if (!isXml) {
	if (href != NULL)
	    xmlFree(href);
	href = NULL;
    }
    return(href);
}