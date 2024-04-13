xmlXPathNodeSetAdd(xmlNodeSetPtr cur, xmlNodePtr val) {
    int i;

    if ((cur == NULL) || (val == NULL)) return(-1);

    /* @@ with_ns to check whether namespace nodes should be looked at @@ */
    /*
     * prevent duplcates
     */
    for (i = 0;i < cur->nodeNr;i++)
        if (cur->nodeTab[i] == val) return(0);

    /*
     * grow the nodeTab if needed
     */
    if (cur->nodeMax == 0) {
        cur->nodeTab = (xmlNodePtr *) xmlMalloc(XML_NODESET_DEFAULT *
					     sizeof(xmlNodePtr));
	if (cur->nodeTab == NULL) {
	    xmlXPathErrMemory(NULL, "growing nodeset\n");
	    return(-1);
	}
	memset(cur->nodeTab, 0 ,
	       XML_NODESET_DEFAULT * (size_t) sizeof(xmlNodePtr));
        cur->nodeMax = XML_NODESET_DEFAULT;
    } else if (cur->nodeNr == cur->nodeMax) {
        xmlNodePtr *temp;

        if (cur->nodeMax >= XPATH_MAX_NODESET_LENGTH) {
            xmlXPathErrMemory(NULL, "growing nodeset hit limit\n");
            return(-1);
        }
	temp = (xmlNodePtr *) xmlRealloc(cur->nodeTab, cur->nodeMax * 2 *
				      sizeof(xmlNodePtr));
	if (temp == NULL) {
	    xmlXPathErrMemory(NULL, "growing nodeset\n");
	    return(-1);
	}
        cur->nodeMax *= 2;
	cur->nodeTab = temp;
    }
    if (val->type == XML_NAMESPACE_DECL) {
	xmlNsPtr ns = (xmlNsPtr) val;

	cur->nodeTab[cur->nodeNr++] =
	    xmlXPathNodeSetDupNs((xmlNodePtr) ns->next, ns);
    } else
	cur->nodeTab[cur->nodeNr++] = val;
    return(0);
}