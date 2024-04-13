xmlXIncludeGetProp(xmlXIncludeCtxtPtr ctxt, xmlNodePtr cur,
                   const xmlChar *name) {
    xmlChar *ret;

    ret = xmlGetNsProp(cur, XINCLUDE_NS, name);
    if (ret != NULL)
        return(ret);
    if (ctxt->legacy != 0) {
	ret = xmlGetNsProp(cur, XINCLUDE_OLD_NS, name);
	if (ret != NULL)
	    return(ret);
    }
    ret = xmlGetProp(cur, name);
    return(ret);
}