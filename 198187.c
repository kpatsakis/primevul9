xmlFAParsePosCharGroup(xmlRegParserCtxtPtr ctxt) {
    do {
	if (CUR == '\\') {
	    xmlFAParseCharClassEsc(ctxt);
	} else {
	    xmlFAParseCharRange(ctxt);
	}
    } while ((CUR != ']') && (CUR != '^') && (CUR != '-') &&
             (CUR != 0) && (ctxt->error == 0));
}