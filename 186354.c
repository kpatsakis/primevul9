xmlDictAddQString(xmlDictPtr dict, const xmlChar *prefix, unsigned int plen,
                 const xmlChar *name, unsigned int namelen)
{
    xmlDictStringsPtr pool;
    const xmlChar *ret;
    size_t size = 0; /* + sizeof(_xmlDictStrings) == 1024 */
    size_t limit = 0;

    if (prefix == NULL) return(xmlDictAddString(dict, name, namelen));

#ifdef DICT_DEBUG_PATTERNS
    fprintf(stderr, "=");
#endif
    pool = dict->strings;
    while (pool != NULL) {
	if (pool->end - pool->free > namelen + plen + 1)
	    goto found_pool;
	if (pool->size > size) size = pool->size;
        limit += pool->size;
	pool = pool->next;
    }
    /*
     * Not found, need to allocate
     */
    if (pool == NULL) {
        if ((dict->limit > 0) && (limit > dict->limit)) {
            return(NULL);
        }

        if (size == 0) size = 1000;
	else size *= 4; /* exponential growth */
        if (size < 4 * (namelen + plen + 1))
	    size = 4 * (namelen + plen + 1); /* just in case ! */
	pool = (xmlDictStringsPtr) xmlMalloc(sizeof(xmlDictStrings) + size);
	if (pool == NULL)
	    return(NULL);
	pool->size = size;
	pool->nbStrings = 0;
	pool->free = &pool->array[0];
	pool->end = &pool->array[size];
	pool->next = dict->strings;
	dict->strings = pool;
#ifdef DICT_DEBUG_PATTERNS
        fprintf(stderr, "+");
#endif
    }
found_pool:
    ret = pool->free;
    memcpy(pool->free, prefix, plen);
    pool->free += plen;
    *(pool->free++) = ':';
    memcpy(pool->free, name, namelen);
    pool->free += namelen;
    *(pool->free++) = 0;
    pool->nbStrings++;
    return(ret);
}