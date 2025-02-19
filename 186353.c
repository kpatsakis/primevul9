xmlDictCreate(void) {
    xmlDictPtr dict;

    if (!xmlDictInitialized)
        if (!__xmlInitializeDict())
            return(NULL);

#ifdef DICT_DEBUG_PATTERNS
    fprintf(stderr, "C");
#endif

    dict = xmlMalloc(sizeof(xmlDict));
    if (dict) {
        dict->ref_counter = 1;
        dict->limit = 0;

        dict->size = MIN_DICT_SIZE;
	dict->nbElems = 0;
        dict->dict = xmlMalloc(MIN_DICT_SIZE * sizeof(xmlDictEntry));
	dict->strings = NULL;
	dict->subdict = NULL;
        if (dict->dict) {
	    memset(dict->dict, 0, MIN_DICT_SIZE * sizeof(xmlDictEntry));
#ifdef DICT_RANDOMIZATION
            dict->seed = __xmlRandom();
#else
            dict->seed = 0;
#endif
	    return(dict);
        }
        xmlFree(dict);
    }
    return(NULL);
}