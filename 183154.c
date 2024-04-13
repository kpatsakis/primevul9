xmlCopyDoc(xmlDocPtr doc, int recursive) {
    xmlDocPtr ret;

    if (doc == NULL) return(NULL);
    ret = xmlNewDoc(doc->version);
    if (ret == NULL) return(NULL);
    if (doc->name != NULL)
        ret->name = xmlMemStrdup(doc->name);
    if (doc->encoding != NULL)
        ret->encoding = xmlStrdup(doc->encoding);
    if (doc->URL != NULL)
        ret->URL = xmlStrdup(doc->URL);
    ret->charset = doc->charset;
    ret->compression = doc->compression;
    ret->standalone = doc->standalone;
    if (!recursive) return(ret);

    ret->last = NULL;
    ret->children = NULL;
#ifdef LIBXML_TREE_ENABLED
    if (doc->intSubset != NULL) {
        ret->intSubset = xmlCopyDtd(doc->intSubset);
	if (ret->intSubset == NULL) {
	    xmlFreeDoc(ret);
	    return(NULL);
	}
	xmlSetTreeDoc((xmlNodePtr)ret->intSubset, ret);
	ret->intSubset->parent = ret;
    }
#endif
    if (doc->oldNs != NULL)
        ret->oldNs = xmlCopyNamespaceList(doc->oldNs);
    if (doc->children != NULL) {
	xmlNodePtr tmp;

	ret->children = xmlStaticCopyNodeList(doc->children, ret,
		                               (xmlNodePtr)ret);
	ret->last = NULL;
	tmp = ret->children;
	while (tmp != NULL) {
	    if (tmp->next == NULL)
	        ret->last = tmp;
	    tmp = tmp->next;
	}
    }
    return(ret);
}