xsltFreeNamespaceMap(xsltNsMapPtr item)
{
    xsltNsMapPtr tmp;
    
    while (item) {
	tmp = item;
	item = item->next;
	xmlFree(tmp);
    } 
    return;
}