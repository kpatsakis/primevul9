xsltParseExclResultPrefixes(xsltCompilerCtxtPtr cctxt, xmlNodePtr node,
			    xsltPointerListPtr def,
			    int instrCategory)
{    
    xsltPointerListPtr list = NULL;
    xmlChar *value;
    xmlAttrPtr attr;

    if ((cctxt == NULL) || (node == NULL))
	return(NULL);

    if (instrCategory == XSLT_ELEMENT_CATEGORY_XSLT)
	attr = xmlHasNsProp(node, BAD_CAST "exclude-result-prefixes", NULL);
    else
	attr = xmlHasNsProp(node, BAD_CAST "exclude-result-prefixes",
	    XSLT_NAMESPACE);
    if (attr == NULL)	
	return(def);

    if (attr && (instrCategory == XSLT_ELEMENT_CATEGORY_LRE)) {
	/*
	* Mark the XSLT attr.
	*/
	attr->psvi = (void *) xsltXSLTAttrMarker;
    }

    if ((attr->children != NULL) &&	
	(attr->children->content != NULL))
	value = attr->children->content;
    else {
	xsltTransformError(NULL, cctxt->style, node,
	    "Attribute 'exclude-result-prefixes': Invalid value.\n");
	cctxt->style->errors++;
	return(def);
    }        

    if (xsltParseNsPrefixList(cctxt, cctxt->tmpList, node,
	BAD_CAST value) != 0)
	goto exit;
    if (cctxt->tmpList->number == 0)	
	goto exit;    
    /*
    * Merge the list with the inherited list.
    */
    list = xsltCompilerUtilsCreateMergedList(def, cctxt->tmpList);
    if (list == NULL)
	goto exit;    
    /*
    * Store the list in the stylesheet/compiler context.
    */
    if (xsltPointerListAddSize(
	cctxt->psData->exclResultNamespaces, list, 5) == -1)
    {
	xsltPointerListFree(list);
	list = NULL;
	goto exit;
    }
    /*
    * Notify of change in status wrt namespaces.
    */
    if (cctxt->inode != NULL)
	cctxt->inode->nsChanged = 1;

exit:    
    if (list != NULL)
	return(list);
    else
	return(def);
}