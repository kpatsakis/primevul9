xsltCompilerNodePop(xsltCompilerCtxtPtr cctxt, xmlNodePtr node)
{    
    if (cctxt->inode == NULL) {
	xmlGenericError(xmlGenericErrorContext,
	    "xsltCompilerNodePop: Top-node mismatch.\n");
	return;
    }
    /*
    * NOTE: Be carefull with the @node, since it might be
    *  a doc-node.
    */
    if (cctxt->inode->node != node) {
	xmlGenericError(xmlGenericErrorContext,
	"xsltCompilerNodePop: Node mismatch.\n");
	goto mismatch;
    }
    if (cctxt->inode->depth != cctxt->depth) {
	xmlGenericError(xmlGenericErrorContext,
	"xsltCompilerNodePop: Depth mismatch.\n");
	goto mismatch;
    }
    /*
    * Pop information of variables.
    */
    if ((cctxt->ivar) && (cctxt->ivar->depth > cctxt->depth))
	xsltCompilerVarInfoPop(cctxt);

    cctxt->depth--;
    cctxt->inode = cctxt->inode->prev;
    if (cctxt->inode != NULL)
	cctxt->inode->curChildType = 0;
    return;

mismatch:
    {
	const xmlChar *nsName = NULL, *name = NULL;
	const xmlChar *infnsName = NULL, *infname = NULL;
	
	if (node) {
	    if (node->type == XML_ELEMENT_NODE) {
		name = node->name;
		if (node->ns != NULL)
		    nsName = node->ns->href;
		else
		    nsName = BAD_CAST "";
	    } else {
		name = BAD_CAST "#document";
		nsName = BAD_CAST "";
	    }
	} else
	    name = BAD_CAST "Not given";

	if (cctxt->inode->node) {
	    if (node->type == XML_ELEMENT_NODE) {
		infname = cctxt->inode->node->name;
		if (cctxt->inode->node->ns != NULL)
		    infnsName = cctxt->inode->node->ns->href;
		else
		    infnsName = BAD_CAST "";
	    } else {
		infname = BAD_CAST "#document";
		infnsName = BAD_CAST "";
	    }
	} else
	    infname = BAD_CAST "Not given";

	
	xmlGenericError(xmlGenericErrorContext,
	    "xsltCompilerNodePop: Given   : '%s' URI '%s'\n",
	    name, nsName);
	xmlGenericError(xmlGenericErrorContext,
	    "xsltCompilerNodePop: Expected: '%s' URI '%s'\n",
	    infname, infnsName);
    }
}