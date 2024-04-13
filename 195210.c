xmlXIncludeMergeEntities(xmlXIncludeCtxtPtr ctxt, xmlDocPtr doc,
	                 xmlDocPtr from) {
    xmlNodePtr cur;
    xmlDtdPtr target, source;

    if (ctxt == NULL)
	return(-1);

    if ((from == NULL) || (from->intSubset == NULL))
	return(0);

    target = doc->intSubset;
    if (target == NULL) {
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL)
	    return(-1);
        target = xmlCreateIntSubset(doc, cur->name, NULL, NULL);
	if (target == NULL)
	    return(-1);
    }

    source = from->intSubset;
    if ((source != NULL) && (source->entities != NULL)) {
	xmlXIncludeMergeData data;

	data.ctxt = ctxt;
	data.doc = doc;

	xmlHashScan((xmlHashTablePtr) source->entities,
		    xmlXIncludeMergeEntity, &data);
    }
    source = from->extSubset;
    if ((source != NULL) && (source->entities != NULL)) {
	xmlXIncludeMergeData data;

	data.ctxt = ctxt;
	data.doc = doc;

	/*
	 * don't duplicate existing stuff when external subsets are the same
	 */
	if ((!xmlStrEqual(target->ExternalID, source->ExternalID)) &&
	    (!xmlStrEqual(target->SystemID, source->SystemID))) {
	    xmlHashScan((xmlHashTablePtr) source->entities,
			xmlXIncludeMergeEntity, &data);
	}
    }
    return(0);
}