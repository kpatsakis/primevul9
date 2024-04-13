xmlDOMWrapNSNormAquireNormalizedNs(xmlDocPtr doc,
				   xmlNodePtr elem,
				   xmlNsPtr ns,
				   xmlNsPtr *retNs,
				   xmlNsMapPtr *nsMap,

				   int depth,
				   int ancestorsOnly,
				   int prefixed)
{
    xmlNsMapItemPtr mi;

    if ((doc == NULL) || (ns == NULL) || (retNs == NULL) ||
	(nsMap == NULL))
	return (-1);

    *retNs = NULL;
    /*
    * Handle XML namespace.
    */
    if (IS_STR_XML(ns->prefix)) {
	/*
	* Insert XML namespace mapping.
	*/
	*retNs = xmlTreeEnsureXMLDecl(doc);
	if (*retNs == NULL)
	    return (-1);
	return (0);
    }
    /*
    * If the search should be done in ancestors only and no
    * @elem (the first ancestor) was specified, then skip the search.
    */
    if ((XML_NSMAP_NOTEMPTY(*nsMap)) &&
	(! (ancestorsOnly && (elem == NULL))))
    {
	/*
	* Try to find an equal ns-name in in-scope ns-decls.
	*/
	XML_NSMAP_FOREACH(*nsMap, mi) {
	    if ((mi->depth >= XML_TREE_NSMAP_PARENT) &&
		/*
		* ancestorsOnly: This should be turned on to gain speed,
		* if one knows that the branch itself was already
		* ns-wellformed and no stale references existed.
		* I.e. it searches in the ancestor axis only.
		*/
		((! ancestorsOnly) || (mi->depth == XML_TREE_NSMAP_PARENT)) &&
		/* Skip shadowed prefixes. */
		(mi->shadowDepth == -1) &&
		/* Skip xmlns="" or xmlns:foo="". */
		((mi->newNs->href != NULL) &&
		(mi->newNs->href[0] != 0)) &&
		/* Ensure a prefix if wanted. */
		((! prefixed) || (mi->newNs->prefix != NULL)) &&
		/* Equal ns name */
		((mi->newNs->href == ns->href) ||
		xmlStrEqual(mi->newNs->href, ns->href))) {
		/* Set the mapping. */
		mi->oldNs = ns;
		*retNs = mi->newNs;
		return (0);
	    }
	}
    }
    /*
    * No luck, the namespace is out of scope or shadowed.
    */
    if (elem == NULL) {
	xmlNsPtr tmpns;

	/*
	* Store ns-decls in "oldNs" of the document-node.
	*/
	tmpns = xmlDOMWrapStoreNs(doc, ns->href, ns->prefix);
	if (tmpns == NULL)
	    return (-1);
	/*
	* Insert mapping.
	*/
	if (xmlDOMWrapNsMapAddItem(nsMap, -1, ns,
		tmpns, XML_TREE_NSMAP_DOC) == NULL) {
	    xmlFreeNs(tmpns);
	    return (-1);
	}
	*retNs = tmpns;
    } else {
	xmlNsPtr tmpns;

	tmpns = xmlDOMWrapNSNormDeclareNsForced(doc, elem, ns->href,
	    ns->prefix, 0);
	if (tmpns == NULL)
	    return (-1);

	if (*nsMap != NULL) {
	    /*
	    * Does it shadow ancestor ns-decls?
	    */
	    XML_NSMAP_FOREACH(*nsMap, mi) {
		if ((mi->depth < depth) &&
		    (mi->shadowDepth == -1) &&
		    ((ns->prefix == mi->newNs->prefix) ||
		    xmlStrEqual(ns->prefix, mi->newNs->prefix))) {
		    /*
		    * Shadows.
		    */
		    mi->shadowDepth = depth;
		    break;
		}
	    }
	}
	if (xmlDOMWrapNsMapAddItem(nsMap, -1, ns, tmpns, depth) == NULL) {
	    xmlFreeNs(tmpns);
	    return (-1);
	}
	*retNs = tmpns;
    }
    return (0);
}