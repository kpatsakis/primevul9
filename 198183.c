xmlRegFreeRange(xmlRegRangePtr range) {
    if (range == NULL)
	return;

    if (range->blockName != NULL)
	xmlFree(range->blockName);
    xmlFree(range);
}