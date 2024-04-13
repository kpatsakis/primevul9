xmlXIncludeNewRef(xmlXIncludeCtxtPtr ctxt, const xmlChar *URI,
	          xmlNodePtr ref) {
    xmlXIncludeRefPtr ret;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "New ref %s\n", URI);
#endif
    ret = (xmlXIncludeRefPtr) xmlMalloc(sizeof(xmlXIncludeRef));
    if (ret == NULL) {
        xmlXIncludeErrMemory(ctxt, ref, "growing XInclude context");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlXIncludeRef));
    if (URI == NULL)
	ret->URI = NULL;
    else
	ret->URI = xmlStrdup(URI);
    ret->fragment = NULL;
    ret->ref = ref;
    ret->doc = NULL;
    ret->count = 0;
    ret->xml = 0;
    ret->inc = NULL;
    if (ctxt->incMax == 0) {
	ctxt->incMax = 4;
        ctxt->incTab = (xmlXIncludeRefPtr *) xmlMalloc(ctxt->incMax *
					      sizeof(ctxt->incTab[0]));
        if (ctxt->incTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, ref, "growing XInclude context");
	    xmlXIncludeFreeRef(ret);
	    return(NULL);
	}
    }
    if (ctxt->incNr >= ctxt->incMax) {
	ctxt->incMax *= 2;
        ctxt->incTab = (xmlXIncludeRefPtr *) xmlRealloc(ctxt->incTab,
	             ctxt->incMax * sizeof(ctxt->incTab[0]));
        if (ctxt->incTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, ref, "growing XInclude context");
	    xmlXIncludeFreeRef(ret);
	    return(NULL);
	}
    }
    ctxt->incTab[ctxt->incNr++] = ret;
    return(ret);
}