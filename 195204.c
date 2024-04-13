xmlXIncludeURLPush(xmlXIncludeCtxtPtr ctxt,
	           const xmlChar *value)
{
    if (ctxt->urlNr > XINCLUDE_MAX_DEPTH) {
	xmlXIncludeErr(ctxt, NULL, XML_XINCLUDE_RECURSION,
	               "detected a recursion in %s\n", value);
	return(-1);
    }
    if (ctxt->urlTab == NULL) {
	ctxt->urlMax = 4;
	ctxt->urlNr = 0;
	ctxt->urlTab = (xmlChar * *) xmlMalloc(
		        ctxt->urlMax * sizeof(ctxt->urlTab[0]));
        if (ctxt->urlTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "adding URL");
            return (-1);
        }
    }
    if (ctxt->urlNr >= ctxt->urlMax) {
        ctxt->urlMax *= 2;
        ctxt->urlTab =
            (xmlChar * *) xmlRealloc(ctxt->urlTab,
                                      ctxt->urlMax *
                                      sizeof(ctxt->urlTab[0]));
        if (ctxt->urlTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "adding URL");
            return (-1);
        }
    }
    ctxt->url = ctxt->urlTab[ctxt->urlNr] = xmlStrdup(value);
    return (ctxt->urlNr++);
}