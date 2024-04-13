xsltParseStylesheetKey(xsltStylesheetPtr style, xmlNodePtr key) {
    xmlChar *prop = NULL;
    xmlChar *use = NULL;
    xmlChar *match = NULL;
    xmlChar *name = NULL;
    xmlChar *nameURI = NULL;

    if ((style == NULL) || (key == NULL))
	return;

    /*
     * Get arguments
     */
    prop = xmlGetNsProp(key, (const xmlChar *)"name", NULL);
    if (prop != NULL) {
        const xmlChar *URI;

	/*
	* TODO: Don't use xsltGetQNameURI().
	*/
	URI = xsltGetQNameURI(key, &prop);
	if (prop == NULL) {
	    if (style != NULL) style->errors++;
	    goto error;
	} else {
	    name = prop;
	    if (URI != NULL)
		nameURI = xmlStrdup(URI);
	}
#ifdef WITH_XSLT_DEBUG_PARSING
	xsltGenericDebug(xsltGenericDebugContext,
	     "xsltParseStylesheetKey: name %s\n", name);
#endif
    } else {
	xsltTransformError(NULL, style, key,
	    "xsl:key : error missing name\n");
	if (style != NULL) style->errors++;
	goto error;
    }

    match = xmlGetNsProp(key, (const xmlChar *)"match", NULL);
    if (match == NULL) {
	xsltTransformError(NULL, style, key,
	    "xsl:key : error missing match\n");
	if (style != NULL) style->errors++;
	goto error;
    }

    use = xmlGetNsProp(key, (const xmlChar *)"use", NULL);
    if (use == NULL) {
	xsltTransformError(NULL, style, key,
	    "xsl:key : error missing use\n");
	if (style != NULL) style->errors++;
	goto error;
    }

    /*
     * register the keys
     */
    xsltAddKey(style, name, nameURI, match, use, key);


error:
    if (use != NULL)
	xmlFree(use);
    if (match != NULL)
	xmlFree(match);
    if (name != NULL)
	xmlFree(name);
    if (nameURI != NULL)
	xmlFree(nameURI);

    if (key->children != NULL) {
	xsltParseContentError(style, key->children);
    }
}