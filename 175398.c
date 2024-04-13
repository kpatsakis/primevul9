xmlXPathCastNodeToNumber (xmlNodePtr node) {
    xmlChar *strval;
    double ret;

    if (node == NULL)
	return(NAN);
    strval = xmlXPathCastNodeToString(node);
    if (strval == NULL)
	return(NAN);
    ret = xmlXPathCastStringToNumber(strval);
    xmlFree(strval);

    return(ret);
}