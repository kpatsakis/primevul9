xsltParseStylesheetTop(xsltStylesheetPtr style, xmlNodePtr top) {
    xmlNodePtr cur;
    xmlChar *prop;
#ifdef WITH_XSLT_DEBUG_PARSING
    int templates = 0;
#endif

    if (top == NULL)
	return;

    prop = xmlGetNsProp(top, (const xmlChar *)"version", NULL);
    if (prop == NULL) {
	xsltTransformError(NULL, style, top,
	    "xsl:version is missing: document may not be a stylesheet\n");
	if (style != NULL) style->warnings++;
    } else {
	if ((!xmlStrEqual(prop, (const xmlChar *)"1.0")) &&
            (!xmlStrEqual(prop, (const xmlChar *)"1.1"))) {
	    xsltTransformError(NULL, style, top,
		"xsl:version: only 1.0 features are supported\n");
	     /* TODO set up compatibility when not XSLT 1.0 */
	    if (style != NULL) style->warnings++;
	}
	xmlFree(prop);
    }

    /*
     * process xsl:import elements
     */
    cur = top->children;
    while (cur != NULL) {
	    if (IS_BLANK_NODE(cur)) {
		    cur = cur->next;
		    continue;
	    }
	    if (IS_XSLT_ELEM(cur) && IS_XSLT_NAME(cur, "import")) {
		    if (xsltParseStylesheetImport(style, cur) != 0)
			    if (style != NULL) style->errors++;
	    } else
		    break;
	    cur = cur->next;
    }

    /*
     * process other top-level elements
     */
    while (cur != NULL) {
	if (IS_BLANK_NODE(cur)) {
	    cur = cur->next;
	    continue;
	}
	if (cur->type == XML_TEXT_NODE) {
	    if (cur->content != NULL) {
		xsltTransformError(NULL, style, cur,
		    "misplaced text node: '%s'\n", cur->content);
	    }
	    if (style != NULL) style->errors++;
            cur = cur->next;
	    continue;
	}
	if ((cur->type == XML_ELEMENT_NODE) && (cur->ns == NULL)) {
	    xsltGenericError(xsltGenericErrorContext,
		     "Found a top-level element %s with null namespace URI\n",
		     cur->name);
	    if (style != NULL) style->errors++;
	    cur = cur->next;
	    continue;
	}
	if ((cur->type == XML_ELEMENT_NODE) && (!(IS_XSLT_ELEM(cur)))) {
	    xsltTopLevelFunction function;

	    function = xsltExtModuleTopLevelLookup(cur->name,
						   cur->ns->href);
	    if (function != NULL)
		function(style, cur);

#ifdef WITH_XSLT_DEBUG_PARSING
	    xsltGenericDebug(xsltGenericDebugContext,
		    "xsltParseStylesheetTop : found foreign element %s\n",
		    cur->name);
#endif
            cur = cur->next;
	    continue;
	}
	if (IS_XSLT_NAME(cur, "import")) {
	    xsltTransformError(NULL, style, cur,
			"xsltParseStylesheetTop: ignoring misplaced import element\n");
	    if (style != NULL) style->errors++;
    } else if (IS_XSLT_NAME(cur, "include")) {
	    if (xsltParseStylesheetInclude(style, cur) != 0)
		if (style != NULL) style->errors++;
    } else if (IS_XSLT_NAME(cur, "strip-space")) {
	    xsltParseStylesheetStripSpace(style, cur);
    } else if (IS_XSLT_NAME(cur, "preserve-space")) {
	    xsltParseStylesheetPreserveSpace(style, cur);
    } else if (IS_XSLT_NAME(cur, "output")) {
	    xsltParseStylesheetOutput(style, cur);
    } else if (IS_XSLT_NAME(cur, "key")) {
	    xsltParseStylesheetKey(style, cur);
    } else if (IS_XSLT_NAME(cur, "decimal-format")) {
	    xsltParseStylesheetDecimalFormat(style, cur);
    } else if (IS_XSLT_NAME(cur, "attribute-set")) {
	    xsltParseStylesheetAttributeSet(style, cur);
    } else if (IS_XSLT_NAME(cur, "variable")) {
	    xsltParseGlobalVariable(style, cur);
    } else if (IS_XSLT_NAME(cur, "param")) {
	    xsltParseGlobalParam(style, cur);
    } else if (IS_XSLT_NAME(cur, "template")) {
#ifdef WITH_XSLT_DEBUG_PARSING
	    templates++;
#endif
	    xsltParseStylesheetTemplate(style, cur);
    } else if (IS_XSLT_NAME(cur, "namespace-alias")) {
	    xsltNamespaceAlias(style, cur);
	} else {
	    /*
	    * BUG TODO: The version of the *doc* is irrelevant for
	    *  the forwards-compatible mode.
	    */
            if ((style != NULL) && (style->doc->version != NULL) &&
	        (!strncmp((const char *) style->doc->version, "1.0", 3))) {
	        xsltTransformError(NULL, style, cur,
			"xsltParseStylesheetTop: unknown %s element\n",
			cur->name);
	        if (style != NULL) style->errors++;
	    }
	    else {
                /* do Forwards-Compatible Processing */
	        xsltTransformError(NULL, style, cur,
			"xsltParseStylesheetTop: ignoring unknown %s element\n",
			cur->name);
	        if (style != NULL) style->warnings++;
            }
	}
	cur = cur->next;
    }
#ifdef WITH_XSLT_DEBUG_PARSING
    xsltGenericDebug(xsltGenericDebugContext,
		    "parsed %d templates\n", templates);
#endif
}