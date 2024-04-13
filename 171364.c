xsltParseSequenceConstructor(xsltCompilerCtxtPtr cctxt, xmlNodePtr cur)
{
    xsltStyleType type;
    xmlNodePtr deleteNode = NULL;

    if (cctxt == NULL) {
	xmlGenericError(xmlGenericErrorContext,
	    "xsltParseSequenceConstructor: Bad arguments\n");
	cctxt->style->errors++;
	return;
    }
    /*
    * Detection of handled content of extension instructions.
    */
    if (cctxt->inode->category == XSLT_ELEMENT_CATEGORY_EXTENSION) {
	cctxt->inode->extContentHandled = 1;
    }
    if (cur == NULL)
	return;
    /*
    * This is the content reffered to as a "template".
    * E.g. an xsl:element has such content model:
    * <xsl:element
    *   name = { qname }
    *   namespace = { uri-reference }
    *   use-attribute-sets = qnames>
    * <!-- Content: template -->
    *
    * NOTE that in XSLT-2 the term "template" was abandoned due to
    *  confusion with xsl:template and the term "sequence constructor"
    *  was introduced instead.
    *
    * The following XSLT-instructions are allowed to appear:
    *  xsl:apply-templates, xsl:call-template, xsl:apply-imports,
    *  xsl:for-each, xsl:value-of, xsl:copy-of, xsl:number,
    *  xsl:choose, xsl:if, xsl:text, xsl:copy, xsl:variable,
    *  xsl:message, xsl:fallback,
    *  xsl:processing-instruction, xsl:comment, xsl:element
    *  xsl:attribute. 
    * Additional allowed content:
    * 1) extension instructions
    * 2) literal result elements
    * 3) PCDATA
    *
    * NOTE that this content model does *not* allow xsl:param.
    */    
    while (cur != NULL) {
	if (deleteNode != NULL)	{
#ifdef WITH_XSLT_DEBUG_BLANKS
	    xsltGenericDebug(xsltGenericDebugContext,
	     "xsltParseSequenceConstructor: removing xsl:text element\n");
#endif
	    xmlUnlinkNode(deleteNode);
	    xmlFreeNode(deleteNode);
	    deleteNode = NULL;
	}
	if (cur->type == XML_ELEMENT_NODE) {	    
	    
	    if (cur->psvi == xsltXSLTTextMarker) {
		/*
		* xsl:text elements
		* --------------------------------------------------------
		*/
		xmlNodePtr tmp;

		cur->psvi = NULL;
		/*
		* Mark the xsl:text element for later deletion.
		*/
		deleteNode = cur;
		/*
		* Validate content.
		*/
		tmp = cur->children;
		if (tmp) {
		    /*
		    * We don't expect more than one text-node in the
		    * content, since we already merged adjacent
		    * text/CDATA-nodes and eliminated PI/comment-nodes.
		    */
		    if ((tmp->type == XML_TEXT_NODE) ||
			(tmp->next == NULL))
		    {
			/*
			* Leave the contained text-node in the tree.
			*/
			xmlUnlinkNode(tmp);
			xmlAddPrevSibling(cur, tmp);
		    } else {
			tmp = NULL;
			xsltTransformError(NULL, cctxt->style, cur,
			    "Element 'xsl:text': Invalid type "
			    "of node found in content.\n");
			cctxt->style->errors++;
		    } 
		}
		if (cur->properties) {
		    xmlAttrPtr attr;
		    /*
		    * TODO: We need to report errors for
		    *  invalid attrs.
		    */
		    attr = cur->properties;
		    do {
			if ((attr->ns == NULL) &&
			    (attr->name != NULL) &&
			    (attr->name[0] == 'd') &&
			    xmlStrEqual(attr->name,
			    BAD_CAST "disable-output-escaping"))
			{
			    /*
			    * Attr "disable-output-escaping".
			    * XSLT-2: This attribute is deprecated.
			    */
			    if ((attr->children != NULL) &&
				xmlStrEqual(attr->children->content,
				BAD_CAST "yes"))
			    {
				/*
				* Disable output escaping for this
				* text node.
				*/
				if (tmp)
				    tmp->name = xmlStringTextNoenc;
			    } else if ((attr->children == NULL) ||
				(attr->children->content == NULL) ||
				(!xmlStrEqual(attr->children->content,
				BAD_CAST "no")))
			    {
				xsltTransformError(NULL, cctxt->style,
				    cur,
				    "Attribute 'disable-output-escaping': "
				    "Invalid value. Expected is "
				    "'yes' or 'no'.\n");
				cctxt->style->errors++;
			    }
			    break;
			}
			attr = attr->next;
		    } while (attr != NULL);
		}
	    } else if (IS_XSLT_ELEM_FAST(cur)) {
		/*
		* TODO: Using the XSLT-marker is still not stable yet.
		*/
		/* if (cur->psvi == xsltXSLTElemMarker) { */	    
		/*
		* XSLT instructions
		* --------------------------------------------------------
		*/
		cur->psvi = NULL;
		type = xsltGetXSLTElementTypeByNode(cctxt, cur);
		switch (type) {
		    case XSLT_FUNC_APPLYIMPORTS:
		    case XSLT_FUNC_APPLYTEMPLATES:
		    case XSLT_FUNC_ATTRIBUTE:
		    case XSLT_FUNC_CALLTEMPLATE:
		    case XSLT_FUNC_CHOOSE:
		    case XSLT_FUNC_COMMENT:
		    case XSLT_FUNC_COPY:
		    case XSLT_FUNC_COPYOF:
		    case XSLT_FUNC_DOCUMENT: /* Extra one */
		    case XSLT_FUNC_ELEMENT:
		    case XSLT_FUNC_FALLBACK:
		    case XSLT_FUNC_FOREACH:
		    case XSLT_FUNC_IF:
		    case XSLT_FUNC_MESSAGE:
		    case XSLT_FUNC_NUMBER:
		    case XSLT_FUNC_PI:
		    case XSLT_FUNC_TEXT:
		    case XSLT_FUNC_VALUEOF:
		    case XSLT_FUNC_VARIABLE:
			/*
			* Parse the XSLT element.
			*/
			cctxt->inode->curChildType = type;
			xsltParseAnyXSLTElem(cctxt, cur);
			break;
		    default:
			xsltParseUnknownXSLTElem(cctxt, cur);			
			cur = cur->next;
			continue;
		}
	    } else {
		/*
		* Non-XSLT elements
		* -----------------
		*/
		xsltCompilerNodePush(cctxt, cur);
		/*
		* Update the in-scope namespaces if needed.
		*/
		if (cur->nsDef != NULL)
		    cctxt->inode->inScopeNs =
			xsltCompilerBuildInScopeNsList(cctxt, cur);
		/*
		* The current element is either a literal result element
		* or an extension instruction.
		*
		* Process attr "xsl:extension-element-prefixes".
		* FUTURE TODO: IIRC in XSLT 2.0 this attribute must be
		* processed by the implementor of the extension function;
		* i.e., it won't be handled by the XSLT processor.
		*/
		/* SPEC 1.0:
		*   "exclude-result-prefixes" is only allowed on literal
		*   result elements and "xsl:exclude-result-prefixes"
		*   on xsl:stylesheet/xsl:transform.
		* SPEC 2.0:
		*   "There are a number of standard attributes
		*   that may appear on any XSLT element: specifically
		*   version, exclude-result-prefixes,
		*   extension-element-prefixes, xpath-default-namespace,
		*   default-collation, and use-when."
		*
		* SPEC 2.0:
		*   For literal result elements:
		*   "xsl:version, xsl:exclude-result-prefixes,
		*    xsl:extension-element-prefixes,
		*    xsl:xpath-default-namespace,
		*    xsl:default-collation, or xsl:use-when."
		*/
		if (cur->properties)
		    cctxt->inode->extElemNs =
			xsltParseExtElemPrefixes(cctxt,
			    cur, cctxt->inode->extElemNs,
			    XSLT_ELEMENT_CATEGORY_LRE);
		/*
		* Eval if we have an extension instruction here.
		*/
		if ((cur->ns != NULL) &&
		    (cctxt->inode->extElemNs != NULL) &&
		    (xsltCheckExtPrefix(cctxt->style, cur->ns->href) == 1))
		{
		    /*
		    * Extension instructions
		    * ----------------------------------------------------
		    * Mark the node information.
		    */
		    cctxt->inode->category = XSLT_ELEMENT_CATEGORY_EXTENSION;
		    cctxt->inode->extContentHandled = 0;
		    if (cur->psvi != NULL) {
			cur->psvi = NULL;
			/*
			* TODO: Temporary sanity check.
			*/
			xsltTransformError(NULL, cctxt->style, cur,
			    "Internal error in xsltParseSequenceConstructor(): "
			    "Occupied PSVI field.\n");
			cctxt->style->errors++;
			cur = cur->next;
			continue;
		    }
		    cur->psvi = (void *)
			xsltPreComputeExtModuleElement(cctxt->style, cur);
		    
		    if (cur->psvi == NULL) {
			/*
			* OLD COMMENT: "Unknown element, maybe registered
			*  at the context level. Mark it for later
			*  recognition."
			* QUESTION: What does the xsltExtMarker mean?
			*  ANSWER: It is used in
			*   xsltApplySequenceConstructor() at
			*   transformation-time to look out for extension
			*   registered in the transformation context.
			*/
			cur->psvi = (void *) xsltExtMarker;
		    }
		    /*
		    * BIG NOTE: Now the ugly part. In previous versions
		    *  of Libxslt (until 1.1.16), all the content of an
		    *  extension instruction was processed and compiled without
		    *  the need of the extension-author to explicitely call
		    *  such a processing;.We now need to mimic this old
		    *  behaviour in order to avoid breaking old code
		    *  on the extension-author's side.
		    * The mechanism:
		    *  1) If the author does *not* set the
		    *    compile-time-flag @extContentHandled, then we'll
		    *    parse the content assuming that it's a "template"
		    *    (or "sequence constructor in XSLT 2.0 terms).
		    *    NOTE: If the extension is registered at
		    *    transformation-time only, then there's no way of
		    *    knowing that content shall be valid, and we'll
		    *    process the content the same way.
		    *  2) If the author *does* set the flag, then we'll assume
		    *   that the author has handled the parsing him/herself
		    *   (e.g. called xsltParseSequenceConstructor(), etc.
		    *   explicitely in his/her code).
		    */
		    if ((cur->children != NULL) &&
			(cctxt->inode->extContentHandled == 0))
		    {
			/*
			* Default parsing of the content using the
			* sequence-constructor model.
			*/
			xsltParseSequenceConstructor(cctxt, cur->children);
		    }
		} else {
		    /*
		    * Literal result element
		    * ----------------------------------------------------
		    * Allowed XSLT attributes:
		    *  xsl:extension-element-prefixes CDATA #IMPLIED
		    *  xsl:exclude-result-prefixes CDATA #IMPLIED
		    *  TODO: xsl:use-attribute-sets %qnames; #IMPLIED
		    *  xsl:version NMTOKEN #IMPLIED
		    */
		    cur->psvi = NULL;
		    cctxt->inode->category = XSLT_ELEMENT_CATEGORY_LRE;
		    if (cur->properties != NULL) {
			xmlAttrPtr attr = cur->properties;
			/*
			* Attribute "xsl:exclude-result-prefixes".
			*/
			cctxt->inode->exclResultNs =
			    xsltParseExclResultPrefixes(cctxt, cur,
				cctxt->inode->exclResultNs,
				XSLT_ELEMENT_CATEGORY_LRE);
			/*
			* Attribute "xsl:version".
			*/
			xsltParseAttrXSLTVersion(cctxt, cur,
			    XSLT_ELEMENT_CATEGORY_LRE);
			/*
			* Report invalid XSLT attributes.			
			* For XSLT 1.0 only xsl:use-attribute-sets is allowed
			* next to xsl:version, xsl:exclude-result-prefixes and
			* xsl:extension-element-prefixes.
			*
			* Mark all XSLT attributes, in order to skip such
			* attributes when instantiating the LRE.
			*/
			do {
			    if ((attr->psvi != xsltXSLTAttrMarker) &&
				IS_XSLT_ATTR_FAST(attr))
			    {				    
				if (! xmlStrEqual(attr->name,
				    BAD_CAST "use-attribute-sets"))
				{				
				    xsltTransformError(NULL, cctxt->style,
					cur,
					"Unknown XSLT attribute '%s'.\n",
					attr->name);
				    cctxt->style->errors++;
				} else {
				    /*
				    * XSLT attr marker.
				    */
				    attr->psvi = (void *) xsltXSLTAttrMarker;
				}
			    }
			    attr = attr->next;
			} while (attr != NULL);
		    }
		    /*
		    * Create/reuse info for the literal result element.
		    */
		    if (cctxt->inode->nsChanged)
			xsltLREInfoCreate(cctxt, cur, 1);
		    cur->psvi = cctxt->inode->litResElemInfo;
		    /*
		    * Apply ns-aliasing on the element and on its attributes.
		    */
		    if (cctxt->hasNsAliases)
			xsltLREBuildEffectiveNs(cctxt, cur);
		    /*
		    * Compile attribute value templates (AVT).
		    */
		    if (cur->properties) {
			xmlAttrPtr attr = cur->properties;
			
			while (attr != NULL) {
			    xsltCompileAttr(cctxt->style, attr);
			    attr = attr->next;
			}
		    }
		    /*
		    * Parse the content, which is defined to be a "template"
		    * (or "sequence constructor" in XSLT 2.0 terms).
		    */
		    if (cur->children != NULL) {
			xsltParseSequenceConstructor(cctxt, cur->children);
		    }
		}
		/*
		* Leave the non-XSLT element.
		*/
		xsltCompilerNodePop(cctxt, cur);
	    }
	}
	cur = cur->next;
    }
    if (deleteNode != NULL) {
#ifdef WITH_XSLT_DEBUG_BLANKS
	xsltGenericDebug(xsltGenericDebugContext,
	    "xsltParseSequenceConstructor: removing xsl:text element\n");
#endif
	xmlUnlinkNode(deleteNode);
	xmlFreeNode(deleteNode);
	deleteNode = NULL;
    }
}