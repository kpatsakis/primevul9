xmlRegexpErrCompile(xmlRegParserCtxtPtr ctxt, const char *extra)
{
    const char *regexp = NULL;
    int idx = 0;

    if (ctxt != NULL) {
        regexp = (const char *) ctxt->string;
	idx = ctxt->cur - ctxt->string;
	ctxt->error = XML_REGEXP_COMPILE_ERROR;
    }
    __xmlRaiseError(NULL, NULL, NULL, NULL, NULL, XML_FROM_REGEXP,
		    XML_REGEXP_COMPILE_ERROR, XML_ERR_FATAL, NULL, 0, extra,
		    regexp, NULL, idx, 0,
		    "failed to compile: %s\n", extra);
}