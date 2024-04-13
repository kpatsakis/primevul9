xmlXIncludeNewContext(xmlDocPtr doc) {
    xmlXIncludeCtxtPtr ret;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "New context\n");
#endif
    if (doc == NULL)
	return(NULL);
    ret = (xmlXIncludeCtxtPtr) xmlMalloc(sizeof(xmlXIncludeCtxt));
    if (ret == NULL) {
	xmlXIncludeErrMemory(NULL, (xmlNodePtr) doc,
	                     "creating XInclude context");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlXIncludeCtxt));
    ret->doc = doc;
    ret->incNr = 0;
    ret->incBase = 0;
    ret->incMax = 0;
    ret->incTab = NULL;
    ret->nbErrors = 0;
    return(ret);
}