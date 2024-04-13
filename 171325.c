xsltParsePreprocessStylesheetTree(xsltCompilerCtxtPtr cctxt, xmlNodePtr node)
{
    xmlNodePtr deleteNode, cur, txt, textNode = NULL;
    xmlDocPtr doc;
    xsltStylesheetPtr style;
    int internalize = 0, findSpaceAttr;
    int xsltStylesheetElemDepth;
    xmlAttrPtr attr;
    xmlChar *value;
    const xmlChar *name, *nsNameXSLT = NULL;
    int strictWhitespace, inXSLText = 0;
#ifdef XSLT_REFACTORED_XSLT_NSCOMP
    xsltNsMapPtr nsMapItem;
#endif

    if ((cctxt == NULL) || (cctxt->style == NULL) ||
	(node == NULL) || (node->type != XML_ELEMENT_NODE))
        return(-1);

    doc = node->doc;
    if (doc == NULL)
	goto internal_err;

    style = cctxt->style;
    if ((style->dict != NULL) && (doc->dict == style->dict))
	internalize = 1;
    else
        style->internalized = 0;

    /*
    * Init value of xml:space. Since this might be an embedded
    * stylesheet, this is needed to be performed on the element
    * where the stylesheet is rooted at, taking xml:space of
    * ancestors into account.
    */
    if (! cctxt->simplified)
	xsltStylesheetElemDepth = cctxt->depth +1;
    else
	xsltStylesheetElemDepth = 0;

    if (xmlNodeGetSpacePreserve(node) != 1)
	cctxt->inode->preserveWhitespace = 0;
    else
	cctxt->inode->preserveWhitespace = 1; 
    
    /*
    * Eval if we should keep the old incorrect behaviour.
    */
    strictWhitespace = (cctxt->strict != 0) ? 1 : 0;

    nsNameXSLT = xsltConstNamespaceNameXSLT;

    deleteNode = NULL;
    cur = node;
    while (cur != NULL) {
	if (deleteNode != NULL)	{

#ifdef WITH_XSLT_DEBUG_BLANKS
	    xsltGenericDebug(xsltGenericDebugContext,
	     "xsltParsePreprocessStylesheetTree: removing node\n");
#endif
	    xmlUnlinkNode(deleteNode);
	    xmlFreeNode(deleteNode);
	    deleteNode = NULL;
	}
	if (cur->type == XML_ELEMENT_NODE) {
	    
	    /*
	    * Clear the PSVI field.
	    */
	    cur->psvi = NULL;

	    xsltCompilerNodePush(cctxt, cur);

	    inXSLText = 0;
	    textNode = NULL;	    
	    findSpaceAttr = 1;	    
	    cctxt->inode->stripWhitespace = 0;
	    /*
	    * TODO: I'd love to use a string pointer comparison here :-/
	    */
	    if (IS_XSLT_ELEM(cur)) {
#ifdef XSLT_REFACTORED_XSLT_NSCOMP
		if (cur->ns->href != nsNameXSLT) {
		    nsMapItem = xsltNewNamespaceMapItem(cctxt,
			doc, cur->ns, cur);
		    if (nsMapItem == NULL)
			goto internal_err;
		    cur->ns->href = nsNameXSLT;
		}
#endif

		if (cur->name == NULL)
		    goto process_attributes;
		/*
		* Mark the XSLT element for later recognition.
		* TODO: Using the marker is still too dangerous, since if
		*   the parsing mechanism leaves out an XSLT element, then
		*   this might hit the transformation-mechanism, which
		*   will break if it doesn't expect such a marker.
		*/
		/* cur->psvi = (void *) xsltXSLTElemMarker; */

		/*
		* XSLT 2.0: "Any whitespace text node whose parent is
		* one of the following elements is removed from the "
		* tree, regardless of any xml:space attributes:..."
		* xsl:apply-imports, 
		* xsl:apply-templates,
		* xsl:attribute-set,
		* xsl:call-template, 
		* xsl:choose,
		* xsl:stylesheet, xsl:transform.
		* XSLT 2.0: xsl:analyze-string,
		*           xsl:character-map,
		*           xsl:next-match		
		*
		* TODO: I'd love to use a string pointer comparison here :-/
		*/		
		name = cur->name;
		switch (*name) {
		    case 't':
			if ((name[0] == 't') && (name[1] == 'e') &&
			    (name[2] == 'x') && (name[3] == 't') &&
			    (name[4] == 0))
			{
			    /*
			    * Process the xsl:text element.
			    * ----------------------------
			    * Mark it for later recognition.
			    */
			    cur->psvi = (void *) xsltXSLTTextMarker;
			    /*
			    * For stylesheets, the set of
			    * whitespace-preserving element names
			    * consists of just xsl:text.
			    */
			    findSpaceAttr = 0;
			    cctxt->inode->preserveWhitespace = 1;
			    inXSLText = 1;
			}			    
			break;
		    case 'c':
			if (xmlStrEqual(name, BAD_CAST "choose") ||
			    xmlStrEqual(name, BAD_CAST "call-template"))
			    cctxt->inode->stripWhitespace = 1;
			break;
		    case 'a':
			if (xmlStrEqual(name, BAD_CAST "apply-templates") ||
			    xmlStrEqual(name, BAD_CAST "apply-imports") ||
			    xmlStrEqual(name, BAD_CAST "attribute-set"))

			    cctxt->inode->stripWhitespace = 1;
			break;
		    default:
			if (xsltStylesheetElemDepth == cctxt->depth) {
			    /*
			    * This is a xsl:stylesheet/xsl:transform.
			    */
			    cctxt->inode->stripWhitespace = 1;
			    break;
			}

			if ((cur->prev != NULL) &&
			    (cur->prev->type == XML_TEXT_NODE))
			{
			    /*
			    * XSLT 2.0 : "Any whitespace text node whose
			    *  following-sibling node is an xsl:param or
			    *  xsl:sort element is removed from the tree,
			    *  regardless of any xml:space attributes."
			    */
			    if (((*name == 'p') || (*name == 's')) &&
				(xmlStrEqual(name, BAD_CAST "param") ||
				 xmlStrEqual(name, BAD_CAST "sort")))
			    {
				do {
				    if (IS_BLANK_NODE(cur->prev)) {
					txt = cur->prev;
					xmlUnlinkNode(txt);
					xmlFreeNode(txt);
				    } else {
					/*
					* This will result in a content
					* error, when hitting the parsing
					* functions.
					*/
					break;
				    }
				} while (cur->prev);				    
			    }
			}
			break;
		}
	    }

process_attributes:
	    /*
	    * Process attributes.
	    * ------------------
	    */
	    if (cur->properties != NULL) {
		if (cur->children == NULL)
		    findSpaceAttr = 0;
		attr = cur->properties;
		do {
#ifdef XSLT_REFACTORED_XSLT_NSCOMP
		    if ((attr->ns) && (attr->ns->href != nsNameXSLT) &&
			xmlStrEqual(attr->ns->href, nsNameXSLT))
		    {			
			nsMapItem = xsltNewNamespaceMapItem(cctxt,
			    doc, attr->ns, cur);
			if (nsMapItem == NULL)
			    goto internal_err;
			attr->ns->href = nsNameXSLT;
		    }		    
#endif
		    if (internalize) {
			/*
			* Internalize the attribute's value; the goal is to
			* speed up operations and minimize used space by
			* compiled stylesheets.
			*/
			txt = attr->children;
			/*
			* NOTE that this assumes only one
			*  text-node in the attribute's content.
			*/
			if ((txt != NULL) && (txt->content != NULL) &&
			    (!xmlDictOwns(style->dict, txt->content)))
			{
			    value = (xmlChar *) xmlDictLookup(style->dict,
				txt->content, -1);
			    xmlNodeSetContent(txt, NULL);
			    txt->content = value;
			}
		    }
		    /*
		    * Process xml:space attributes.
		    * ----------------------------
		    */
		    if ((findSpaceAttr != 0) &&
			(attr->ns != NULL) &&
			(attr->name != NULL) &&
			(attr->name[0] == 's') &&			
			(attr->ns->prefix != NULL) &&
			(attr->ns->prefix[0] == 'x') &&
			(attr->ns->prefix[1] == 'm') &&
			(attr->ns->prefix[2] == 'l') &&
			(attr->ns->prefix[3] == 0))
		    {
			value = xmlGetNsProp(cur, BAD_CAST "space",
			    XML_XML_NAMESPACE);
			if (value != NULL) {
			    if (xmlStrEqual(value, BAD_CAST "preserve")) {
				cctxt->inode->preserveWhitespace = 1;				
			    } else if (xmlStrEqual(value, BAD_CAST "default")) {
				cctxt->inode->preserveWhitespace = 0;
			    } else {
				/* Invalid value for xml:space. */
				xsltTransformError(NULL, style, cur,
				    "Attribute xml:space: Invalid value.\n");
				cctxt->style->warnings++;
			    }
			    findSpaceAttr = 0;
			    xmlFree(value);
			}
			
		    }
		    attr = attr->next;
		} while (attr != NULL);
	    }
	    /*
	    * We'll descend into the children of element nodes only.
	    */
	    if (cur->children != NULL) {
		cur = cur->children;
		continue;
	    }
	} else if ((cur->type == XML_TEXT_NODE) ||
		(cur->type == XML_CDATA_SECTION_NODE))
	{
	    /*
	    * Merge adjacent text/CDATA-section-nodes
	    * ---------------------------------------	    
	    * In order to avoid breaking of existing stylesheets,
	    * if the old behaviour is wanted (strictWhitespace == 0),
	    * then we *won't* merge adjacent text-nodes
	    * (except in xsl:text); this will ensure that whitespace-only
	    * text nodes are (incorrectly) not stripped in some cases.
	    * 
	    * Example:               : <foo>  <!-- bar -->zoo</foo>
	    * Corrent (strict) result: <foo>  zoo</foo>
	    * Incorrect (old) result : <foo>zoo</foo>
	    *    
	    * NOTE that we *will* merge adjacent text-nodes if
	    * they are in xsl:text.
	    * Example, the following:
	    * <xsl:text>  <!-- bar -->zoo<xsl:text>
	    * will result in both cases in:
	    * <xsl:text>  zoo<xsl:text>
	    */
	    cur->type = XML_TEXT_NODE;
	    if ((strictWhitespace != 0) || (inXSLText != 0)) {
		/*
		* New behaviour; merge nodes.
		*/
		if (textNode == NULL)
		    textNode = cur;
		else {
		    if (cur->content != NULL)
			xmlNodeAddContent(textNode, cur->content);
		    deleteNode = cur;
		}
		if ((cur->next == NULL) ||
		    (cur->next->type == XML_ELEMENT_NODE))
		    goto end_of_text;
		else
		    goto next_sibling;
	    } else {
		/*
		* Old behaviour.
		*/
		if (textNode == NULL)
		    textNode = cur;
		goto end_of_text;
	    }	    	   
	} else if ((cur->type == XML_COMMENT_NODE) ||
	    (cur->type == XML_PI_NODE))
	{	    
	    /*
	    * Remove processing instructions and comments.
	    */
	    deleteNode = cur;
	    if ((cur->next == NULL) ||
		(cur->next->type == XML_ELEMENT_NODE))
		goto end_of_text;
	    else
		goto next_sibling;
	} else {
	    textNode = NULL;
	    /*
	    * Invalid node-type for this data-model.
	    */
	    xsltTransformError(NULL, style, cur,
		"Invalid type of node for the XSLT data model.\n");
	    cctxt->style->errors++;
	    goto next_sibling;
	}

end_of_text:
	if (textNode) {
	    value = textNode->content;
	    /*
	    * At this point all adjacent text/CDATA-section nodes
	    * have been merged.
	    *
	    * Strip whitespace-only text-nodes.
	    * (cctxt->inode->stripWhitespace)
	    */
	    if ((value == NULL) || (*value == 0) ||
		(((cctxt->inode->stripWhitespace) ||
		  (! cctxt->inode->preserveWhitespace)) &&
		 IS_BLANK(*value) &&
		 xsltIsBlank(value)))
	    {		
		if (textNode != cur) {
		    xmlUnlinkNode(textNode);
		    xmlFreeNode(textNode);
		} else
		    deleteNode = textNode;
		textNode = NULL;
		goto next_sibling;
	    }
	    /*
	    * Convert CDATA-section nodes to text-nodes.
	    * TODO: Can this produce problems?
	    */
	    if (textNode->type != XML_TEXT_NODE) {
		textNode->type = XML_TEXT_NODE;
		textNode->name = xmlStringText;
	    }
	    if (internalize &&
		(textNode->content != NULL) &&
		(!xmlDictOwns(style->dict, textNode->content)))
	    {
		/*
		* Internalize the string.
		*/
		value = (xmlChar *) xmlDictLookup(style->dict,
		    textNode->content, -1);
		xmlNodeSetContent(textNode, NULL);
		textNode->content = value;
	    }
	    textNode = NULL;
	    /*
	    * Note that "disable-output-escaping" of the xsl:text
	    * element will be applied at a later level, when
	    * XSLT elements are processed.
	    */
	}

next_sibling:
	if (cur->type == XML_ELEMENT_NODE) {
	    xsltCompilerNodePop(cctxt, cur);
	}
	if (cur == node)
	    break;
	if (cur->next != NULL) {
	    cur = cur->next;
	} else {
	    cur = cur->parent;
	    inXSLText = 0;
	    goto next_sibling;
	};
    }
    if (deleteNode != NULL) {
#ifdef WITH_XSLT_DEBUG_PARSING
	xsltGenericDebug(xsltGenericDebugContext,
	 "xsltParsePreprocessStylesheetTree: removing node\n");
#endif
	xmlUnlinkNode(deleteNode);
	xmlFreeNode(deleteNode);
    }
    return(0);

internal_err:
    return(-1);
}