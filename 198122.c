xmlFAGenerateEpsilonTransition(xmlRegParserCtxtPtr ctxt,
			       xmlRegStatePtr from, xmlRegStatePtr to) {
    if (to == NULL) {
	to = xmlRegNewState(ctxt);
	xmlRegStatePush(ctxt, to);
	ctxt->state = to;
    }
    xmlRegStateAddTrans(ctxt, from, NULL, to, -1, -1);
}