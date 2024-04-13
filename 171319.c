xsltFreeNsAliasList(xsltNsAliasPtr item)
{
    xsltNsAliasPtr tmp;
    
    while (item) {
	tmp = item;
	item = item->next;
	xmlFree(tmp);
    } 
    return;
}