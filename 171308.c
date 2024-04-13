xsltParseTopLevelXSLTElem(xsltCompilerCtxtPtr cctxt,
			  xmlNodePtr node,
			  xsltStyleType type)
{
    int ret = 0;

    /*
    * TODO: The reason why this function exists:
    *  due to historical reasons some of the
    *  top-level declarations are processed by functions
    *  in other files. Since we need still to set
    *  up the node-info and generate information like
    *  in-scope namespaces, this is a wrapper around
    *  those old parsing functions.
    */
    xsltCompilerNodePush(cctxt, node);
    if (node->nsDef != NULL)
	cctxt->inode->inScopeNs =
	    xsltCompilerBuildInScopeNsList(cctxt, node);
    cctxt->inode->type = type;

    switch (type) {
	case XSLT_FUNC_INCLUDE:
	    {
		int oldIsInclude;

		if (xsltCompileXSLTIncludeElem(cctxt, node) == NULL)
		    goto exit;		
		/*
		* Mark this stylesheet tree as being currently included.
		*/
		oldIsInclude = cctxt->isInclude;
		cctxt->isInclude = 1;
						
		if (xsltParseStylesheetInclude(cctxt->style, node) != 0) {
		    cctxt->style->errors++;
		}
		cctxt->isInclude = oldIsInclude;
	    }
	    break;
	case XSLT_FUNC_PARAM:
	    xsltStylePreCompute(cctxt->style, node);
	    xsltParseGlobalParam(cctxt->style, node);
	    break;
	case XSLT_FUNC_VARIABLE:
	    xsltStylePreCompute(cctxt->style, node);
	    xsltParseGlobalVariable(cctxt->style, node);
	    break;
	case XSLT_FUNC_ATTRSET:
	    xsltParseStylesheetAttributeSet(cctxt->style, node);
	    break;
	default:
	    xsltTransformError(NULL, cctxt->style, node,
		"Internal error: (xsltParseTopLevelXSLTElem) "
		"Cannot handle this top-level declaration.\n");
	    cctxt->style->errors++;
	    ret = -1;
    }

exit:
    xsltCompilerNodePop(cctxt, node);

    return(ret);
}