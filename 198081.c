xmlFAParseCharClassEsc(xmlRegParserCtxtPtr ctxt) {
    int cur;

    if (CUR == '.') {
	if (ctxt->atom == NULL) {
	    ctxt->atom = xmlRegNewAtom(ctxt, XML_REGEXP_ANYCHAR);
	} else if (ctxt->atom->type == XML_REGEXP_RANGES) {
	    xmlRegAtomAddRange(ctxt, ctxt->atom, ctxt->neg,
			       XML_REGEXP_ANYCHAR, 0, 0, NULL);
	}
	NEXT;
	return;
    }
    if (CUR != '\\') {
	ERROR("Escaped sequence: expecting \\");
	return;
    }
    NEXT;
    cur = CUR;
    if (cur == 'p') {
	NEXT;
	if (CUR != '{') {
	    ERROR("Expecting '{'");
	    return;
	}
	NEXT;
	xmlFAParseCharProp(ctxt);
	if (CUR != '}') {
	    ERROR("Expecting '}'");
	    return;
	}
	NEXT;
    } else if (cur == 'P') {
	NEXT;
	if (CUR != '{') {
	    ERROR("Expecting '{'");
	    return;
	}
	NEXT;
	xmlFAParseCharProp(ctxt);
	ctxt->atom->neg = 1;
	if (CUR != '}') {
	    ERROR("Expecting '}'");
	    return;
	}
	NEXT;
    } else if ((cur == 'n') || (cur == 'r') || (cur == 't') || (cur == '\\') ||
	(cur == '|') || (cur == '.') || (cur == '?') || (cur == '*') ||
	(cur == '+') || (cur == '(') || (cur == ')') || (cur == '{') ||
	(cur == '}') || (cur == 0x2D) || (cur == 0x5B) || (cur == 0x5D) ||
	(cur == 0x5E)) {
	if (ctxt->atom == NULL) {
	    ctxt->atom = xmlRegNewAtom(ctxt, XML_REGEXP_CHARVAL);
	    if (ctxt->atom != NULL) {
	        switch (cur) {
		    case 'n':
		        ctxt->atom->codepoint = '\n';
			break;
		    case 'r':
		        ctxt->atom->codepoint = '\r';
			break;
		    case 't':
		        ctxt->atom->codepoint = '\t';
			break;
		    default:
			ctxt->atom->codepoint = cur;
		}
	    }
	} else if (ctxt->atom->type == XML_REGEXP_RANGES) {
            switch (cur) {
                case 'n':
                    cur = '\n';
                    break;
                case 'r':
                    cur = '\r';
                    break;
                case 't':
                    cur = '\t';
                    break;
            }
	    xmlRegAtomAddRange(ctxt, ctxt->atom, ctxt->neg,
			       XML_REGEXP_CHARVAL, cur, cur, NULL);
	}
	NEXT;
    } else if ((cur == 's') || (cur == 'S') || (cur == 'i') || (cur == 'I') ||
	(cur == 'c') || (cur == 'C') || (cur == 'd') || (cur == 'D') ||
	(cur == 'w') || (cur == 'W')) {
	xmlRegAtomType type = XML_REGEXP_ANYSPACE;

	switch (cur) {
	    case 's':
		type = XML_REGEXP_ANYSPACE;
		break;
	    case 'S':
		type = XML_REGEXP_NOTSPACE;
		break;
	    case 'i':
		type = XML_REGEXP_INITNAME;
		break;
	    case 'I':
		type = XML_REGEXP_NOTINITNAME;
		break;
	    case 'c':
		type = XML_REGEXP_NAMECHAR;
		break;
	    case 'C':
		type = XML_REGEXP_NOTNAMECHAR;
		break;
	    case 'd':
		type = XML_REGEXP_DECIMAL;
		break;
	    case 'D':
		type = XML_REGEXP_NOTDECIMAL;
		break;
	    case 'w':
		type = XML_REGEXP_REALCHAR;
		break;
	    case 'W':
		type = XML_REGEXP_NOTREALCHAR;
		break;
	}
	NEXT;
	if (ctxt->atom == NULL) {
	    ctxt->atom = xmlRegNewAtom(ctxt, type);
	} else if (ctxt->atom->type == XML_REGEXP_RANGES) {
	    xmlRegAtomAddRange(ctxt, ctxt->atom, ctxt->neg,
			       type, 0, 0, NULL);
	}
    } else {
	ERROR("Wrong escape sequence, misuse of character '\\'");
    }
}