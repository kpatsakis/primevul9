xmlXIncludeMergeEntity(void *payload, void *vdata,
	               const xmlChar *name ATTRIBUTE_UNUSED) {
    xmlEntityPtr ent = (xmlEntityPtr) payload;
    xmlXIncludeMergeDataPtr data = (xmlXIncludeMergeDataPtr) vdata;
    xmlEntityPtr ret, prev;
    xmlDocPtr doc;
    xmlXIncludeCtxtPtr ctxt;

    if ((ent == NULL) || (data == NULL))
	return;
    ctxt = data->ctxt;
    doc = data->doc;
    if ((ctxt == NULL) || (doc == NULL))
	return;
    switch (ent->etype) {
        case XML_INTERNAL_PARAMETER_ENTITY:
        case XML_EXTERNAL_PARAMETER_ENTITY:
        case XML_INTERNAL_PREDEFINED_ENTITY:
	    return;
        case XML_INTERNAL_GENERAL_ENTITY:
        case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
        case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
	    break;
    }
    ret = xmlAddDocEntity(doc, ent->name, ent->etype, ent->ExternalID,
			  ent->SystemID, ent->content);
    if (ret != NULL) {
	if (ent->URI != NULL)
	    ret->URI = xmlStrdup(ent->URI);
    } else {
	prev = xmlGetDocEntity(doc, ent->name);
	if (prev != NULL) {
	    if (ent->etype != prev->etype)
		goto error;

	    if ((ent->SystemID != NULL) && (prev->SystemID != NULL)) {
		if (!xmlStrEqual(ent->SystemID, prev->SystemID))
		    goto error;
	    } else if ((ent->ExternalID != NULL) &&
		       (prev->ExternalID != NULL)) {
		if (!xmlStrEqual(ent->ExternalID, prev->ExternalID))
		    goto error;
	    } else if ((ent->content != NULL) && (prev->content != NULL)) {
		if (!xmlStrEqual(ent->content, prev->content))
		    goto error;
	    } else {
		goto error;
	    }

	}
    }
    return;
error:
    switch (ent->etype) {
        case XML_INTERNAL_PARAMETER_ENTITY:
        case XML_EXTERNAL_PARAMETER_ENTITY:
        case XML_INTERNAL_PREDEFINED_ENTITY:
        case XML_INTERNAL_GENERAL_ENTITY:
        case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
	    return;
        case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
	    break;
    }
    xmlXIncludeErr(ctxt, (xmlNodePtr) ent, XML_XINCLUDE_ENTITY_DEF_MISMATCH,
                   "mismatch in redefinition of entity %s\n",
		   ent->name);
}