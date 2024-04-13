xmlDictGetUsage(xmlDictPtr dict) {
    xmlDictStringsPtr pool;
    size_t limit = 0;

    if (dict == NULL)
	return(0);
    pool = dict->strings;
    while (pool != NULL) {
        limit += pool->size;
	pool = pool->next;
    }
    return(limit);
}