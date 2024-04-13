xsltParseAttrXSLTVersion(xsltCompilerCtxtPtr cctxt, xmlNodePtr node,			 
			 int instrCategory)
{
    xmlChar *value;
    xmlAttrPtr attr;

    if ((cctxt == NULL) || (node == NULL))
	return(-1);

    if (instrCategory == XSLT_ELEMENT_CATEGORY_XSLT)
	attr = xmlHasNsProp(node, BAD_CAST "version", NULL);
    else
	attr = xmlHasNsProp(node, BAD_CAST "version", XSLT_NAMESPACE);

    if (attr == NULL)	
	return(0);

    attr->psvi = (void *) xsltXSLTAttrMarker;

    if ((attr->children != NULL) &&	
	(attr->children->content != NULL))
	value = attr->children->content;
    else {
	xsltTransformError(NULL, cctxt->style, node,
	    "Attribute 'version': Invalid value.\n");
	cctxt->style->errors++;
	return(1);
    }
    
    if (! xmlStrEqual(value, (const xmlChar *)"1.0")) {
	cctxt->inode->forwardsCompat = 1;
	/*
	* TODO: To what extent do we support the
	*  forwards-compatible mode?
	*/
	/*
	* Report this only once per compilation episode.
	*/
	if (! cctxt->hasForwardsCompat) {
	    cctxt->hasForwardsCompat = 1;
	    cctxt->errSeverity = XSLT_ERROR_SEVERITY_WARNING;
	    xsltTransformError(NULL, cctxt->style, node,
		"Warning: the attribute xsl:version specifies a value "
		"different from '1.0'. Switching to forwards-compatible "
		"mode. Only features of XSLT 1.0 are supported by this "
		"processor.\n");
	    cctxt->style->warnings++;
	    cctxt->errSeverity = XSLT_ERROR_SEVERITY_ERROR;
	}	
    } else {
	cctxt->inode->forwardsCompat = 0;
    }

    if (attr && (instrCategory == XSLT_ELEMENT_CATEGORY_LRE)) {
	/*
	* Set a marker on XSLT attributes.
	*/
	attr->psvi = (void *) xsltXSLTAttrMarker;
    }
    return(1);
}