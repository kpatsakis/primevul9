xsltParseAnyXSLTElem(xsltCompilerCtxtPtr cctxt, xmlNodePtr elem)
{
    if ((cctxt == NULL) || (elem == NULL) ||
	(elem->type != XML_ELEMENT_NODE))
	return(-1);

    elem->psvi = NULL;

    if (! (IS_XSLT_ELEM_FAST(elem)))
	return(-1);
    /*
    * Detection of handled content of extension instructions.
    */
    if (cctxt->inode->category == XSLT_ELEMENT_CATEGORY_EXTENSION) {
	cctxt->inode->extContentHandled = 1;
    }
    
    xsltCompilerNodePush(cctxt, elem);
    /*
    * URGENT TODO: Find a way to speed up this annoying redundant
    *  textual node-name and namespace comparison.
    */
    if (cctxt->inode->prev->curChildType != 0)
	cctxt->inode->type = cctxt->inode->prev->curChildType;
    else
	cctxt->inode->type = xsltGetXSLTElementTypeByNode(cctxt, elem);    
    /*
    * Update the in-scope namespaces if needed.
    */
    if (elem->nsDef != NULL)
	cctxt->inode->inScopeNs =
	    xsltCompilerBuildInScopeNsList(cctxt, elem);
    /*
    * xsltStylePreCompute():
    *  This will compile the information found on the current
    *  element's attributes. NOTE that this won't process the
    *  children of the instruction.
    */
    xsltStylePreCompute(cctxt->style, elem);
    /*
    * TODO: How to react on errors in xsltStylePreCompute() ?
    */

    /*
    * Validate the content model of the XSLT-element.
    */
    switch (cctxt->inode->type) {	
	case XSLT_FUNC_APPLYIMPORTS:
	    /* EMPTY */
	    goto empty_content;
	case XSLT_FUNC_APPLYTEMPLATES:
	    /* <!-- Content: (xsl:sort | xsl:with-param)* --> */
	    goto apply_templates;	    
	case XSLT_FUNC_ATTRIBUTE:	    
	    /* <!-- Content: template --> */
	    goto sequence_constructor;
	case XSLT_FUNC_CALLTEMPLATE:
	    /* <!-- Content: xsl:with-param* --> */
	    goto call_template;
	case XSLT_FUNC_CHOOSE:	    
	    /* <!-- Content: (xsl:when+, xsl:otherwise?) --> */
	    goto choose;
	case XSLT_FUNC_COMMENT:
	    /* <!-- Content: template --> */
	    goto sequence_constructor;	    
	case XSLT_FUNC_COPY:
	    /* <!-- Content: template --> */
	    goto sequence_constructor;	    
	case XSLT_FUNC_COPYOF:
	    /* EMPTY */
	    goto empty_content;	   
	case XSLT_FUNC_DOCUMENT: /* Extra one */
	    /* ?? template ?? */
	    goto sequence_constructor;
	case XSLT_FUNC_ELEMENT:
	    /* <!-- Content: template --> */
	    goto sequence_constructor;
	case XSLT_FUNC_FALLBACK:
	    /* <!-- Content: template --> */
	    goto sequence_constructor;
	case XSLT_FUNC_FOREACH:
	    /* <!-- Content: (xsl:sort*, template) --> */
	    goto for_each;
	case XSLT_FUNC_IF:
	    /* <!-- Content: template --> */
	    goto sequence_constructor;
	case XSLT_FUNC_OTHERWISE:
	    /* <!-- Content: template --> */
	    goto sequence_constructor;
	case XSLT_FUNC_MESSAGE:
	    /* <!-- Content: template --> */
	    goto sequence_constructor;
	case XSLT_FUNC_NUMBER:
	    /* EMPTY */
	    goto empty_content;
	case XSLT_FUNC_PARAM:
	    /*
	    * Check for redefinition.
	    */
	    if ((elem->psvi != NULL) && (cctxt->ivar != NULL)) {
		xsltVarInfoPtr ivar = cctxt->ivar;

		do {
		    if ((ivar->name ==
			 ((xsltStyleItemParamPtr) elem->psvi)->name) &&
			(ivar->nsName ==
			 ((xsltStyleItemParamPtr) elem->psvi)->ns))
		    {
			elem->psvi = NULL;
			xsltTransformError(NULL, cctxt->style, elem,
			    "Redefinition of variable or parameter '%s'.\n",
			    ivar->name);
			cctxt->style->errors++;
			goto error;
		    }
		    ivar = ivar->prev;
		} while (ivar != NULL);
	    }
	    /*  <!-- Content: template --> */
	    goto sequence_constructor;
	case XSLT_FUNC_PI:
	    /*  <!-- Content: template --> */
	    goto sequence_constructor;
	case XSLT_FUNC_SORT:
	    /* EMPTY */
	    goto empty_content;
	case XSLT_FUNC_TEXT:
	    /* <!-- Content: #PCDATA --> */
	    goto text;
	case XSLT_FUNC_VALUEOF:
	    /* EMPTY */
	    goto empty_content;
	case XSLT_FUNC_VARIABLE:
	    /*
	    * Check for redefinition.
	    */
	    if ((elem->psvi != NULL) && (cctxt->ivar != NULL)) {
		xsltVarInfoPtr ivar = cctxt->ivar;		

		do {
		    if ((ivar->name ==
			 ((xsltStyleItemVariablePtr) elem->psvi)->name) &&
			(ivar->nsName ==
			 ((xsltStyleItemVariablePtr) elem->psvi)->ns))
		    {
			elem->psvi = NULL;
			xsltTransformError(NULL, cctxt->style, elem,
			    "Redefinition of variable or parameter '%s'.\n",
			    ivar->name);
			cctxt->style->errors++;
			goto error;
		    }
		    ivar = ivar->prev;
		} while (ivar != NULL);
	    }
	    /* <!-- Content: template --> */
	    goto sequence_constructor;
	case XSLT_FUNC_WHEN:
	    /* <!-- Content: template --> */
	    goto sequence_constructor;
	case XSLT_FUNC_WITHPARAM:
	    /* <!-- Content: template --> */
	    goto sequence_constructor;
	default:
#ifdef WITH_XSLT_DEBUG_PARSING
	    xsltGenericDebug(xsltGenericDebugContext,
		"xsltParseXSLTNode: Unhandled XSLT element '%s'.\n",
		elem->name);	    
#endif
	    xsltTransformError(NULL, cctxt->style, elem,
		"xsltParseXSLTNode: Internal error; "
		"unhandled XSLT element '%s'.\n", elem->name);
	    cctxt->style->errors++;
	    goto internal_err;
    }

apply_templates:
    /* <!-- Content: (xsl:sort | xsl:with-param)* --> */
    if (elem->children != NULL) {
	xmlNodePtr child = elem->children;
	do {
	    if (child->type == XML_ELEMENT_NODE) {
		if (IS_XSLT_ELEM_FAST(child)) {
		    if (xmlStrEqual(child->name, BAD_CAST "with-param")) {
			cctxt->inode->curChildType = XSLT_FUNC_WITHPARAM;
			xsltParseAnyXSLTElem(cctxt, child);
		    } else if (xmlStrEqual(child->name, BAD_CAST "sort")) {
			cctxt->inode->curChildType = XSLT_FUNC_SORT;
			xsltParseAnyXSLTElem(cctxt, child);
		    } else
			xsltParseContentError(cctxt->style, child);
		} else
		    xsltParseContentError(cctxt->style, child);
	    }
	    child = child->next;
	} while (child != NULL);
    }    
    goto exit;

call_template:
    /* <!-- Content: xsl:with-param* --> */
    if (elem->children != NULL) {
	xmlNodePtr child = elem->children;
	do {
	    if (child->type == XML_ELEMENT_NODE) {
		if (IS_XSLT_ELEM_FAST(child)) {
		    xsltStyleType type;

		    type = xsltGetXSLTElementTypeByNode(cctxt, child);
		    if (type == XSLT_FUNC_WITHPARAM) {
			cctxt->inode->curChildType = XSLT_FUNC_WITHPARAM;
			xsltParseAnyXSLTElem(cctxt, child);
		    } else {
			xsltParseContentError(cctxt->style, child);
		    }
		} else
		    xsltParseContentError(cctxt->style, child);
	    }
	    child = child->next;
	} while (child != NULL);
    }    
    goto exit;

text:
    if (elem->children != NULL) {
	xmlNodePtr child = elem->children;
	do {
	    if ((child->type != XML_TEXT_NODE) &&
		(child->type != XML_CDATA_SECTION_NODE))
	    {
		xsltTransformError(NULL, cctxt->style, elem,
		    "The XSLT 'text' element must have only character "
		    "data as content.\n");
	    }
	    child = child->next;
	} while (child != NULL);
    }
    goto exit;

empty_content:
    if (elem->children != NULL) {
	xmlNodePtr child = elem->children;
	/*
	* Relaxed behaviour: we will allow whitespace-only text-nodes.
	*/
	do {
	    if (((child->type != XML_TEXT_NODE) &&
		 (child->type != XML_CDATA_SECTION_NODE)) ||
		(! IS_BLANK_NODE(child)))
	    {
		xsltTransformError(NULL, cctxt->style, elem,
		    "This XSLT element must have no content.\n");
		cctxt->style->errors++;
		break;
	    }
	    child = child->next;
	} while (child != NULL);		
    }
    goto exit;

choose:
    /* <!-- Content: (xsl:when+, xsl:otherwise?) --> */
    /*
    * TODO: text-nodes in between are *not* allowed in XSLT 1.0.
    *   The old behaviour did not check this.
    * NOTE: In XSLT 2.0 they are stripped beforehand
    *  if whitespace-only (regardless of xml:space).
    */
    if (elem->children != NULL) {
	xmlNodePtr child = elem->children;
	int nbWhen = 0, nbOtherwise = 0, err = 0;
	do {
	    if (child->type == XML_ELEMENT_NODE) {
		if (IS_XSLT_ELEM_FAST(child)) {
		    xsltStyleType type;
		
		    type = xsltGetXSLTElementTypeByNode(cctxt, child);
		    if (type == XSLT_FUNC_WHEN) {
			nbWhen++;
			if (nbOtherwise) {
			    xsltParseContentError(cctxt->style, child);
			    err = 1;
			    break;
			}
			cctxt->inode->curChildType = XSLT_FUNC_WHEN;
			xsltParseAnyXSLTElem(cctxt, child);
		    } else if (type == XSLT_FUNC_OTHERWISE) {
			if (! nbWhen) {
			    xsltParseContentError(cctxt->style, child);
			    err = 1;
			    break;
			}			
			if (nbOtherwise) {
			    xsltTransformError(NULL, cctxt->style, elem,
				"The XSLT 'choose' element must not contain "
				"more than one XSLT 'otherwise' element.\n");
			    cctxt->style->errors++;
			    err = 1;
			    break;
			}
			nbOtherwise++;
			cctxt->inode->curChildType = XSLT_FUNC_OTHERWISE;
			xsltParseAnyXSLTElem(cctxt, child);
		    } else
			xsltParseContentError(cctxt->style, child);
		} else
		    xsltParseContentError(cctxt->style, child);
	    } 
	    /*
		else
		    xsltParseContentError(cctxt, child);
	    */
	    child = child->next;
	} while (child != NULL);
	if ((! err) && (! nbWhen)) {
	    xsltTransformError(NULL, cctxt->style, elem,
		"The XSLT element 'choose' must contain at least one "
		"XSLT element 'when'.\n");
		cctxt->style->errors++;
	}	
    }    
    goto exit;

for_each:
    /* <!-- Content: (xsl:sort*, template) --> */
    /*
    * NOTE: Text-nodes before xsl:sort are *not* allowed in XSLT 1.0.
    *   The old behaviour did not allow this, but it catched this
    *   only at transformation-time.
    *   In XSLT 2.0 they are stripped beforehand if whitespace-only
    *   (regardless of xml:space).
    */
    if (elem->children != NULL) {
	xmlNodePtr child = elem->children;
	/*
	* Parse xsl:sort first.
	*/
	do {	    
	    if ((child->type == XML_ELEMENT_NODE) &&
		IS_XSLT_ELEM_FAST(child))
	    {		
		if (xsltGetXSLTElementTypeByNode(cctxt, child) ==
		    XSLT_FUNC_SORT)
		{		
		    cctxt->inode->curChildType = XSLT_FUNC_SORT;
		    xsltParseAnyXSLTElem(cctxt, child);
		} else
		    break;
	    } else
		break;
	    child = child->next;
	} while (child != NULL);
	/*
	* Parse the sequece constructor.
	*/
	if (child != NULL)
	    xsltParseSequenceConstructor(cctxt, child);
    }    
    goto exit;

sequence_constructor:
    /*
    * Parse the sequence constructor.
    */
    if (elem->children != NULL)
	xsltParseSequenceConstructor(cctxt, elem->children);

    /*
    * Register information for vars/params. Only needed if there
    * are any following siblings.
    */
    if ((elem->next != NULL) &&
	((cctxt->inode->type == XSLT_FUNC_VARIABLE) ||
	 (cctxt->inode->type == XSLT_FUNC_PARAM)))
    {	
	if ((elem->psvi != NULL) &&
	    (((xsltStyleBasicItemVariablePtr) elem->psvi)->name))
	{	
	    xsltCompilerVarInfoPush(cctxt, elem,
		((xsltStyleBasicItemVariablePtr) elem->psvi)->name,
		((xsltStyleBasicItemVariablePtr) elem->psvi)->ns);
	}
    }

error:
exit:
    xsltCompilerNodePop(cctxt, elem);
    return(0);

internal_err:
    xsltCompilerNodePop(cctxt, elem);
    return(-1);
}