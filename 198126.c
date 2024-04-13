xmlFAParseCharClass(xmlRegParserCtxtPtr ctxt) {
    if (CUR == '[') {
	NEXT;
	ctxt->atom = xmlRegNewAtom(ctxt, XML_REGEXP_RANGES);
	if (ctxt->atom == NULL)
	    return;
	xmlFAParseCharGroup(ctxt);
	if (CUR == ']') {
	    NEXT;
	} else {
	    ERROR("xmlFAParseCharClass: ']' expected");
	}
    } else {
	xmlFAParseCharClassEsc(ctxt);
    }
}