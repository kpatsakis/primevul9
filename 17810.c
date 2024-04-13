xmlParseCtxtExternalEntity(xmlParserCtxtPtr ctx, const xmlChar *URL,
	               const xmlChar *ID, xmlNodePtr *lst) {
    xmlParserCtxtPtr ctxt;
    xmlDocPtr newDoc;
    xmlNodePtr newRoot;
    xmlSAXHandlerPtr oldsax = NULL;
    int ret = 0;
    xmlChar start[4];
    xmlCharEncoding enc;

    if (ctx == NULL) return(-1);

    if (((ctx->depth > 40) && ((ctx->options & XML_PARSE_HUGE) == 0)) ||
        (ctx->depth > 1024)) {
	return(XML_ERR_ENTITY_LOOP);
    }

    if (lst != NULL)
        *lst = NULL;
    if ((URL == NULL) && (ID == NULL))
	return(-1);
    if (ctx->myDoc == NULL) /* @@ relax but check for dereferences */
	return(-1);

    ctxt = xmlCreateEntityParserCtxtInternal(URL, ID, NULL, ctx);
    if (ctxt == NULL) {
	return(-1);
    }

    oldsax = ctxt->sax;
    ctxt->sax = ctx->sax;
    xmlDetectSAX2(ctxt);
    newDoc = xmlNewDoc(BAD_CAST "1.0");
    if (newDoc == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(-1);
    }
    newDoc->properties = XML_DOC_INTERNAL;
    if (ctx->myDoc->dict) {
	newDoc->dict = ctx->myDoc->dict;
	xmlDictReference(newDoc->dict);
    }
    if (ctx->myDoc != NULL) {
	newDoc->intSubset = ctx->myDoc->intSubset;
	newDoc->extSubset = ctx->myDoc->extSubset;
    }
    if (ctx->myDoc->URL != NULL) {
	newDoc->URL = xmlStrdup(ctx->myDoc->URL);
    }
    newRoot = xmlNewDocNode(newDoc, NULL, BAD_CAST "pseudoroot", NULL);
    if (newRoot == NULL) {
	ctxt->sax = oldsax;
	xmlFreeParserCtxt(ctxt);
	newDoc->intSubset = NULL;
	newDoc->extSubset = NULL;
        xmlFreeDoc(newDoc);
	return(-1);
    }
    xmlAddChild((xmlNodePtr) newDoc, newRoot);
    nodePush(ctxt, newDoc->children);
    if (ctx->myDoc == NULL) {
	ctxt->myDoc = newDoc;
    } else {
	ctxt->myDoc = ctx->myDoc;
	newDoc->children->doc = ctx->myDoc;
    }

    /*
     * Get the 4 first bytes and decode the charset
     * if enc != XML_CHAR_ENCODING_NONE
     * plug some encoding conversion routines.
     */
    GROW
    if ((ctxt->input->end - ctxt->input->cur) >= 4) {
	start[0] = RAW;
	start[1] = NXT(1);
	start[2] = NXT(2);
	start[3] = NXT(3);
	enc = xmlDetectCharEncoding(start, 4);
	if (enc != XML_CHAR_ENCODING_NONE) {
	    xmlSwitchEncoding(ctxt, enc);
	}
    }

    /*
     * Parse a possible text declaration first
     */
    if ((CMP5(CUR_PTR, '<', '?', 'x', 'm', 'l')) && (IS_BLANK_CH(NXT(5)))) {
	xmlParseTextDecl(ctxt);
	/*
	 * An XML-1.0 document can't reference an entity not XML-1.0
	 */
	if ((xmlStrEqual(ctx->version, BAD_CAST "1.0")) &&
	    (!xmlStrEqual(ctxt->input->version, BAD_CAST "1.0"))) {
	    xmlFatalErrMsg(ctxt, XML_ERR_VERSION_MISMATCH, 
	                   "Version mismatch between document and entity\n");
	}
    }

    /*
     * Doing validity checking on chunk doesn't make sense
     */
    ctxt->instate = XML_PARSER_CONTENT;
    ctxt->validate = ctx->validate;
    ctxt->valid = ctx->valid;
    ctxt->loadsubset = ctx->loadsubset;
    ctxt->depth = ctx->depth + 1;
    ctxt->replaceEntities = ctx->replaceEntities;
    if (ctxt->validate) {
	ctxt->vctxt.error = ctx->vctxt.error;
	ctxt->vctxt.warning = ctx->vctxt.warning;
    } else {
	ctxt->vctxt.error = NULL;
	ctxt->vctxt.warning = NULL;
    }
    ctxt->vctxt.nodeTab = NULL;
    ctxt->vctxt.nodeNr = 0;
    ctxt->vctxt.nodeMax = 0;
    ctxt->vctxt.node = NULL;
    if (ctxt->dict != NULL) xmlDictFree(ctxt->dict);
    ctxt->dict = ctx->dict;
    ctxt->str_xml = xmlDictLookup(ctxt->dict, BAD_CAST "xml", 3);
    ctxt->str_xmlns = xmlDictLookup(ctxt->dict, BAD_CAST "xmlns", 5);
    ctxt->str_xml_ns = xmlDictLookup(ctxt->dict, XML_XML_NAMESPACE, 36);
    ctxt->dictNames = ctx->dictNames;
    ctxt->attsDefault = ctx->attsDefault;
    ctxt->attsSpecial = ctx->attsSpecial;
    ctxt->linenumbers = ctx->linenumbers;

    xmlParseContent(ctxt);

    ctx->validate = ctxt->validate;
    ctx->valid = ctxt->valid;
    if ((RAW == '<') && (NXT(1) == '/')) {
	xmlFatalErr(ctxt, XML_ERR_NOT_WELL_BALANCED, NULL);
    } else if (RAW != 0) {
	xmlFatalErr(ctxt, XML_ERR_EXTRA_CONTENT, NULL);
    }
    if (ctxt->node != newDoc->children) {
	xmlFatalErr(ctxt, XML_ERR_NOT_WELL_BALANCED, NULL);
    }

    if (!ctxt->wellFormed) {
        if (ctxt->errNo == 0)
	    ret = 1;
	else
	    ret = ctxt->errNo;
    } else {
	if (lst != NULL) {
	    xmlNodePtr cur;

	    /*
	     * Return the newly created nodeset after unlinking it from
	     * they pseudo parent.
	     */
	    cur = newDoc->children->children;
	    *lst = cur;
	    while (cur != NULL) {
		cur->parent = NULL;
		cur = cur->next;
	    }
            newDoc->children->children = NULL;
	}
	ret = 0;
    }
    ctxt->sax = oldsax;
    ctxt->dict = NULL;
    ctxt->attsDefault = NULL;
    ctxt->attsSpecial = NULL;
    xmlFreeParserCtxt(ctxt);
    newDoc->intSubset = NULL;
    newDoc->extSubset = NULL;
    xmlFreeDoc(newDoc);

    return(ret);
}