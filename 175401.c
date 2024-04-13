xmlXPathCastNodeSetToNumber (xmlNodeSetPtr ns) {
    xmlChar *str;
    double ret;

    if (ns == NULL)
	return(NAN);
    str = xmlXPathCastNodeSetToString(ns);
    ret = xmlXPathCastStringToNumber(str);
    xmlFree(str);
    return(ret);
}