xmlXIncludeErrMemory(xmlXIncludeCtxtPtr ctxt, xmlNodePtr node,
                     const char *extra)
{
    if (ctxt != NULL)
	ctxt->nbErrors++;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, node, XML_FROM_XINCLUDE,
                    XML_ERR_NO_MEMORY, XML_ERR_ERROR, NULL, 0,
		    extra, NULL, NULL, 0, 0,
		    "Memory allocation failed : %s\n", extra);
}