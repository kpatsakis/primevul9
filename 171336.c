xsltNewPrincipalStylesheetData(void)
{
    xsltPrincipalStylesheetDataPtr ret;

    ret = (xsltPrincipalStylesheetDataPtr)
	xmlMalloc(sizeof(xsltPrincipalStylesheetData));
    if (ret == NULL) {
	xsltTransformError(NULL, NULL, NULL,
	    "xsltNewPrincipalStylesheetData: memory allocation failed.\n");
	return(NULL);
    }
    memset(ret, 0, sizeof(xsltPrincipalStylesheetData));
    
    /*
    * Global list of in-scope namespaces.
    */    
    ret->inScopeNamespaces = xsltPointerListCreate(-1);
    if (ret->inScopeNamespaces == NULL)
	goto internal_err;
    /*
    * Global list of excluded result ns-decls.
    */
    ret->exclResultNamespaces = xsltPointerListCreate(-1);
    if (ret->exclResultNamespaces == NULL)
	goto internal_err;
    /*
    * Global list of extension instruction namespace names.
    */    
    ret->extElemNamespaces = xsltPointerListCreate(-1);
    if (ret->extElemNamespaces == NULL)
	goto internal_err;

    return(ret);

internal_err:

    return(NULL);
}