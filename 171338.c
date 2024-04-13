xsltCompilerNodePush(xsltCompilerCtxtPtr cctxt, xmlNodePtr node)
{    
    xsltCompilerNodeInfoPtr inode, iprev;

    if ((cctxt->inode != NULL) && (cctxt->inode->next != NULL)) {	
	inode = cctxt->inode->next;
    } else if ((cctxt->inode == NULL) && (cctxt->inodeList != NULL)) {
	inode = cctxt->inodeList;	
    } else {
	/*
	* Create a new node-info.
	*/
	inode = (xsltCompilerNodeInfoPtr)
	    xmlMalloc(sizeof(xsltCompilerNodeInfo));
	if (inode == NULL) {
	    xsltTransformError(NULL, cctxt->style, NULL,
		"xsltCompilerNodePush: malloc failed.\n");
	    return(NULL);
	}
	memset(inode, 0, sizeof(xsltCompilerNodeInfo));
	if (cctxt->inodeList == NULL)
	    cctxt->inodeList = inode;
	else {
	    cctxt->inodeLast->next = inode;
	    inode->prev = cctxt->inodeLast;
	}
	cctxt->inodeLast = inode;
	cctxt->maxNodeInfos++;	
	if (cctxt->inode == NULL) {
	    cctxt->inode = inode;
	    /*
	    * Create an initial literal result element info for
	    * the root of the stylesheet.
	    */
	    xsltLREInfoCreate(cctxt, NULL, 0);
	} 
    }       
    cctxt->depth++;
    cctxt->inode = inode;
    /*
    * REVISIT TODO: Keep the reset always complete.    
    * NOTE: Be carefull with the @node, since it might be
    *  a doc-node.
    */
    inode->node = node;
    inode->depth = cctxt->depth;
    inode->templ = NULL;
    inode->category = XSLT_ELEMENT_CATEGORY_XSLT;
    inode->type = 0;
    inode->item = NULL;
    inode->curChildType = 0;
    inode->extContentHandled = 0;
    inode->isRoot = 0;
    
    if (inode->prev != NULL) {
	iprev = inode->prev;
	/*
	* Inherit the following information:
	* ---------------------------------
	*
	* In-scope namespaces
	*/
	inode->inScopeNs = iprev->inScopeNs;
	/*
	* Info for literal result elements
	*/
	inode->litResElemInfo = iprev->litResElemInfo;
	inode->nsChanged = iprev->nsChanged;
	/*
	* Excluded result namespaces
	*/
	inode->exclResultNs = iprev->exclResultNs;
	/*
	* Extension instruction namespaces
	*/
	inode->extElemNs = iprev->extElemNs;
	/*
	* Whitespace preservation
	*/
	inode->preserveWhitespace = iprev->preserveWhitespace;
	/*
	* Forwards-compatible mode
	*/
	inode->forwardsCompat = iprev->forwardsCompat;	
    } else {
	inode->inScopeNs = NULL;
	inode->exclResultNs = NULL;
	inode->extElemNs = NULL;
	inode->preserveWhitespace = 0;
	inode->forwardsCompat = 0;
    }
    
    return(inode);
}