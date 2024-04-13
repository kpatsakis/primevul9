xmlRegStrEqualWildcard(const xmlChar *expStr, const xmlChar *valStr) {
    if (expStr == valStr) return(1);
    if (expStr == NULL) return(0);
    if (valStr == NULL) return(0);
    do {
	/*
	* Eval if we have a wildcard for the current item.
	*/
        if (*expStr != *valStr) {
	    /* if one of them starts with a wildcard make valStr be it */
	    if (*valStr == '*') {
	        const xmlChar *tmp;

		tmp = valStr;
		valStr = expStr;
		expStr = tmp;
	    }
	    if ((*valStr != 0) && (*expStr != 0) && (*expStr++ == '*')) {
		do {
		    if (*valStr == XML_REG_STRING_SEPARATOR)
			break;
		    valStr++;
		} while (*valStr != 0);
		continue;
	    } else
		return(0);
	}
	expStr++;
	valStr++;
    } while (*valStr != 0);
    if (*expStr != 0)
	return (0);
    else
	return (1);
}