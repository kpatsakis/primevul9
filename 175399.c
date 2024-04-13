xmlXPathDistinctSorted (xmlNodeSetPtr nodes) {
    xmlNodeSetPtr ret;
    xmlHashTablePtr hash;
    int i, l;
    xmlChar * strval;
    xmlNodePtr cur;

    if (xmlXPathNodeSetIsEmpty(nodes))
	return(nodes);

    ret = xmlXPathNodeSetCreate(NULL);
    if (ret == NULL)
        return(ret);
    l = xmlXPathNodeSetGetLength(nodes);
    hash = xmlHashCreate (l);
    for (i = 0; i < l; i++) {
	cur = xmlXPathNodeSetItem(nodes, i);
	strval = xmlXPathCastNodeToString(cur);
	if (xmlHashLookup(hash, strval) == NULL) {
	    xmlHashAddEntry(hash, strval, strval);
	    if (xmlXPathNodeSetAddUnique(ret, cur) < 0)
	        break;
	} else {
	    xmlFree(strval);
	}
    }
    xmlHashFree(hash, xmlHashDefaultDeallocator);
    return(ret);
}