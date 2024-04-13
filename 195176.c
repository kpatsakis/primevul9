xmlXIncludeErr(xmlXIncludeCtxtPtr ctxt, xmlNodePtr node, int error,
               const char *msg, const xmlChar *extra)
{
    if (ctxt != NULL)
	ctxt->nbErrors++;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, node, XML_FROM_XINCLUDE,
                    error, XML_ERR_ERROR, NULL, 0,
		    (const char *) extra, NULL, NULL, 0, 0,
		    msg, (const char *) extra);
}