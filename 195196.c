xmlXIncludeProcessFlagsData(xmlDocPtr doc, int flags, void *data) {
    xmlNodePtr tree;

    if (doc == NULL)
	return(-1);
    tree = xmlDocGetRootElement(doc);
    if (tree == NULL)
	return(-1);
    return(xmlXIncludeProcessTreeFlagsData(tree, flags, data));
}