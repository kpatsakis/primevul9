xmlRegAtomAddRange(xmlRegParserCtxtPtr ctxt, xmlRegAtomPtr atom,
	           int neg, xmlRegAtomType type, int start, int end,
		   xmlChar *blockName) {
    xmlRegRangePtr range;

    if (atom == NULL) {
	ERROR("add range: atom is NULL");
	return;
    }
    if (atom->type != XML_REGEXP_RANGES) {
	ERROR("add range: atom is not ranges");
	return;
    }
    if (atom->maxRanges == 0) {
	atom->maxRanges = 4;
	atom->ranges = (xmlRegRangePtr *) xmlMalloc(atom->maxRanges *
		                             sizeof(xmlRegRangePtr));
	if (atom->ranges == NULL) {
	    xmlRegexpErrMemory(ctxt, "adding ranges");
	    atom->maxRanges = 0;
	    return;
	}
    } else if (atom->nbRanges >= atom->maxRanges) {
	xmlRegRangePtr *tmp;
	atom->maxRanges *= 2;
	tmp = (xmlRegRangePtr *) xmlRealloc(atom->ranges, atom->maxRanges *
		                             sizeof(xmlRegRangePtr));
	if (tmp == NULL) {
	    xmlRegexpErrMemory(ctxt, "adding ranges");
	    atom->maxRanges /= 2;
	    return;
	}
	atom->ranges = tmp;
    }
    range = xmlRegNewRange(ctxt, neg, type, start, end);
    if (range == NULL)
	return;
    range->blockName = blockName;
    atom->ranges[atom->nbRanges++] = range;

}