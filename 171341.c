xsltCompilerUtilsCreateMergedList(xsltPointerListPtr first,
			    xsltPointerListPtr second)
{
    xsltPointerListPtr ret;
    size_t num;

    if (first)
	num = first->number;
    else
	num = 0;
    if (second)
	num += second->number;    
    if (num == 0)
	return(NULL);
    ret = xsltPointerListCreate(num);
    if (ret == NULL)
	return(NULL);
    /*
    * Copy contents.
    */
    if ((first != NULL) &&  (first->number != 0)) {
	memcpy(ret->items, first->items,
	    first->number * sizeof(void *));
	if ((second != NULL) && (second->number != 0))
	    memcpy(ret->items + first->number, second->items,
		second->number * sizeof(void *));
    } else if ((second != NULL) && (second->number != 0))
	memcpy(ret->items, (void *) second->items,
	    second->number * sizeof(void *));
    ret->number = num;
    return(ret);
}