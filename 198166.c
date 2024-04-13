xmlFAGenerateCountedTransition(xmlRegParserCtxtPtr ctxt,
	    xmlRegStatePtr from, xmlRegStatePtr to, int counter) {
    if (to == NULL) {
	to = xmlRegNewState(ctxt);
	xmlRegStatePush(ctxt, to);
	ctxt->state = to;
    }
    xmlRegStateAddTrans(ctxt, from, NULL, to, -1, counter);
}