xmlDictGrow(xmlDictPtr dict, size_t size) {
    unsigned long key, okey;
    size_t oldsize, i;
    xmlDictEntryPtr iter, next;
    struct _xmlDictEntry *olddict;
#ifdef DEBUG_GROW
    unsigned long nbElem = 0;
#endif
    int ret = 0;
    int keep_keys = 1;

    if (dict == NULL)
	return(-1);
    if (size < 8)
        return(-1);
    if (size > 8 * 2048)
	return(-1);

#ifdef DICT_DEBUG_PATTERNS
    fprintf(stderr, "*");
#endif

    oldsize = dict->size;
    olddict = dict->dict;
    if (olddict == NULL)
        return(-1);
    if (oldsize == MIN_DICT_SIZE)
        keep_keys = 0;

    dict->dict = xmlMalloc(size * sizeof(xmlDictEntry));
    if (dict->dict == NULL) {
	dict->dict = olddict;
	return(-1);
    }
    memset(dict->dict, 0, size * sizeof(xmlDictEntry));
    dict->size = size;

    /*	If the two loops are merged, there would be situations where
	a new entry needs to allocated and data copied into it from
	the main dict. It is nicer to run through the array twice, first
	copying all the elements in the main array (less probability of
	allocate) and then the rest, so we only free in the second loop.
    */
    for (i = 0; i < oldsize; i++) {
	if (olddict[i].valid == 0)
	    continue;

	if (keep_keys)
	    okey = olddict[i].okey;
	else
	    okey = xmlDictComputeKey(dict, olddict[i].name, olddict[i].len);
	key = okey % dict->size;

	if (dict->dict[key].valid == 0) {
	    memcpy(&(dict->dict[key]), &(olddict[i]), sizeof(xmlDictEntry));
	    dict->dict[key].next = NULL;
	    dict->dict[key].okey = okey;
	} else {
	    xmlDictEntryPtr entry;

	    entry = xmlMalloc(sizeof(xmlDictEntry));
	    if (entry != NULL) {
		entry->name = olddict[i].name;
		entry->len = olddict[i].len;
		entry->okey = okey;
		entry->next = dict->dict[key].next;
		entry->valid = 1;
		dict->dict[key].next = entry;
	    } else {
	        /*
		 * we don't have much ways to alert from herei
		 * result is loosing an entry and unicity garantee
		 */
	        ret = -1;
	    }
	}
#ifdef DEBUG_GROW
	nbElem++;
#endif
    }

    for (i = 0; i < oldsize; i++) {
	iter = olddict[i].next;
	while (iter) {
	    next = iter->next;

	    /*
	     * put back the entry in the new dict
	     */

	    if (keep_keys)
		okey = iter->okey;
	    else
		okey = xmlDictComputeKey(dict, iter->name, iter->len);
	    key = okey % dict->size;
	    if (dict->dict[key].valid == 0) {
		memcpy(&(dict->dict[key]), iter, sizeof(xmlDictEntry));
		dict->dict[key].next = NULL;
		dict->dict[key].valid = 1;
		dict->dict[key].okey = okey;
		xmlFree(iter);
	    } else {
		iter->next = dict->dict[key].next;
		iter->okey = okey;
		dict->dict[key].next = iter;
	    }

#ifdef DEBUG_GROW
	    nbElem++;
#endif

	    iter = next;
	}
    }

    xmlFree(olddict);

#ifdef DEBUG_GROW
    xmlGenericError(xmlGenericErrorContext,
	    "xmlDictGrow : from %lu to %lu, %u elems\n", oldsize, size, nbElem);
#endif

    return(ret);
}