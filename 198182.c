xmlExpNewCtxt(int maxNodes, xmlDictPtr dict) {
    xmlExpCtxtPtr ret;
    int size = 256;

    if (maxNodes <= 4096)
        maxNodes = 4096;

    ret = (xmlExpCtxtPtr) xmlMalloc(sizeof(xmlExpCtxt));
    if (ret == NULL)
        return(NULL);
    memset(ret, 0, sizeof(xmlExpCtxt));
    ret->size = size;
    ret->nbElems = 0;
    ret->maxNodes = maxNodes;
    ret->table = xmlMalloc(size * sizeof(xmlExpNodePtr));
    if (ret->table == NULL) {
        xmlFree(ret);
	return(NULL);
    }
    memset(ret->table, 0, size * sizeof(xmlExpNodePtr));
    if (dict == NULL) {
        ret->dict = xmlDictCreate();
	if (ret->dict == NULL) {
	    xmlFree(ret->table);
	    xmlFree(ret);
	    return(NULL);
	}
    } else {
        ret->dict = dict;
	xmlDictReference(ret->dict);
    }
    return(ret);
}