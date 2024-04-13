xmlDOMWrapReconcileNamespaces(xmlDOMWrapCtxtPtr ctxt ATTRIBUTE_UNUSED,
			      xmlNodePtr elem,
			      int options)
{
    int depth = -1, adoptns = 0, parnsdone = 0;
    xmlNsPtr ns, prevns;
    xmlDocPtr doc;
    xmlNodePtr cur, curElem = NULL;
    xmlNsMapPtr nsMap = NULL;
    xmlNsMapItemPtr /* topmi = NULL, */ mi;
    /* @ancestorsOnly should be set by an option flag. */
    int ancestorsOnly = 0;
    int optRemoveRedundantNS =
	((xmlDOMReconcileNSOptions) options & XML_DOM_RECONNS_REMOVEREDUND) ? 1 : 0;
    xmlNsPtr *listRedund = NULL;
    int sizeRedund = 0, nbRedund = 0, ret, i, j;

    if ((elem == NULL) || (elem->doc == NULL) ||
	(elem->type != XML_ELEMENT_NODE))
	return (-1);

    doc = elem->doc;
    cur = elem;
    do {
	switch (cur->type) {
	    case XML_ELEMENT_NODE:
		adoptns = 1;
		curElem = cur;
		depth++;
		/*
		* Namespace declarations.
		*/
		if (cur->nsDef != NULL) {
		    prevns = NULL;
		    ns = cur->nsDef;
		    while (ns != NULL) {
			if (! parnsdone) {
			    if ((elem->parent) &&
				((xmlNodePtr) elem->parent->doc != elem->parent)) {
				/*
				* Gather ancestor in-scope ns-decls.
				*/
				if (xmlDOMWrapNSNormGatherInScopeNs(&nsMap,
				    elem->parent) == -1)
				    goto internal_error;
			    }
			    parnsdone = 1;
			}

			/*
			* Lookup the ns ancestor-axis for equal ns-decls in scope.
			*/
			if (optRemoveRedundantNS && XML_NSMAP_NOTEMPTY(nsMap)) {
			    XML_NSMAP_FOREACH(nsMap, mi) {
				if ((mi->depth >= XML_TREE_NSMAP_PARENT) &&
				    (mi->shadowDepth == -1) &&
				    ((ns->prefix == mi->newNs->prefix) ||
				      xmlStrEqual(ns->prefix, mi->newNs->prefix)) &&
				    ((ns->href == mi->newNs->href) ||
				      xmlStrEqual(ns->href, mi->newNs->href)))
				{
				    /*
				    * A redundant ns-decl was found.
				    * Add it to the list of redundant ns-decls.
				    */
				    if (xmlDOMWrapNSNormAddNsMapItem2(&listRedund,
					&sizeRedund, &nbRedund, ns, mi->newNs) == -1)
					goto internal_error;
				    /*
				    * Remove the ns-decl from the element-node.
				    */
				    if (prevns)
					prevns->next = ns->next;
				    else
					cur->nsDef = ns->next;
				    goto next_ns_decl;
				}
			    }
			}

			/*
			* Skip ns-references handling if the referenced
			* ns-decl is declared on the same element.
			*/
			if ((cur->ns != NULL) && adoptns && (cur->ns == ns))
			    adoptns = 0;
			/*
			* Does it shadow any ns-decl?
			*/
			if (XML_NSMAP_NOTEMPTY(nsMap)) {
			    XML_NSMAP_FOREACH(nsMap, mi) {
				if ((mi->depth >= XML_TREE_NSMAP_PARENT) &&
				    (mi->shadowDepth == -1) &&
				    ((ns->prefix == mi->newNs->prefix) ||
				    xmlStrEqual(ns->prefix, mi->newNs->prefix))) {

				    mi->shadowDepth = depth;
				}
			    }
			}
			/*
			* Push mapping.
			*/
			if (xmlDOMWrapNsMapAddItem(&nsMap, -1, ns, ns,
			    depth) == NULL)
			    goto internal_error;

			prevns = ns;
next_ns_decl:
			ns = ns->next;
		    }
		}
		if (! adoptns)
		    goto ns_end;
		/* No break on purpose. */
	    case XML_ATTRIBUTE_NODE:
		/* No ns, no fun. */
		if (cur->ns == NULL)
		    goto ns_end;

		if (! parnsdone) {
		    if ((elem->parent) &&
			((xmlNodePtr) elem->parent->doc != elem->parent)) {
			if (xmlDOMWrapNSNormGatherInScopeNs(&nsMap,
				elem->parent) == -1)
			    goto internal_error;
		    }
		    parnsdone = 1;
		}
		/*
		* Adjust the reference if this was a redundant ns-decl.
		*/
		if (listRedund) {
		   for (i = 0, j = 0; i < nbRedund; i++, j += 2) {
		       if (cur->ns == listRedund[j]) {
			   cur->ns = listRedund[++j];
			   break;
		       }
		   }
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
		* Aquire a normalized ns-decl and add it to the map.
		*/
		if (xmlDOMWrapNSNormAquireNormalizedNs(doc, curElem,
			cur->ns, &ns,
			&nsMap, depth,
			ancestorsOnly,
			(cur->type == XML_ATTRIBUTE_NODE) ? 1 : 0) == -1)
		    goto internal_error;
		cur->ns = ns;

ns_end:
		if ((cur->type == XML_ELEMENT_NODE) &&
		    (cur->properties != NULL)) {
		    /*
		    * Process attributes.
		    */
		    cur = (xmlNodePtr) cur->properties;
		    continue;
		}
		break;
	    default:
		goto next_sibling;
	}
into_content:
	if ((cur->type == XML_ELEMENT_NODE) &&
	    (cur->children != NULL)) {
	    /*
	    * Process content of element-nodes only.
	    */
	    cur = cur->children;
	    continue;
	}
next_sibling:
	if (cur == elem)
	    break;
	if (cur->type == XML_ELEMENT_NODE) {
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
	else {
	    if (cur->type == XML_ATTRIBUTE_NODE) {
		cur = cur->parent;
		goto into_content;
	    }
	    cur = cur->parent;
	    goto next_sibling;
	}
    } while (cur != NULL);

    ret = 0;
    goto exit;
internal_error:
    ret = -1;
exit:
    if (listRedund) {
	for (i = 0, j = 0; i < nbRedund; i++, j += 2) {
	    xmlFreeNs(listRedund[j]);
	}
	xmlFree(listRedund);
    }
    if (nsMap != NULL)
	xmlDOMWrapNsMapFree(nsMap);
    return (ret);
}