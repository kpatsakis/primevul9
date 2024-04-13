xmlRegCopyAtom(xmlRegParserCtxtPtr ctxt, xmlRegAtomPtr atom) {
    xmlRegAtomPtr ret;

    ret = (xmlRegAtomPtr) xmlMalloc(sizeof(xmlRegAtom));
    if (ret == NULL) {
	xmlRegexpErrMemory(ctxt, "copying atom");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlRegAtom));
    ret->type = atom->type;
    ret->quant = atom->quant;
    ret->min = atom->min;
    ret->max = atom->max;
    if (atom->nbRanges > 0) {
        int i;

        ret->ranges = (xmlRegRangePtr *) xmlMalloc(sizeof(xmlRegRangePtr) *
	                                           atom->nbRanges);
	if (ret->ranges == NULL) {
	    xmlRegexpErrMemory(ctxt, "copying atom");
	    goto error;
	}
	for (i = 0;i < atom->nbRanges;i++) {
	    ret->ranges[i] = xmlRegCopyRange(ctxt, atom->ranges[i]);
	    if (ret->ranges[i] == NULL)
	        goto error;
	    ret->nbRanges = i + 1;
	}
    }
    return(ret);

error:
    xmlRegFreeAtom(ret);
    return(NULL);
}