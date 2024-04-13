xsltParseStylesheetImportedDoc(xmlDocPtr doc,
			       xsltStylesheetPtr parentStyle) {
    xsltStylesheetPtr retStyle;

    if (doc == NULL)
	return(NULL);

    retStyle = xsltNewStylesheet();
    if (retStyle == NULL)
	return(NULL);
    /*
    * Set the importing stylesheet module; also used to detect recursion.
    */
    retStyle->parent = parentStyle;
    /*
    * Adjust the string dict.
    */
    if (doc->dict != NULL) {
        xmlDictFree(retStyle->dict);
	retStyle->dict = doc->dict;
#ifdef WITH_XSLT_DEBUG
        xsltGenericDebug(xsltGenericDebugContext,
	    "reusing dictionary from %s for stylesheet\n",
	    doc->URL);
#endif
	xmlDictReference(retStyle->dict);
    }    	
    
    /*
    * TODO: Eliminate xsltGatherNamespaces(); we must not restrict
    *  the stylesheet to containt distinct namespace prefixes.
    */
    xsltGatherNamespaces(retStyle);

#ifdef XSLT_REFACTORED
    {
	xsltCompilerCtxtPtr cctxt;
	xsltStylesheetPtr oldCurSheet;
	    
	if (parentStyle == NULL) {
	    xsltPrincipalStylesheetDataPtr principalData;
	    /*
	    * Principal stylesheet
	    * --------------------
	    */
	    retStyle->principal = retStyle;
	    /*
	    * Create extra data for the principal stylesheet.
	    */
	    principalData = xsltNewPrincipalStylesheetData();
	    if (principalData == NULL) {
		xsltFreeStylesheet(retStyle);
		return(NULL);
	    }
	    retStyle->principalData = principalData;
	    /*
	    * Create the compilation context
	    * ------------------------------
	    * (only once; for the principal stylesheet).
	    * This is currently the only function where the
	    * compilation context is created.
	    */
	    cctxt = xsltCompilationCtxtCreate(retStyle);
	    if (cctxt == NULL) {
		xsltFreeStylesheet(retStyle);
		return(NULL);
	    }	    	    
	    retStyle->compCtxt = (void *) cctxt;
	    cctxt->style = retStyle;
	    cctxt->dict = retStyle->dict;
	    cctxt->psData = principalData;
	    /*
	    * Push initial dummy node info.
	    */
	    cctxt->depth = -1;
	    xsltCompilerNodePush(cctxt, (xmlNodePtr) doc);
	} else {
	    /*
	    * Imported stylesheet.
	    */
	    retStyle->principal = parentStyle->principal;
	    cctxt = parentStyle->compCtxt;
	    retStyle->compCtxt = cctxt;
	}
	/*
	* Save the old and set the current stylesheet structure in the
	* compilation context.
	*/
	oldCurSheet = cctxt->style;
	cctxt->style = retStyle;
	
	retStyle->doc = doc;
	xsltParseStylesheetProcess(retStyle, doc);
	
	cctxt->style = oldCurSheet;
	if (parentStyle == NULL) {
	    /*
	    * Pop the initial dummy node info.
	    */
	    xsltCompilerNodePop(cctxt, (xmlNodePtr) doc);
	} else {
	    /*
	    * Clear the compilation context of imported
	    * stylesheets.
	    * TODO: really?
	    */
	    /* retStyle->compCtxt = NULL; */
	}
	/*
	* Free the stylesheet if there were errors.
	*/
	if (retStyle != NULL) {
	    if (retStyle->errors != 0) {
#ifdef XSLT_REFACTORED_XSLT_NSCOMP
		/*
		* Restore all changes made to namespace URIs of ns-decls.
		*/
		if (cctxt->psData->nsMap)		
		    xsltRestoreDocumentNamespaces(cctxt->psData->nsMap, doc);
#endif
		/*
		* Detach the doc from the stylesheet; otherwise the doc
		* will be freed in xsltFreeStylesheet().
		*/
		retStyle->doc = NULL;
		/*
		* Cleanup the doc if its the main stylesheet.
		*/
		if (parentStyle == NULL) {
		    xsltCleanupStylesheetTree(doc, xmlDocGetRootElement(doc));
		    if (retStyle->compCtxt != NULL) {			
			xsltCompilationCtxtFree(retStyle->compCtxt);
			retStyle->compCtxt = NULL;
		    }
		}

		xsltFreeStylesheet(retStyle);
		retStyle = NULL;
	    }
	}
    }
        
#else /* XSLT_REFACTORED */
    /*
    * Old behaviour.
    */
    retStyle->doc = doc;
    if (xsltParseStylesheetProcess(retStyle, doc) == NULL) {
		retStyle->doc = NULL;
		xsltFreeStylesheet(retStyle);
		retStyle = NULL;
    }
    if (retStyle != NULL) {
	if (retStyle->errors != 0) {
	    retStyle->doc = NULL;
	    if (parentStyle == NULL)
		xsltCleanupStylesheetTree(doc,
		    xmlDocGetRootElement(doc));
	    xsltFreeStylesheet(retStyle);
	    retStyle = NULL;
	}
    }
#endif /* else of XSLT_REFACTORED */
        
    return(retStyle);
}