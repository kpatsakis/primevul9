xsltParseXSLTStylesheetElemCore(xsltCompilerCtxtPtr cctxt, xmlNodePtr node)
{
#ifdef WITH_XSLT_DEBUG_PARSING
    int templates = 0;
#endif
    xmlNodePtr cur, start = NULL;
    xsltStylesheetPtr style;

    if ((cctxt == NULL) || (node == NULL) ||
	(node->type != XML_ELEMENT_NODE))
	return(-1);    

    style = cctxt->style;    
    /*
    * At this stage all import declarations of all stylesheet modules
    * with the same stylesheet level have been processed.
    * Now we can safely parse the rest of the declarations.
    */
    if (IS_XSLT_ELEM_FAST(node) && IS_XSLT_NAME(node, "include"))
    {
	xsltDocumentPtr include;
	/*
	* URGENT TODO: Make this work with simplified stylesheets!
	*   I.e., when we won't find an xsl:stylesheet element.
	*/
	/*
	* This is as include declaration.
	*/
	include = ((xsltStyleItemIncludePtr) node->psvi)->include;
	if (include == NULL) {
	    /* TODO: raise error? */
	    return(-1);
	}
	/*
	* TODO: Actually an xsl:include should locate an embedded
	*  stylesheet as well; so the document-element won't always
	*  be the element where the actual stylesheet is rooted at.
	*  But such embedded stylesheets are not supported by Libxslt yet.
	*/
	node = xmlDocGetRootElement(include->doc);
	if (node == NULL) {
	    return(-1);
	}
    }    
    
    if (node->children == NULL)
	return(0);
    /*
    * Push the xsl:stylesheet/xsl:transform element.
    */  
    xsltCompilerNodePush(cctxt, node);
    cctxt->inode->isRoot = 1;
    cctxt->inode->nsChanged = 0;
    /*
    * Start with the naked dummy info for literal result elements.
    */
    cctxt->inode->litResElemInfo = cctxt->inodeList->litResElemInfo;

    /*
    * In every case, we need to have
    * the in-scope namespaces of the element, where the
    * stylesheet is rooted at, regardless if it's an XSLT
    * instruction or a literal result instruction (or if
    * this is an embedded stylesheet).
    */		
    cctxt->inode->inScopeNs =
	xsltCompilerBuildInScopeNsList(cctxt, node);

    /*
    * Process attributes of xsl:stylesheet/xsl:transform.
    * --------------------------------------------------
    * Allowed are:
    *  id = id
    *  extension-element-prefixes = tokens
    *  exclude-result-prefixes = tokens
    *  version = number (mandatory)    
    */
    if (xsltParseAttrXSLTVersion(cctxt, node,
	XSLT_ELEMENT_CATEGORY_XSLT) == 0)
    {    
	/*
	* Attribute "version".
	* XSLT 1.0: "An xsl:stylesheet element *must* have a version
	*  attribute, indicating the version of XSLT that the
	*  stylesheet requires".
	* The root element of a simplified stylesheet must also have
	* this attribute.
	*/
#ifdef XSLT_REFACTORED_MANDATORY_VERSION
	if (isXsltElem)
	    xsltTransformError(NULL, cctxt->style, node,
		"The attribute 'version' is missing.\n");
	cctxt->style->errors++;	
#else
	/* OLD behaviour. */
	xsltTransformError(NULL, cctxt->style, node,
	    "xsl:version is missing: document may not be a stylesheet\n");
	cctxt->style->warnings++;
#endif
    }    
    /*
    * The namespaces declared by the attributes
    *  "extension-element-prefixes" and
    *  "exclude-result-prefixes" are local to *this*
    *  stylesheet tree; i.e., they are *not* visible to
    *  other stylesheet-modules, whether imported or included.
    * 
    * Attribute "extension-element-prefixes".
    */
    cctxt->inode->extElemNs =
	xsltParseExtElemPrefixes(cctxt, node, NULL,
	    XSLT_ELEMENT_CATEGORY_XSLT);
    /*
    * Attribute "exclude-result-prefixes".
    */
    cctxt->inode->exclResultNs =
	xsltParseExclResultPrefixes(cctxt, node, NULL,
	    XSLT_ELEMENT_CATEGORY_XSLT);
    /*
    * Create/reuse info for the literal result element.
    */
    if (cctxt->inode->nsChanged)
	xsltLREInfoCreate(cctxt, node, 0);
    /*
    * Processed top-level elements:
    * ----------------------------
    *  xsl:variable, xsl:param (QName, in-scope ns,
    *    expression (vars allowed))
    *  xsl:attribute-set (QName, in-scope ns)
    *  xsl:strip-space, xsl:preserve-space (XPath NameTests,
    *    in-scope ns)
    *    I *think* global scope, merge with includes
    *  xsl:output (QName, in-scope ns)
    *  xsl:key (QName, in-scope ns, pattern,
    *    expression (vars *not* allowed))
    *  xsl:decimal-format (QName, needs in-scope ns)
    *  xsl:namespace-alias (in-scope ns)
    *    global scope, merge with includes
    *  xsl:template (last, QName, pattern)
    *
    * (whitespace-only text-nodes have *not* been removed
    *  yet; this will be done in xsltParseSequenceConstructor)
    *
    * Report misplaced child-nodes first.
    */
    cur = node->children;
    while (cur != NULL) {
	if (cur->type == XML_TEXT_NODE) {
	    xsltTransformError(NULL, style, cur,
		"Misplaced text node (content: '%s').\n",
		(cur->content != NULL) ? cur->content : BAD_CAST "");
	    style->errors++;
	} else if (cur->type != XML_ELEMENT_NODE) {
	    xsltTransformError(NULL, style, cur, "Misplaced node.\n");
	    style->errors++;
	}
	cur = cur->next;
    }
    /*
    * Skip xsl:import elements; they have been processed
    * already.
    */
    cur = node->children;
    while ((cur != NULL) && xsltParseFindTopLevelElem(cctxt, cur,
	    BAD_CAST "import", XSLT_NAMESPACE, 1, &cur) == 1)
	cur = cur->next;
    if (cur == NULL)
	goto exit;

    start = cur;
    /*
    * Process all top-level xsl:param elements.
    */
    while ((cur != NULL) &&
	xsltParseFindTopLevelElem(cctxt, cur,
	BAD_CAST "param", XSLT_NAMESPACE, 0, &cur) == 1)
    {
	xsltParseTopLevelXSLTElem(cctxt, cur, XSLT_FUNC_PARAM);	
	cur = cur->next;
    }  
    /*
    * Process all top-level xsl:variable elements.
    */
    cur = start;
    while ((cur != NULL) &&
	xsltParseFindTopLevelElem(cctxt, cur,
	BAD_CAST "variable", XSLT_NAMESPACE, 0, &cur) == 1)
    {
	xsltParseTopLevelXSLTElem(cctxt, cur, XSLT_FUNC_VARIABLE);
	cur = cur->next;
    }   
    /*
    * Process all the rest of top-level elements.
    */
    cur = start;
    while (cur != NULL) {	
	/*
	* Process element nodes.
	*/
	if (cur->type == XML_ELEMENT_NODE) {	    
	    if (cur->ns == NULL) {
		xsltTransformError(NULL, style, cur,
		    "Unexpected top-level element in no namespace.\n");
		style->errors++;
		cur = cur->next;
		continue;
	    }
	    /*
	    * Process all XSLT elements.
	    */
	    if (IS_XSLT_ELEM_FAST(cur)) {
		/*
		* xsl:import is only allowed at the beginning.
		*/
		if (IS_XSLT_NAME(cur, "import")) {
		    xsltTransformError(NULL, style, cur,
			"Misplaced xsl:import element.\n");
		    style->errors++;
		    cur = cur->next;
		    continue;
		}
		/* 
		* TODO: Change the return type of the parsing functions
		*  to int.
		*/
		if (IS_XSLT_NAME(cur, "template")) {
#ifdef WITH_XSLT_DEBUG_PARSING
		    templates++;
#endif
		    /*
		    * TODO: Is the position of xsl:template in the
		    *  tree significant? If not it would be easier to
		    *  parse them at a later stage.
		    */
		    xsltParseXSLTTemplate(cctxt, cur);
		} else if (IS_XSLT_NAME(cur, "variable")) {
		    /* NOP; done already */
		} else if (IS_XSLT_NAME(cur, "param")) {
		    /* NOP; done already */
		} else if (IS_XSLT_NAME(cur, "include")) {		    
		    if (cur->psvi != NULL)		    
			xsltParseXSLTStylesheetElemCore(cctxt, cur);
		    else {
			xsltTransformError(NULL, style, cur,
			    "Internal error: "
			    "(xsltParseXSLTStylesheetElemCore) "
			    "The xsl:include element was not compiled.\n");
			style->errors++;
		    }
		} else if (IS_XSLT_NAME(cur, "strip-space")) {
		    /* No node info needed. */
		    xsltParseStylesheetStripSpace(style, cur);
		} else if (IS_XSLT_NAME(cur, "preserve-space")) {
		    /* No node info needed. */
		    xsltParseStylesheetPreserveSpace(style, cur);
		} else if (IS_XSLT_NAME(cur, "output")) {
		    /* No node-info needed. */
		    xsltParseStylesheetOutput(style, cur);
		} else if (IS_XSLT_NAME(cur, "key")) {
		    /* TODO: node-info needed for expressions ? */
		    xsltParseStylesheetKey(style, cur);
		} else if (IS_XSLT_NAME(cur, "decimal-format")) {
		    /* No node-info needed. */		     
		    xsltParseStylesheetDecimalFormat(style, cur);
		} else if (IS_XSLT_NAME(cur, "attribute-set")) {		    
		    xsltParseTopLevelXSLTElem(cctxt, cur,
			XSLT_FUNC_ATTRSET);		
		} else if (IS_XSLT_NAME(cur, "namespace-alias")) {
		    /* NOP; done already */		    
		} else {
		    if (cctxt->inode->forwardsCompat) {
			/*
			* Forwards-compatible mode:
			*
			* XSLT-1: "if it is a top-level element and
			*  XSLT 1.0 does not allow such elements as top-level
			*  elements, then the element must be ignored along
			*  with its content;"
			*/
			/*
			* TODO: I don't think we should generate a warning.
			*/
			xsltTransformError(NULL, style, cur,
			    "Forwards-compatible mode: Ignoring unknown XSLT "
			    "element '%s'.\n", cur->name);
			style->warnings++;
		    } else {
			xsltTransformError(NULL, style, cur,
			    "Unknown XSLT element '%s'.\n", cur->name);
			style->errors++;
		    }
		}
	    } else {
		xsltTopLevelFunction function;

		/*
		* Process non-XSLT elements, which are in a
		*  non-NULL namespace.
		*/
		/*
		* QUESTION: What does xsltExtModuleTopLevelLookup()
		*  do exactly?
		*/
		function = xsltExtModuleTopLevelLookup(cur->name,
		    cur->ns->href);
		if (function != NULL)
		    function(style, cur);
#ifdef WITH_XSLT_DEBUG_PARSING
		xsltGenericDebug(xsltGenericDebugContext,
		    "xsltParseXSLTStylesheetElemCore : User-defined "
		    "data element '%s'.\n", cur->name);
#endif
	    }
	}
	cur = cur->next;
    }

exit:

#ifdef WITH_XSLT_DEBUG_PARSING
    xsltGenericDebug(xsltGenericDebugContext,
	"### END of parsing top-level elements of doc '%s'.\n",
	node->doc->URL);
    xsltGenericDebug(xsltGenericDebugContext,
	"### Templates: %d\n", templates);
#ifdef XSLT_REFACTORED
    xsltGenericDebug(xsltGenericDebugContext,
	"### Max inodes: %d\n", cctxt->maxNodeInfos);
    xsltGenericDebug(xsltGenericDebugContext,
	"### Max LREs  : %d\n", cctxt->maxLREs);
#endif /* XSLT_REFACTORED */
#endif /* WITH_XSLT_DEBUG_PARSING */

    xsltCompilerNodePop(cctxt, node);
    return(0);
}