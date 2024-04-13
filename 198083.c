xmlRegCopyRange(xmlRegParserCtxtPtr ctxt, xmlRegRangePtr range) {
    xmlRegRangePtr ret;

    if (range == NULL)
	return(NULL);

    ret = xmlRegNewRange(ctxt, range->neg, range->type, range->start,
                         range->end);
    if (ret == NULL)
        return(NULL);
    if (range->blockName != NULL) {
	ret->blockName = xmlStrdup(range->blockName);
	if (ret->blockName == NULL) {
	    xmlRegexpErrMemory(ctxt, "allocating range");
	    xmlRegFreeRange(ret);
	    return(NULL);
	}
    }
    return(ret);
}