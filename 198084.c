xmlFACompareRanges(xmlRegRangePtr range1, xmlRegRangePtr range2) {
    int ret = 0;

    if ((range1->type == XML_REGEXP_RANGES) ||
        (range2->type == XML_REGEXP_RANGES) ||
        (range2->type == XML_REGEXP_SUBREG) ||
        (range1->type == XML_REGEXP_SUBREG) ||
        (range1->type == XML_REGEXP_STRING) ||
        (range2->type == XML_REGEXP_STRING))
	return(-1);

    /* put them in order */
    if (range1->type > range2->type) {
        xmlRegRangePtr tmp;

	tmp = range1;
	range1 = range2;
	range2 = tmp;
    }
    if ((range1->type == XML_REGEXP_ANYCHAR) ||
        (range2->type == XML_REGEXP_ANYCHAR)) {
	ret = 1;
    } else if ((range1->type == XML_REGEXP_EPSILON) ||
               (range2->type == XML_REGEXP_EPSILON)) {
	return(0);
    } else if (range1->type == range2->type) {
        if (range1->type != XML_REGEXP_CHARVAL)
            ret = 1;
        else if ((range1->end < range2->start) ||
	         (range2->end < range1->start))
	    ret = 0;
	else
	    ret = 1;
    } else if (range1->type == XML_REGEXP_CHARVAL) {
        int codepoint;
	int neg = 0;

	/*
	 * just check all codepoints in the range for acceptance,
	 * this is usually way cheaper since done only once at
	 * compilation than testing over and over at runtime or
	 * pushing too many states when evaluating.
	 */
	if (((range1->neg == 0) && (range2->neg != 0)) ||
	    ((range1->neg != 0) && (range2->neg == 0)))
	    neg = 1;

	for (codepoint = range1->start;codepoint <= range1->end ;codepoint++) {
	    ret = xmlRegCheckCharacterRange(range2->type, codepoint,
					    0, range2->start, range2->end,
					    range2->blockName);
	    if (ret < 0)
	        return(-1);
	    if (((neg == 1) && (ret == 0)) ||
	        ((neg == 0) && (ret == 1)))
		return(1);
	}
	return(0);
    } else if ((range1->type == XML_REGEXP_BLOCK_NAME) ||
               (range2->type == XML_REGEXP_BLOCK_NAME)) {
	if (range1->type == range2->type) {
	    ret = xmlStrEqual(range1->blockName, range2->blockName);
	} else {
	    /*
	     * comparing a block range with anything else is way
	     * too costly, and maintining the table is like too much
	     * memory too, so let's force the automata to save state
	     * here.
	     */
	    return(1);
	}
    } else if ((range1->type < XML_REGEXP_LETTER) ||
               (range2->type < XML_REGEXP_LETTER)) {
	if ((range1->type == XML_REGEXP_ANYSPACE) &&
	    (range2->type == XML_REGEXP_NOTSPACE))
	    ret = 0;
	else if ((range1->type == XML_REGEXP_INITNAME) &&
	         (range2->type == XML_REGEXP_NOTINITNAME))
	    ret = 0;
	else if ((range1->type == XML_REGEXP_NAMECHAR) &&
	         (range2->type == XML_REGEXP_NOTNAMECHAR))
	    ret = 0;
	else if ((range1->type == XML_REGEXP_DECIMAL) &&
	         (range2->type == XML_REGEXP_NOTDECIMAL))
	    ret = 0;
	else if ((range1->type == XML_REGEXP_REALCHAR) &&
	         (range2->type == XML_REGEXP_NOTREALCHAR))
	    ret = 0;
	else {
	    /* same thing to limit complexity */
	    return(1);
	}
    } else {
        ret = 0;
        /* range1->type < range2->type here */
        switch (range1->type) {
	    case XML_REGEXP_LETTER:
	         /* all disjoint except in the subgroups */
	         if ((range2->type == XML_REGEXP_LETTER_UPPERCASE) ||
		     (range2->type == XML_REGEXP_LETTER_LOWERCASE) ||
		     (range2->type == XML_REGEXP_LETTER_TITLECASE) ||
		     (range2->type == XML_REGEXP_LETTER_MODIFIER) ||
		     (range2->type == XML_REGEXP_LETTER_OTHERS))
		     ret = 1;
		 break;
	    case XML_REGEXP_MARK:
	         if ((range2->type == XML_REGEXP_MARK_NONSPACING) ||
		     (range2->type == XML_REGEXP_MARK_SPACECOMBINING) ||
		     (range2->type == XML_REGEXP_MARK_ENCLOSING))
		     ret = 1;
		 break;
	    case XML_REGEXP_NUMBER:
	         if ((range2->type == XML_REGEXP_NUMBER_DECIMAL) ||
		     (range2->type == XML_REGEXP_NUMBER_LETTER) ||
		     (range2->type == XML_REGEXP_NUMBER_OTHERS))
		     ret = 1;
		 break;
	    case XML_REGEXP_PUNCT:
	         if ((range2->type == XML_REGEXP_PUNCT_CONNECTOR) ||
		     (range2->type == XML_REGEXP_PUNCT_DASH) ||
		     (range2->type == XML_REGEXP_PUNCT_OPEN) ||
		     (range2->type == XML_REGEXP_PUNCT_CLOSE) ||
		     (range2->type == XML_REGEXP_PUNCT_INITQUOTE) ||
		     (range2->type == XML_REGEXP_PUNCT_FINQUOTE) ||
		     (range2->type == XML_REGEXP_PUNCT_OTHERS))
		     ret = 1;
		 break;
	    case XML_REGEXP_SEPAR:
	         if ((range2->type == XML_REGEXP_SEPAR_SPACE) ||
		     (range2->type == XML_REGEXP_SEPAR_LINE) ||
		     (range2->type == XML_REGEXP_SEPAR_PARA))
		     ret = 1;
		 break;
	    case XML_REGEXP_SYMBOL:
	         if ((range2->type == XML_REGEXP_SYMBOL_MATH) ||
		     (range2->type == XML_REGEXP_SYMBOL_CURRENCY) ||
		     (range2->type == XML_REGEXP_SYMBOL_MODIFIER) ||
		     (range2->type == XML_REGEXP_SYMBOL_OTHERS))
		     ret = 1;
		 break;
	    case XML_REGEXP_OTHER:
	         if ((range2->type == XML_REGEXP_OTHER_CONTROL) ||
		     (range2->type == XML_REGEXP_OTHER_FORMAT) ||
		     (range2->type == XML_REGEXP_OTHER_PRIVATE))
		     ret = 1;
		 break;
            default:
	         if ((range2->type >= XML_REGEXP_LETTER) &&
		     (range2->type < XML_REGEXP_BLOCK_NAME))
		     ret = 0;
		 else {
		     /* safety net ! */
		     return(1);
		 }
	}
    }
    if (((range1->neg == 0) && (range2->neg != 0)) ||
        ((range1->neg != 0) && (range2->neg == 0)))
	ret = !ret;
    return(ret);
}