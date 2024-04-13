xmlFAParseCharProp(xmlRegParserCtxtPtr ctxt) {
    int cur;
    xmlRegAtomType type = (xmlRegAtomType) 0;
    xmlChar *blockName = NULL;

    cur = CUR;
    if (cur == 'L') {
	NEXT;
	cur = CUR;
	if (cur == 'u') {
	    NEXT;
	    type = XML_REGEXP_LETTER_UPPERCASE;
	} else if (cur == 'l') {
	    NEXT;
	    type = XML_REGEXP_LETTER_LOWERCASE;
	} else if (cur == 't') {
	    NEXT;
	    type = XML_REGEXP_LETTER_TITLECASE;
	} else if (cur == 'm') {
	    NEXT;
	    type = XML_REGEXP_LETTER_MODIFIER;
	} else if (cur == 'o') {
	    NEXT;
	    type = XML_REGEXP_LETTER_OTHERS;
	} else {
	    type = XML_REGEXP_LETTER;
	}
    } else if (cur == 'M') {
	NEXT;
	cur = CUR;
	if (cur == 'n') {
	    NEXT;
	    /* nonspacing */
	    type = XML_REGEXP_MARK_NONSPACING;
	} else if (cur == 'c') {
	    NEXT;
	    /* spacing combining */
	    type = XML_REGEXP_MARK_SPACECOMBINING;
	} else if (cur == 'e') {
	    NEXT;
	    /* enclosing */
	    type = XML_REGEXP_MARK_ENCLOSING;
	} else {
	    /* all marks */
	    type = XML_REGEXP_MARK;
	}
    } else if (cur == 'N') {
	NEXT;
	cur = CUR;
	if (cur == 'd') {
	    NEXT;
	    /* digital */
	    type = XML_REGEXP_NUMBER_DECIMAL;
	} else if (cur == 'l') {
	    NEXT;
	    /* letter */
	    type = XML_REGEXP_NUMBER_LETTER;
	} else if (cur == 'o') {
	    NEXT;
	    /* other */
	    type = XML_REGEXP_NUMBER_OTHERS;
	} else {
	    /* all numbers */
	    type = XML_REGEXP_NUMBER;
	}
    } else if (cur == 'P') {
	NEXT;
	cur = CUR;
	if (cur == 'c') {
	    NEXT;
	    /* connector */
	    type = XML_REGEXP_PUNCT_CONNECTOR;
	} else if (cur == 'd') {
	    NEXT;
	    /* dash */
	    type = XML_REGEXP_PUNCT_DASH;
	} else if (cur == 's') {
	    NEXT;
	    /* open */
	    type = XML_REGEXP_PUNCT_OPEN;
	} else if (cur == 'e') {
	    NEXT;
	    /* close */
	    type = XML_REGEXP_PUNCT_CLOSE;
	} else if (cur == 'i') {
	    NEXT;
	    /* initial quote */
	    type = XML_REGEXP_PUNCT_INITQUOTE;
	} else if (cur == 'f') {
	    NEXT;
	    /* final quote */
	    type = XML_REGEXP_PUNCT_FINQUOTE;
	} else if (cur == 'o') {
	    NEXT;
	    /* other */
	    type = XML_REGEXP_PUNCT_OTHERS;
	} else {
	    /* all punctuation */
	    type = XML_REGEXP_PUNCT;
	}
    } else if (cur == 'Z') {
	NEXT;
	cur = CUR;
	if (cur == 's') {
	    NEXT;
	    /* space */
	    type = XML_REGEXP_SEPAR_SPACE;
	} else if (cur == 'l') {
	    NEXT;
	    /* line */
	    type = XML_REGEXP_SEPAR_LINE;
	} else if (cur == 'p') {
	    NEXT;
	    /* paragraph */
	    type = XML_REGEXP_SEPAR_PARA;
	} else {
	    /* all separators */
	    type = XML_REGEXP_SEPAR;
	}
    } else if (cur == 'S') {
	NEXT;
	cur = CUR;
	if (cur == 'm') {
	    NEXT;
	    type = XML_REGEXP_SYMBOL_MATH;
	    /* math */
	} else if (cur == 'c') {
	    NEXT;
	    type = XML_REGEXP_SYMBOL_CURRENCY;
	    /* currency */
	} else if (cur == 'k') {
	    NEXT;
	    type = XML_REGEXP_SYMBOL_MODIFIER;
	    /* modifiers */
	} else if (cur == 'o') {
	    NEXT;
	    type = XML_REGEXP_SYMBOL_OTHERS;
	    /* other */
	} else {
	    /* all symbols */
	    type = XML_REGEXP_SYMBOL;
	}
    } else if (cur == 'C') {
	NEXT;
	cur = CUR;
	if (cur == 'c') {
	    NEXT;
	    /* control */
	    type = XML_REGEXP_OTHER_CONTROL;
	} else if (cur == 'f') {
	    NEXT;
	    /* format */
	    type = XML_REGEXP_OTHER_FORMAT;
	} else if (cur == 'o') {
	    NEXT;
	    /* private use */
	    type = XML_REGEXP_OTHER_PRIVATE;
	} else if (cur == 'n') {
	    NEXT;
	    /* not assigned */
	    type = XML_REGEXP_OTHER_NA;
	} else {
	    /* all others */
	    type = XML_REGEXP_OTHER;
	}
    } else if (cur == 'I') {
	const xmlChar *start;
	NEXT;
	cur = CUR;
	if (cur != 's') {
	    ERROR("IsXXXX expected");
	    return;
	}
	NEXT;
	start = ctxt->cur;
	cur = CUR;
	if (((cur >= 'a') && (cur <= 'z')) ||
	    ((cur >= 'A') && (cur <= 'Z')) ||
	    ((cur >= '0') && (cur <= '9')) ||
	    (cur == 0x2D)) {
	    NEXT;
	    cur = CUR;
	    while (((cur >= 'a') && (cur <= 'z')) ||
		((cur >= 'A') && (cur <= 'Z')) ||
		((cur >= '0') && (cur <= '9')) ||
		(cur == 0x2D)) {
		NEXT;
		cur = CUR;
	    }
	}
	type = XML_REGEXP_BLOCK_NAME;
	blockName = xmlStrndup(start, ctxt->cur - start);
    } else {
	ERROR("Unknown char property");
	return;
    }
    if (ctxt->atom == NULL) {
	ctxt->atom = xmlRegNewAtom(ctxt, type);
	if (ctxt->atom != NULL)
	    ctxt->atom->valuep = blockName;
    } else if (ctxt->atom->type == XML_REGEXP_RANGES) {
        xmlRegAtomAddRange(ctxt, ctxt->atom, ctxt->neg,
		           type, 0, 0, blockName);
    }
}