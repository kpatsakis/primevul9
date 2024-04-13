xmlDOMWrapCloneNode(xmlDOMWrapCtxtPtr ctxt,
		      xmlDocPtr sourceDoc,
		      xmlNodePtr node,
		      xmlNodePtr *resNode,
		      xmlDocPtr destDoc,
		      xmlNodePtr destParent,
		      int deep,
		      int options ATTRIBUTE_UNUSED)
{
    int ret = 0;
    xmlNodePtr cur, curElem = NULL;
    xmlNsMapPtr nsMap = NULL;
    xmlNsMapItemPtr mi;
    xmlNsPtr ns;
    int depth = -1;
    /* int adoptStr = 1; */
    /* gather @parent's ns-decls. */
    int parnsdone = 0;
    /*
    * @ancestorsOnly:
    * TODO: @ancestorsOnly should be set per option.
    *
    */
    int ancestorsOnly = 0;
    xmlNodePtr resultClone = NULL, clone = NULL, parentClone = NULL, prevClone = NULL;
    xmlNsPtr cloneNs = NULL, *cloneNsDefSlot = NULL;
    xmlDictPtr dict; /* The destination dict */

    if ((node == NULL) || (resNode == NULL) || (destDoc == NULL))
	return(-1);
    /*
    * TODO: Initially we support only element-nodes.
    */
    if (node->type != XML_ELEMENT_NODE)
	return(1);
    /*
    * Check node->doc sanity.
    */
    if ((node->doc != NULL) && (sourceDoc != NULL) &&
	(node->doc != sourceDoc)) {
	/*
	* Might be an XIncluded node.
	*/
	return (-1);
    }
    if (sourceDoc == NULL)
	sourceDoc = node->doc;
    if (sourceDoc == NULL)
        return (-1);

    dict = destDoc->dict;
    /*
    * Reuse the namespace map of the context.
    */
    if (ctxt)
	nsMap = (xmlNsMapPtr) ctxt->namespaceMap;

    *resNode = NULL;

    cur = node;
    if ((cur != NULL) && (cur->type == XML_NAMESPACE_DECL))
        return(-1);

    while (cur != NULL) {
	if (cur->doc != sourceDoc) {
	    /*
	    * We'll assume XIncluded nodes if the doc differs.
	    * TODO: Do we need to reconciliate XIncluded nodes?
	    * TODO: This here returns -1 in this case.
	    */
	    goto internal_error;
	}
	/*
	* Create a new node.
	*/
	switch (cur->type) {
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
		/*
		* TODO: What to do with XInclude?
		*/
		goto internal_error;
		break;
	    case XML_ELEMENT_NODE:
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
	    case XML_COMMENT_NODE:
	    case XML_PI_NODE:
	    case XML_DOCUMENT_FRAG_NODE:
	    case XML_ENTITY_REF_NODE:
	    case XML_ENTITY_NODE:
		/*
		* Nodes of xmlNode structure.
		*/
		clone = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
		if (clone == NULL) {
		    xmlTreeErrMemory("xmlDOMWrapCloneNode(): allocating a node");
		    goto internal_error;
		}
		memset(clone, 0, sizeof(xmlNode));
		/*
		* Set hierachical links.
		*/
		if (resultClone != NULL) {
		    clone->parent = parentClone;
		    if (prevClone) {
			prevClone->next = clone;
			clone->prev = prevClone;
		    } else
			parentClone->children = clone;
		} else
		    resultClone = clone;

		break;
	    case XML_ATTRIBUTE_NODE:
		/*
		* Attributes (xmlAttr).
		*/
		clone = (xmlNodePtr) xmlMalloc(sizeof(xmlAttr));
		if (clone == NULL) {
		    xmlTreeErrMemory("xmlDOMWrapCloneNode(): allocating an attr-node");
		    goto internal_error;
		}
		memset(clone, 0, sizeof(xmlAttr));
		/*
		* Set hierachical links.
		* TODO: Change this to add to the end of attributes.
		*/
		if (resultClone != NULL) {
		    clone->parent = parentClone;
		    if (prevClone) {
			prevClone->next = clone;
			clone->prev = prevClone;
		    } else
			parentClone->properties = (xmlAttrPtr) clone;
		} else
		    resultClone = clone;
		break;
	    default:
		/*
		* TODO QUESTION: Any other nodes expected?
		*/
		goto internal_error;
	}

	clone->type = cur->type;
	clone->doc = destDoc;

	/*
	* Clone the name of the node if any.
	*/
	if (cur->name == xmlStringText)
	    clone->name = xmlStringText;
	else if (cur->name == xmlStringTextNoenc)
	    /*
	    * NOTE: Although xmlStringTextNoenc is never assigned to a node
	    *   in tree.c, it might be set in Libxslt via
	    *   "xsl:disable-output-escaping".
	    */
	    clone->name = xmlStringTextNoenc;
	else if (cur->name == xmlStringComment)
	    clone->name = xmlStringComment;
	else if (cur->name != NULL) {
	    DICT_CONST_COPY(cur->name, clone->name);
	}

	switch (cur->type) {
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
		/*
		* TODO
		*/
		return (-1);
	    case XML_ELEMENT_NODE:
		curElem = cur;
		depth++;
		/*
		* Namespace declarations.
		*/
		if (cur->nsDef != NULL) {
		    if (! parnsdone) {
			if (destParent && (ctxt == NULL)) {
			    /*
			    * Gather @parent's in-scope ns-decls.
			    */
			    if (xmlDOMWrapNSNormGatherInScopeNs(&nsMap,
				destParent) == -1)
				goto internal_error;
			}
			parnsdone = 1;
		    }
		    /*
		    * Clone namespace declarations.
		    */
		    cloneNsDefSlot = &(clone->nsDef);
		    for (ns = cur->nsDef; ns != NULL; ns = ns->next) {
			/*
			* Create a new xmlNs.
			*/
			cloneNs = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
			if (cloneNs == NULL) {
			    xmlTreeErrMemory("xmlDOMWrapCloneNode(): "
				"allocating namespace");
			    return(-1);
			}
			memset(cloneNs, 0, sizeof(xmlNs));
			cloneNs->type = XML_LOCAL_NAMESPACE;

			if (ns->href != NULL)
			    cloneNs->href = xmlStrdup(ns->href);
			if (ns->prefix != NULL)
			    cloneNs->prefix = xmlStrdup(ns->prefix);

			*cloneNsDefSlot = cloneNs;
			cloneNsDefSlot = &(cloneNs->next);

			/*
			* Note that for custom handling of ns-references,
			* the ns-decls need not be stored in the ns-map,
			* since they won't be referenced by node->ns.
			*/
			if ((ctxt == NULL) ||
			    (ctxt->getNsForNodeFunc == NULL))
			{
			    /*
			    * Does it shadow any ns-decl?
			    */
			    if (XML_NSMAP_NOTEMPTY(nsMap)) {
				XML_NSMAP_FOREACH(nsMap, mi) {
				    if ((mi->depth >= XML_TREE_NSMAP_PARENT) &&
					(mi->shadowDepth == -1) &&
					((ns->prefix == mi->newNs->prefix) ||
					xmlStrEqual(ns->prefix,
					mi->newNs->prefix))) {
					/*
					* Mark as shadowed at the current
					* depth.
					*/
					mi->shadowDepth = depth;
				    }
				}
			    }
			    /*
			    * Push mapping.
			    */
			    if (xmlDOMWrapNsMapAddItem(&nsMap, -1,
				ns, cloneNs, depth) == NULL)
				goto internal_error;
			}
		    }
		}
		/* cur->ns will be processed further down. */
		break;
	    case XML_ATTRIBUTE_NODE:
		/* IDs will be processed further down. */
		/* cur->ns will be processed further down. */
		break;
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
		/*
		* Note that this will also cover the values of attributes.
		*/
		DICT_COPY(cur->content, clone->content);
		goto leave_node;
	    case XML_ENTITY_NODE:
		/* TODO: What to do here? */
		goto leave_node;
	    case XML_ENTITY_REF_NODE:
		if (sourceDoc != destDoc) {
		    if ((destDoc->intSubset) || (destDoc->extSubset)) {
			xmlEntityPtr ent;
			/*
			* Different doc: Assign new entity-node if available.
			*/
			ent = xmlGetDocEntity(destDoc, cur->name);
			if (ent != NULL) {
			    clone->content = ent->content;
			    clone->children = (xmlNodePtr) ent;
			    clone->last = (xmlNodePtr) ent;
			}
		    }
		} else {
		    /*
		    * Same doc: Use the current node's entity declaration
		    * and value.
		    */
		    clone->content = cur->content;
		    clone->children = cur->children;
		    clone->last = cur->last;
		}
		goto leave_node;
	    case XML_PI_NODE:
		DICT_COPY(cur->content, clone->content);
		goto leave_node;
	    case XML_COMMENT_NODE:
		DICT_COPY(cur->content, clone->content);
		goto leave_node;
	    default:
		goto internal_error;
	}

	if (cur->ns == NULL)
	    goto end_ns_reference;

/* handle_ns_reference: */
	/*
	** The following will take care of references to ns-decls ********
	** and is intended only for element- and attribute-nodes.
	**
	*/
	if (! parnsdone) {
	    if (destParent && (ctxt == NULL)) {
		if (xmlDOMWrapNSNormGatherInScopeNs(&nsMap, destParent) == -1)
		    goto internal_error;
	    }
	    parnsdone = 1;
	}
	/*
	* Adopt ns-references.
	*/
	if (XML_NSMAP_NOTEMPTY(nsMap)) {
	    /*
	    * Search for a mapping.
	    */
	    XML_NSMAP_FOREACH(nsMap, mi) {
		if ((mi->shadowDepth == -1) &&
		    (cur->ns == mi->oldNs)) {
		    /*
		    * This is the nice case: a mapping was found.
		    */
		    clone->ns = mi->newNs;
		    goto end_ns_reference;
		}
	    }
	}
	/*
	* No matching namespace in scope. We need a new one.
	*/
	if ((ctxt != NULL) && (ctxt->getNsForNodeFunc != NULL)) {
	    /*
	    * User-defined behaviour.
	    */
	    ns = ctxt->getNsForNodeFunc(ctxt, cur,
		cur->ns->href, cur->ns->prefix);
	    /*
	    * Add user's mapping.
	    */
	    if (xmlDOMWrapNsMapAddItem(&nsMap, -1,
		cur->ns, ns, XML_TREE_NSMAP_CUSTOM) == NULL)
		goto internal_error;
	    clone->ns = ns;
	} else {
	    /*
	    * Aquire a normalized ns-decl and add it to the map.
	    */
	    if (xmlDOMWrapNSNormAquireNormalizedNs(destDoc,
		/* ns-decls on curElem or on destDoc->oldNs */
		destParent ? curElem : NULL,
		cur->ns, &ns,
		&nsMap, depth,
		/* if we need to search only in the ancestor-axis */
		ancestorsOnly,
		/* ns-decls must be prefixed for attributes. */
		(cur->type == XML_ATTRIBUTE_NODE) ? 1 : 0) == -1)
		goto internal_error;
	    clone->ns = ns;
	}

end_ns_reference:

	/*
	* Some post-processing.
	*
	* Handle ID attributes.
	*/
	if ((clone->type == XML_ATTRIBUTE_NODE) &&
	    (clone->parent != NULL))
	{
	    if (xmlIsID(destDoc, clone->parent, (xmlAttrPtr) clone)) {

		xmlChar *idVal;

		idVal = xmlNodeListGetString(cur->doc, cur->children, 1);
		if (idVal != NULL) {
		    if (xmlAddID(NULL, destDoc, idVal, (xmlAttrPtr) cur) == NULL) {
			/* TODO: error message. */
			xmlFree(idVal);
			goto internal_error;
		    }
		    xmlFree(idVal);
		}
	    }
	}
	/*
	**
	** The following will traverse the tree **************************
	**
	*
	* Walk the element's attributes before descending into child-nodes.
	*/
	if ((cur->type == XML_ELEMENT_NODE) && (cur->properties != NULL)) {
	    prevClone = NULL;
	    parentClone = clone;
	    cur = (xmlNodePtr) cur->properties;
	    continue;
	}
into_content:
	/*
	* Descend into child-nodes.
	*/
	if (cur->children != NULL) {
	    if (deep || (cur->type == XML_ATTRIBUTE_NODE)) {
		prevClone = NULL;
		parentClone = clone;
		cur = cur->children;
		continue;
	    }
	}

leave_node:
	/*
	* At this point we are done with the node, its content
	* and an element-nodes's attribute-nodes.
	*/
	if (cur == node)
	    break;
	if ((cur->type == XML_ELEMENT_NODE) ||
	    (cur->type == XML_XINCLUDE_START) ||
	    (cur->type == XML_XINCLUDE_END)) {
	    /*
	    * TODO: Do we expect nsDefs on XML_XINCLUDE_START?
	    */
	    if (XML_NSMAP_NOTEMPTY(nsMap)) {
		/*
		* Pop mappings.
		*/
		while ((nsMap->last != NULL) &&
		    (nsMap->last->depth >= depth))
		{
		    XML_NSMAP_POP(nsMap, mi)
		}
		/*
		* Unshadow.
		*/
		XML_NSMAP_FOREACH(nsMap, mi) {
		    if (mi->shadowDepth >= depth)
			mi->shadowDepth = -1;
		}
	    }
	    depth--;
	}
	if (cur->next != NULL) {
	    prevClone = clone;
	    cur = cur->next;
	} else if (cur->type != XML_ATTRIBUTE_NODE) {
	    /*
	    * Set clone->last.
	    */
	    if (clone->parent != NULL)
		clone->parent->last = clone;
	    clone = clone->parent;
	    if (clone != NULL)
		parentClone = clone->parent;
	    /*
	    * Process parent --> next;
	    */
	    cur = cur->parent;
	    goto leave_node;
	} else {
	    /* This is for attributes only. */
	    clone = clone->parent;
	    parentClone = clone->parent;
	    /*
	    * Process parent-element --> children.
	    */
	    cur = cur->parent;
	    goto into_content;
	}
    }
    goto exit;

internal_error:
    ret = -1;

exit:
    /*
    * Cleanup.
    */
    if (nsMap != NULL) {
	if ((ctxt) && (ctxt->namespaceMap == nsMap)) {
	    /*
	    * Just cleanup the map but don't free.
	    */
	    if (nsMap->first) {
		if (nsMap->pool)
		    nsMap->last->next = nsMap->pool;
		nsMap->pool = nsMap->first;
		nsMap->first = NULL;
	    }
	} else
	    xmlDOMWrapNsMapFree(nsMap);
    }
    /*
    * TODO: Should we try a cleanup of the cloned node in case of a
    * fatal error?
    */
    *resNode = resultClone;
    return (ret);
}