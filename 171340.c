xsltCompilationCtxtCreate(xsltStylesheetPtr style) {
    xsltCompilerCtxtPtr ret;

    ret = (xsltCompilerCtxtPtr) xmlMalloc(sizeof(xsltCompilerCtxt));
    if (ret == NULL) {
	xsltTransformError(NULL, style, NULL,
	    "xsltCompilerCreate: allocation of compiler "
	    "context failed.\n");
	return(NULL);
    }
    memset(ret, 0, sizeof(xsltCompilerCtxt));

    ret->errSeverity = XSLT_ERROR_SEVERITY_ERROR;
    ret->tmpList = xsltPointerListCreate(20);
    if (ret->tmpList == NULL) {
	goto internal_err;
    }
#ifdef XSLT_REFACTORED_XPATHCOMP
    /*
    * Create the XPath compilation context in order
    * to speed up precompilation of XPath expressions.
    */
    ret->xpathCtxt = xmlXPathNewContext(NULL);
    if (ret->xpathCtxt == NULL)
	goto internal_err;
#endif
    
    return(ret);

internal_err:
    xsltCompilationCtxtFree(ret);
    return(NULL);
}