xmlXIncludeAddTxt(xmlXIncludeCtxtPtr ctxt, const xmlChar *txt,
                  const xmlURL url) {
#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Adding text %s\n", url);
#endif
    if (ctxt->txtMax == 0) {
	ctxt->txtMax = 4;
        ctxt->txtTab = (xmlChar **) xmlMalloc(ctxt->txtMax *
		                          sizeof(ctxt->txtTab[0]));
        if (ctxt->txtTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "processing text");
	    return;
	}
        ctxt->txturlTab = (xmlURL *) xmlMalloc(ctxt->txtMax *
		                          sizeof(ctxt->txturlTab[0]));
        if (ctxt->txturlTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "processing text");
	    return;
	}
    }
    if (ctxt->txtNr >= ctxt->txtMax) {
	ctxt->txtMax *= 2;
        ctxt->txtTab = (xmlChar **) xmlRealloc(ctxt->txtTab,
	             ctxt->txtMax * sizeof(ctxt->txtTab[0]));
        if (ctxt->txtTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "processing text");
	    return;
	}
        ctxt->txturlTab = (xmlURL *) xmlRealloc(ctxt->txturlTab,
	             ctxt->txtMax * sizeof(ctxt->txturlTab[0]));
        if (ctxt->txturlTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "processing text");
	    return;
	}
    }
    ctxt->txtTab[ctxt->txtNr] = xmlStrdup(txt);
    ctxt->txturlTab[ctxt->txtNr] = xmlStrdup(url);
    ctxt->txtNr++;
}