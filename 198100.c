xmlRegexpExec(xmlRegexpPtr comp, const xmlChar *content) {
    if ((comp == NULL) || (content == NULL))
	return(-1);
    return(xmlFARegExec(comp, content));
}