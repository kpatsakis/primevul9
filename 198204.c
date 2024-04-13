xmlFAParsePiece(xmlRegParserCtxtPtr ctxt) {
    int ret;

    ctxt->atom = NULL;
    ret = xmlFAParseAtom(ctxt);
    if (ret == 0)
	return(0);
    if (ctxt->atom == NULL) {
	ERROR("internal: no atom generated");
    }
    xmlFAParseQuantifier(ctxt);
    return(1);
}