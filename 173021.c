xmlParsePITarget(xmlParserCtxtPtr ctxt) {
    const xmlChar *name;

    name = xmlParseName(ctxt);
    if ((name != NULL) &&
        ((name[0] == 'x') || (name[0] == 'X')) &&
        ((name[1] == 'm') || (name[1] == 'M')) &&
        ((name[2] == 'l') || (name[2] == 'L'))) {
	int i;
	if ((name[0] == 'x') && (name[1] == 'm') &&
	    (name[2] == 'l') && (name[3] == 0)) {
	    xmlFatalErrMsg(ctxt, XML_ERR_RESERVED_XML_NAME,
		 "XML declaration allowed only at the start of the document\n");
	    return(name);
	} else if (name[3] == 0) {
	    xmlFatalErr(ctxt, XML_ERR_RESERVED_XML_NAME, NULL);
	    return(name);
	}
	for (i = 0;;i++) {
	    if (xmlW3CPIs[i] == NULL) break;
	    if (xmlStrEqual(name, (const xmlChar *)xmlW3CPIs[i]))
	        return(name);
	}
	xmlWarningMsg(ctxt, XML_ERR_RESERVED_XML_NAME,
		      "xmlParsePITarget: invalid name prefix 'xml'\n",
		      NULL, NULL);
    }
    if ((name != NULL) && (xmlStrchr(name, ':') != NULL)) {
	xmlNsErr(ctxt, XML_NS_ERR_COLON,
		 "colon are forbidden from PI names '%s'\n", name, NULL, NULL);
    }
    return(name);
}