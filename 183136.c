xmlDOMWrapAdoptBranch(xmlDOMWrapCtxtPtr ctxt,
		      xmlDocPtr sourceDoc,
		      xmlNodePtr node,
		      xmlDocPtr destDoc,
		      xmlNodePtr destParent,
		      int options ATTRIBUTE_UNUSED)
{
    int ret = 0;
    xmlNodePtr cur, curElem = NULL;
    xmlNsMapPtr nsMap = NULL;
    xmlNsMapItemPtr mi;
    xmlNsPtr ns = NULL;
    int depth = -1, adoptStr = 1;
    /* gather @parent's ns-decls. */
    int parnsdone;
    /* @ancestorsOnly should be set per option. */
    int ancestorsOnly = 0;

    /*
    * Optimize string adoption for equal or none dicts.
    */
    if ((sourceDoc != NULL) &&
	(sourceDoc->dict == destDoc->dict))
	adoptStr = 0;
    else
	adoptStr = 1;

    /*
    * Get the ns-map from the context if available.
    */
    if (ctxt)
	nsMap = (xmlNsMapPtr) ctxt->namespaceMap;
    /*
    * Disable search for ns-decls in the parent-axis of the
    * desination element, if:
    * 1) there's no destination parent
    * 2) custom ns-reference handling is used
    */
    if ((destParent == NULL) ||
	(ctxt && ctxt->getNsForNodeFunc))
    {
	parnsdone = 1;
    } else
	parnsdone = 0;

    cur = node;
    if ((cur != NULL) && (cur->type == XML_NAMESPACE_DECL))
	goto internal_error;

    while (cur != NULL) {
	/*
	* Paranoid source-doc sanity check.
	*/
	if (cur->doc != sourceDoc) {
	    /*
	    * We'll assume XIncluded nodes if the doc differs.
	    * TODO: Do we need to reconciliate XIncluded nodes?
	    * This here skips XIncluded nodes and tries to handle
	    * broken sequences.
	    */
	    if (cur->next == NULL)
		goto leave_node;
	    do {
		cur = cur->next;
		if ((cur->type == XML_XINCLUDE_END) ||
		    (cur->doc == node->doc))
		    break;
	    } while (cur->next != NULL);

	    if (cur->doc != node->doc)
		goto leave_node;
	}
	cur->doc = destDoc;
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
		* - ns->href and ns->prefix are never in the dict, so
		*   we need not move the values over to the destination dict.
		* - Note that for custom handling of ns-references,
		*   the ns-decls need not be stored in the ns-map,
		*   since they won't be referenced by node->ns.
		*/
		if ((cur->nsDef) &&
		    ((ctxt == NULL) || (ctxt->getNsForNodeFunc == NULL)))
		{
		    if (! parnsdone) {
			/*
			* Gather @parent's in-scope ns-decls.
			*/
			if (xmlDOMWrapNSNormGatherInScopeNs(&nsMap,
			    destParent) == -1)
			    goto internal_error;
			parnsdone = 1;
		    }
		    for (ns = cur->nsDef; ns != NULL; ns = ns->next) {
			/*
			* NOTE: ns->prefix and ns->href are never in the dict.
			* XML_TREE_ADOPT_STR(ns->prefix)
			* XML_TREE_ADOPT_STR(ns->href)
			*/
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

				    mi->shadowDepth = depth;
				}
			    }
			}
			/*
			* Push mapping.
			*/
			if (xmlDOMWrapNsMapAddItem(&nsMap, -1,
			    ns, ns, depth) == NULL)
			    goto internal_error;
		    }
		}
		/* No break on purpose. */
	    case XML_ATTRIBUTE_NODE:
		/* No namespace, no fun. */
		if (cur->ns == NULL)
		    goto ns_end;

		if (! parnsdone) {
		    if (xmlDOMWrapNSNormGatherInScopeNs(&nsMap,
			destParent) == -1)
			goto internal_error;
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

			    cur->ns = mi->newNs;
			    goto ns_end;
			}
		    }
		}
		/*
		* No matching namespace in scope. We need a new one.
		*/
		if ((ctxt) && (ctxt->getNsForNodeFunc)) {
		    /*
		    * User-defined behaviour.
		    */
		    ns = ctxt->getNsForNodeFunc(ctxt, cur,
			cur->ns->href, cur->ns->prefix);
		    /*
		    * Insert mapping if ns is available; it's the users fault
		    * if not.
		    */
		    if (xmlDOMWrapNsMapAddItem(&nsMap, -1,
			    cur->ns, ns, XML_TREE_NSMAP_CUSTOM) == NULL)
			goto internal_error;
		    cur->ns = ns;
		} else {
		    /*
		    * Aquire a normalized ns-decl and add it to the map.
		    */
		    if (xmlDOMWrapNSNormAquireNormalizedNs(destDoc,
			/* ns-decls on curElem or on destDoc->oldNs */
			destParent ? curElem : NULL,
			cur->ns, &ns,
			&nsMap, depth,
			ancestorsOnly,
			/* ns-decls must be prefixed for attributes. */
			(cur->type == XML_ATTRIBUTE_NODE) ? 1 : 0) == -1)
			goto internal_error;
		    cur->ns = ns;
		}
ns_end:
		/*
		* Further node properties.
		* TODO: Is this all?
		*/
		XML_TREE_ADOPT_STR(cur->name)
		if (cur->type == XML_ELEMENT_NODE) {
		    cur->psvi = NULL;
		    cur->line = 0;
		    cur->extra = 0;
		    /*
		    * Walk attributes.
		    */
		    if (cur->properties != NULL) {
			/*
			* Process first attribute node.
			*/
			cur = (xmlNodePtr) cur->properties;
			continue;
		    }
		} else {
		    /*
		    * Attributes.
		    */
		    if ((sourceDoc != NULL) &&
			(((xmlAttrPtr) cur)->atype == XML_ATTRIBUTE_ID))
		    {
			xmlRemoveID(sourceDoc, (xmlAttrPtr) cur);
		    }
		    ((xmlAttrPtr) cur)->atype = 0;
		    ((xmlAttrPtr) cur)->psvi = NULL;
		}
		break;
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
		/*
		* This puts the content in the dest dict, only if
		* it was previously in the source dict.
		*/
		XML_TREE_ADOPT_STR_2(cur->content)
		goto leave_node;
	    case XML_ENTITY_REF_NODE:
		/*
		* Remove reference to the entitity-node.
		*/
		cur->content = NULL;
		cur->children = NULL;
		cur->last = NULL;
		if ((destDoc->intSubset) || (destDoc->extSubset)) {
		    xmlEntityPtr ent;
		    /*
		    * Assign new entity-node if available.
		    */
		    ent = xmlGetDocEntity(destDoc, cur->name);
		    if (ent != NULL) {
			cur->content = ent->content;
			cur->children = (xmlNodePtr) ent;
			cur->last = (xmlNodePtr) ent;
		    }
		}
		goto leave_node;
	    case XML_PI_NODE:
		XML_TREE_ADOPT_STR(cur->name)
		XML_TREE_ADOPT_STR_2(cur->content)
		break;
	    case XML_COMMENT_NODE:
		break;
	    default:
		goto internal_error;
	}
	/*
	* Walk the tree.
	*/
	if (cur->children != NULL) {
	    cur = cur->children;
	    continue;
	}

leave_node:
	if (cur == node)
	    break;
	if ((cur->type == XML_ELEMENT_NODE) ||
	    (cur->type == XML_XINCLUDE_START) ||
	    (cur->type == XML_XINCLUDE_END))
	{
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
	if (cur->next != NULL)
	    cur = cur->next;
	else if ((cur->type == XML_ATTRIBUTE_NODE) &&
	    (cur->parent->children != NULL))
	{
	    cur = cur->parent->children;
	} else {
	    cur = cur->parent;
	    goto leave_node;
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
    return(ret);
}