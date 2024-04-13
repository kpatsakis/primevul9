xmlFAParseQuantifier(xmlRegParserCtxtPtr ctxt) {
    int cur;

    cur = CUR;
    if ((cur == '?') || (cur == '*') || (cur == '+')) {
	if (ctxt->atom != NULL) {
	    if (cur == '?')
		ctxt->atom->quant = XML_REGEXP_QUANT_OPT;
	    else if (cur == '*')
		ctxt->atom->quant = XML_REGEXP_QUANT_MULT;
	    else if (cur == '+')
		ctxt->atom->quant = XML_REGEXP_QUANT_PLUS;
	}
	NEXT;
	return(1);
    }
    if (cur == '{') {
	int min = 0, max = 0;

	NEXT;
	cur = xmlFAParseQuantExact(ctxt);
	if (cur >= 0)
	    min = cur;
	if (CUR == ',') {
	    NEXT;
	    if (CUR == '}')
	        max = INT_MAX;
	    else {
	        cur = xmlFAParseQuantExact(ctxt);
	        if (cur >= 0)
		    max = cur;
		else {
		    ERROR("Improper quantifier");
		}
	    }
	}
	if (CUR == '}') {
	    NEXT;
	} else {
	    ERROR("Unterminated quantifier");
	}
	if (max == 0)
	    max = min;
	if (ctxt->atom != NULL) {
	    ctxt->atom->quant = XML_REGEXP_QUANT_RANGE;
	    ctxt->atom->min = min;
	    ctxt->atom->max = max;
	}
	return(1);
    }
    return(0);
}