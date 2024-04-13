xsltCompilationCtxtFree(xsltCompilerCtxtPtr cctxt)
{    
    if (cctxt == NULL)
	return;
#ifdef WITH_XSLT_DEBUG_PARSING
    xsltGenericDebug(xsltGenericDebugContext,
	"Freeing compilation context\n");
    xsltGenericDebug(xsltGenericDebugContext,
	"### Max inodes: %d\n", cctxt->maxNodeInfos);
    xsltGenericDebug(xsltGenericDebugContext,
	"### Max LREs  : %d\n", cctxt->maxLREs);
#endif
    /*
    * Free node-infos.
    */
    if (cctxt->inodeList != NULL) {
	xsltCompilerNodeInfoPtr tmp, cur = cctxt->inodeList;
	while (cur != NULL) {
	    tmp = cur;
	    cur = cur->next;
	    xmlFree(tmp);
	}
    }
    if (cctxt->tmpList != NULL)
	xsltPointerListFree(cctxt->tmpList);
#ifdef XSLT_REFACTORED_XPATHCOMP
    if (cctxt->xpathCtxt != NULL)
	xmlXPathFreeContext(cctxt->xpathCtxt);
#endif
    if (cctxt->nsAliases != NULL)
	xsltFreeNsAliasList(cctxt->nsAliases);

    if (cctxt->ivars)
	xsltCompilerVarInfoFree(cctxt);

    xmlFree(cctxt);
}