xmlRegexpErrMemory(xmlRegParserCtxtPtr ctxt, const char *extra)
{
    const char *regexp = NULL;
    if (ctxt != NULL) {
        regexp = (const char *) ctxt->string;
	ctxt->error = XML_ERR_NO_MEMORY;
    }
    __xmlRaiseError(NULL, NULL, NULL, NULL, NULL, XML_FROM_REGEXP,
		    XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, extra,
		    regexp, NULL, 0, 0,
		    "Memory allocation failed : %s\n", extra);
}