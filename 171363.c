xsltFreePrincipalStylesheetData(xsltPrincipalStylesheetDataPtr data)
{
    if (data == NULL)
	return;

    if (data->inScopeNamespaces != NULL) {
	int i;
	xsltNsListContainerPtr nsi;
	xsltPointerListPtr list =
	    (xsltPointerListPtr) data->inScopeNamespaces;

	for (i = 0; i < list->number; i++) {
	    /*
	    * REVISIT TODO: Free info of in-scope namespaces.
	    */
	    nsi = (xsltNsListContainerPtr) list->items[i];
	    if (nsi->list != NULL)
		xmlFree(nsi->list);
	    xmlFree(nsi);
	}
	xsltPointerListFree(list);
	data->inScopeNamespaces = NULL;
    }

    if (data->exclResultNamespaces != NULL) {
	int i;
	xsltPointerListPtr list = (xsltPointerListPtr)
	    data->exclResultNamespaces;	
	
	for (i = 0; i < list->number; i++)
	    xsltPointerListFree((xsltPointerListPtr) list->items[i]);
	
	xsltPointerListFree(list);
	data->exclResultNamespaces = NULL;
    }

    if (data->extElemNamespaces != NULL) {
	xsltPointerListPtr list = (xsltPointerListPtr)
	    data->extElemNamespaces;
	int i;

	for (i = 0; i < list->number; i++)
	    xsltPointerListFree((xsltPointerListPtr) list->items[i]);

	xsltPointerListFree(list);
	data->extElemNamespaces = NULL;
    }
    if (data->effectiveNs) {
	xsltLREEffectiveNsNodesFree(data->effectiveNs);
	data->effectiveNs = NULL;
    }
#ifdef XSLT_REFACTORED_XSLT_NSCOMP
    xsltFreeNamespaceMap(data->nsMap);
#endif
    xmlFree(data);
}