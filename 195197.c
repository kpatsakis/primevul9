xmlXIncludeFreeContext(xmlXIncludeCtxtPtr ctxt) {
    int i;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Freeing context\n");
#endif
    if (ctxt == NULL)
	return;
    while (ctxt->urlNr > 0)
	xmlXIncludeURLPop(ctxt);
    if (ctxt->urlTab != NULL)
	xmlFree(ctxt->urlTab);
    for (i = 0;i < ctxt->incNr;i++) {
	if (ctxt->incTab[i] != NULL)
	    xmlXIncludeFreeRef(ctxt->incTab[i]);
    }
    if (ctxt->incTab != NULL)
	xmlFree(ctxt->incTab);
    if (ctxt->txtTab != NULL) {
	for (i = 0;i < ctxt->txtNr;i++) {
	    if (ctxt->txtTab[i] != NULL)
		xmlFree(ctxt->txtTab[i]);
	}
	xmlFree(ctxt->txtTab);
    }
    if (ctxt->txturlTab != NULL) {
	for (i = 0;i < ctxt->txtNr;i++) {
	    if (ctxt->txturlTab[i] != NULL)
		xmlFree(ctxt->txturlTab[i]);
	}
	xmlFree(ctxt->txturlTab);
    }
    if (ctxt->base != NULL) {
        xmlFree(ctxt->base);
    }
    xmlFree(ctxt);
}