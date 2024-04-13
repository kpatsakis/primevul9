static void processNode(xmlTextReaderPtr reader) {
    const xmlChar *name, *value;
    int type, empty;

    type = xmlTextReaderNodeType(reader);
    empty = xmlTextReaderIsEmptyElement(reader);

    if (debug) {
	name = xmlTextReaderConstName(reader);
	if (name == NULL)
	    name = BAD_CAST "--";

	value = xmlTextReaderConstValue(reader);


	printf("%d %d %s %d %d",
		xmlTextReaderDepth(reader),
		type,
		name,
		empty,
		xmlTextReaderHasValue(reader));
	if (value == NULL)
	    printf("\n");
	else {
	    printf(" %s\n", value);
	}
    }
#ifdef LIBXML_PATTERN_ENABLED
    if (patternc) {
        xmlChar *path = NULL;
        int match = -1;

	if (type == XML_READER_TYPE_ELEMENT) {
	    /* do the check only on element start */
	    match = xmlPatternMatch(patternc, xmlTextReaderCurrentNode(reader));

	    if (match) {
#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_DEBUG_ENABLED)
		path = xmlGetNodePath(xmlTextReaderCurrentNode(reader));
		printf("Node %s matches pattern %s\n", path, pattern);
#else
                printf("Node %s matches pattern %s\n",
                       xmlTextReaderConstName(reader), pattern);
#endif
	    }
	}
	if (patstream != NULL) {
	    int ret;

	    if (type == XML_READER_TYPE_ELEMENT) {
		ret = xmlStreamPush(patstream,
		                    xmlTextReaderConstLocalName(reader),
				    xmlTextReaderConstNamespaceUri(reader));
		if (ret < 0) {
		    fprintf(stderr, "xmlStreamPush() failure\n");
                    xmlFreeStreamCtxt(patstream);
		    patstream = NULL;
		} else if (ret != match) {
#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_DEBUG_ENABLED)
		    if (path == NULL) {
		        path = xmlGetNodePath(
		                       xmlTextReaderCurrentNode(reader));
		    }
#endif
		    fprintf(stderr,
		            "xmlPatternMatch and xmlStreamPush disagree\n");
                    if (path != NULL)
                        fprintf(stderr, "  pattern %s node %s\n",
                                pattern, path);
                    else
		        fprintf(stderr, "  pattern %s node %s\n",
			    pattern, xmlTextReaderConstName(reader));
		}

	    }
	    if ((type == XML_READER_TYPE_END_ELEMENT) ||
	        ((type == XML_READER_TYPE_ELEMENT) && (empty))) {
	        ret = xmlStreamPop(patstream);
		if (ret < 0) {
		    fprintf(stderr, "xmlStreamPop() failure\n");
                    xmlFreeStreamCtxt(patstream);
		    patstream = NULL;
		}
	    }
	}
	if (path != NULL)
	    xmlFree(path);
    }
#endif
}