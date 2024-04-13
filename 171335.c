xsltLREInfoCreate(xsltCompilerCtxtPtr cctxt,
		  xmlNodePtr elem,
		  int isLRE)
{
    xsltStyleItemLRElementInfoPtr item;

    if ((cctxt == NULL) || (cctxt->inode == NULL))
	return(-1);

    item = (xsltStyleItemLRElementInfoPtr)
	xmlMalloc(sizeof(xsltStyleItemLRElementInfo));
    if (item == NULL) {
	xsltTransformError(NULL, cctxt->style, NULL,
	    "Internal error in xsltLREInfoCreate(): "
	    "memory allocation failed.\n");
	cctxt->style->errors++;
	return(-1);
    }
    memset(item, 0, sizeof(xsltStyleItemLRElementInfo));
    item->type = XSLT_FUNC_LITERAL_RESULT_ELEMENT;
    /*
    * Store it in the stylesheet.
    */
    item->next = cctxt->style->preComps;
    cctxt->style->preComps = (xsltElemPreCompPtr) item;
    /*
    * @inScopeNs are used for execution of XPath expressions
    *  in AVTs.
    */
    item->inScopeNs = cctxt->inode->inScopeNs;
    
    if (elem)
	xsltLREBuildEffectiveNsNodes(cctxt, item, elem, isLRE);

    cctxt->inode->litResElemInfo = item;
    cctxt->inode->nsChanged = 0;
    cctxt->maxLREs++;
    return(0);
}