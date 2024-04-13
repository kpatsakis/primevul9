xsltNewStylesheet(void) {
    xsltStylesheetPtr ret = NULL;    

    ret = (xsltStylesheetPtr) xmlMalloc(sizeof(xsltStylesheet));
    if (ret == NULL) {
	xsltTransformError(NULL, NULL, NULL,
		"xsltNewStylesheet : malloc failed\n");
	goto internal_err;
    }
    memset(ret, 0, sizeof(xsltStylesheet));

    ret->omitXmlDeclaration = -1;
    ret->standalone = -1;
    ret->decimalFormat = xsltNewDecimalFormat(NULL);
    ret->indent = -1;
    ret->errors = 0;
    ret->warnings = 0;
    ret->exclPrefixNr = 0;
    ret->exclPrefixMax = 0;
    ret->exclPrefixTab = NULL;
    ret->extInfos = NULL;
    ret->extrasNr = 0;
    ret->internalized = 1;
    ret->literal_result = 0;
    ret->dict = xmlDictCreate();
#ifdef WITH_XSLT_DEBUG
    xsltGenericDebug(xsltGenericDebugContext,
	"creating dictionary for stylesheet\n");
#endif

    xsltInit();

    return(ret);

internal_err:
    if (ret != NULL)
	xsltFreeStylesheet(ret);
    return(NULL);
}