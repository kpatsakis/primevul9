xsltParseStylesheetOutput(xsltStylesheetPtr style, xmlNodePtr cur)
{
    xmlChar *elements,
     *prop;
    xmlChar *element,
     *end;

    if ((cur == NULL) || (style == NULL))
        return;
   
    prop = xmlGetNsProp(cur, (const xmlChar *) "version", NULL);
    if (prop != NULL) {
        if (style->version != NULL)
            xmlFree(style->version);
        style->version = prop;
    }

    prop = xmlGetNsProp(cur, (const xmlChar *) "encoding", NULL);
    if (prop != NULL) {
        if (style->encoding != NULL)
            xmlFree(style->encoding);
        style->encoding = prop;
    }

    /* relaxed to support xt:document
    * TODO KB: What does "relaxed to support xt:document" mean?
    */
    prop = xmlGetNsProp(cur, (const xmlChar *) "method", NULL);
    if (prop != NULL) {
        const xmlChar *URI;

        if (style->method != NULL)
            xmlFree(style->method);
        style->method = NULL;
        if (style->methodURI != NULL)
            xmlFree(style->methodURI);
        style->methodURI = NULL;

	/*
	* TODO: Don't use xsltGetQNameURI().
	*/
	URI = xsltGetQNameURI(cur, &prop);
	if (prop == NULL) {
	    if (style != NULL) style->errors++;
	} else if (URI == NULL) {
            if ((xmlStrEqual(prop, (const xmlChar *) "xml")) ||
                (xmlStrEqual(prop, (const xmlChar *) "html")) ||
                (xmlStrEqual(prop, (const xmlChar *) "text"))) {
                style->method = prop;
            } else {
		xsltTransformError(NULL, style, cur,
                                 "invalid value for method: %s\n", prop);
                if (style != NULL) style->warnings++;
            }
	} else {
	    style->method = prop;
	    style->methodURI = xmlStrdup(URI);
	}
    }

    prop = xmlGetNsProp(cur, (const xmlChar *) "doctype-system", NULL);
    if (prop != NULL) {
        if (style->doctypeSystem != NULL)
            xmlFree(style->doctypeSystem);
        style->doctypeSystem = prop;
    }

    prop = xmlGetNsProp(cur, (const xmlChar *) "doctype-public", NULL);
    if (prop != NULL) {
        if (style->doctypePublic != NULL)
            xmlFree(style->doctypePublic);
        style->doctypePublic = prop;
    }

    prop = xmlGetNsProp(cur, (const xmlChar *) "standalone", NULL);
    if (prop != NULL) {
        if (xmlStrEqual(prop, (const xmlChar *) "yes")) {
            style->standalone = 1;
        } else if (xmlStrEqual(prop, (const xmlChar *) "no")) {
            style->standalone = 0;
        } else {
	    xsltTransformError(NULL, style, cur,
                             "invalid value for standalone: %s\n", prop);
            style->errors++;
        }
        xmlFree(prop);
    }

    prop = xmlGetNsProp(cur, (const xmlChar *) "indent", NULL);
    if (prop != NULL) {
        if (xmlStrEqual(prop, (const xmlChar *) "yes")) {
            style->indent = 1;
        } else if (xmlStrEqual(prop, (const xmlChar *) "no")) {
            style->indent = 0;
        } else {
	    xsltTransformError(NULL, style, cur,
                             "invalid value for indent: %s\n", prop);
            style->errors++;
        }
        xmlFree(prop);
    }

    prop = xmlGetNsProp(cur, (const xmlChar *) "omit-xml-declaration", NULL);
    if (prop != NULL) {
        if (xmlStrEqual(prop, (const xmlChar *) "yes")) {
            style->omitXmlDeclaration = 1;
        } else if (xmlStrEqual(prop, (const xmlChar *) "no")) {
            style->omitXmlDeclaration = 0;
        } else {
	    xsltTransformError(NULL, style, cur,
                             "invalid value for omit-xml-declaration: %s\n",
                             prop);
            style->errors++;
        }
        xmlFree(prop);
    }

    elements = xmlGetNsProp(cur, (const xmlChar *) "cdata-section-elements",
	NULL);
    if (elements != NULL) {
        if (style->cdataSection == NULL)
            style->cdataSection = xmlHashCreate(10);
        if (style->cdataSection == NULL)
            return;

        element = elements;
        while (*element != 0) {
            while (IS_BLANK(*element))
                element++;
            if (*element == 0)
                break;
            end = element;
            while ((*end != 0) && (!IS_BLANK(*end)))
                end++;
            element = xmlStrndup(element, end - element);
            if (element) {		
#ifdef WITH_XSLT_DEBUG_PARSING
                xsltGenericDebug(xsltGenericDebugContext,
                                 "add cdata section output element %s\n",
                                 element);
#endif
		if (xmlValidateQName(BAD_CAST element, 0) != 0) {
		    xsltTransformError(NULL, style, cur,
			"Attribute 'cdata-section-elements': The value "
			"'%s' is not a valid QName.\n", element);
		    xmlFree(element);
		    style->errors++;
		} else {
		    const xmlChar *URI;

		    /*
		    * TODO: Don't use xsltGetQNameURI().
		    */
		    URI = xsltGetQNameURI(cur, &element);
		    if (element == NULL) {
			/*
			* TODO: We'll report additionally an error
			*  via the stylesheet's error handling.			
			*/
			xsltTransformError(NULL, style, cur,
			    "Attribute 'cdata-section-elements': The value "
			    "'%s' is not a valid QName.\n", element);
			style->errors++;
		    } else {
			xmlNsPtr ns;
			
			/*
			* XSLT-1.0 "Each QName is expanded into an
			*  expanded-name using the namespace declarations in
			*  effect on the xsl:output element in which the QName
			*  occurs; if there is a default namespace, it is used
			*  for QNames that do not have a prefix"
			* NOTE: Fix of bug #339570.
			*/
			if (URI == NULL) {
			    ns = xmlSearchNs(style->doc, cur, NULL);
			    if (ns != NULL)
				URI = ns->href;
			}		   
			xmlHashAddEntry2(style->cdataSection, element, URI,
			    (void *) "cdata");
			xmlFree(element);
		    }
		}
            }
            element = end;
        }
        xmlFree(elements);
    }

    prop = xmlGetNsProp(cur, (const xmlChar *) "media-type", NULL);
    if (prop != NULL) {
	if (style->mediaType)
	    xmlFree(style->mediaType);
	style->mediaType = prop;
    }
    if (cur->children != NULL) {
	xsltParseContentError(style, cur->children);
    }
}