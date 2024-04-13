xmlDictSetLimit(xmlDictPtr dict, size_t limit) {
    size_t ret;

    if (dict == NULL)
	return(0);
    ret = dict->limit;
    dict->limit = limit;
    return(ret);
}