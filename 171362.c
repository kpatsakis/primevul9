xsltLREBuildEffectiveNs(xsltCompilerCtxtPtr cctxt,
			xmlNodePtr elem)
{
    xmlNsPtr ns;
    xsltNsAliasPtr alias;

    if ((cctxt == NULL) || (elem == NULL))
	return(-1);
    if ((cctxt->nsAliases == NULL) || (! cctxt->hasNsAliases))
	return(0);

    alias = cctxt->nsAliases;    		
    while (alias != NULL) {
	if ( /* If both namespaces are NULL... */
	    ( (elem->ns == NULL) &&
	    ((alias->literalNs == NULL) ||
	    (alias->literalNs->href == NULL)) ) ||
	    /* ... or both namespace are equal */
	    ( (elem->ns != NULL) &&
	    (alias->literalNs != NULL) &&
	    xmlStrEqual(elem->ns->href, alias->literalNs->href) ) )
	{
	    if ((alias->targetNs != NULL) &&
		(alias->targetNs->href != NULL))
	    {
		/*
		* Convert namespace.
		*/
		if (elem->doc == alias->docOfTargetNs) {
		    /*
		    * This is the nice case: same docs.
		    * This will eventually assign a ns-decl which
		    * is shadowed, but this has no negative effect on
		    * the generation of the result tree.
		    */
		    elem->ns = alias->targetNs;
		} else {
		    /*
		    * This target xmlNs originates from a different
		    * stylesheet tree. Try to locate it in the
		    * in-scope namespaces.
		    * OPTIMIZE TODO: Use the compiler-node-info inScopeNs.
		    */
		    ns = xmlSearchNs(elem->doc, elem,
			alias->targetNs->prefix);		    
		    /*
		    * If no matching ns-decl found, then assign a
		    * ns-decl stored in xmlDoc.
		    */
		    if ((ns == NULL) ||
			(! xmlStrEqual(ns->href, alias->targetNs->href)))
		    {
			/*
			* BIG NOTE: The use of xsltTreeAcquireStoredNs()
			*  is not very efficient, but currently I don't
			*  see an other way of *safely* changing a node's
			*  namespace, since the xmlNs struct in
			*  alias->targetNs might come from an other
			*  stylesheet tree. So we need to anchor it in the
			*  current document, without adding it to the tree,
			*  which would otherwise change the in-scope-ns
			*  semantic of the tree.
			*/
			ns = xsltTreeAcquireStoredNs(elem->doc,
			    alias->targetNs->href,
			    alias->targetNs->prefix);
			
			if (ns == NULL) {
			    xsltTransformError(NULL, cctxt->style, elem,
				"Internal error in "
				"xsltLREBuildEffectiveNs(): "
				"failed to acquire a stored "
				"ns-declaration.\n");
			    cctxt->style->errors++;
			    return(-1);
			    
			}
		    }
		    elem->ns = ns;
		}		   
	    } else {
		/*
		* Move into or leave in the NULL namespace.
		*/
		elem->ns = NULL;
	    }
	    break;
	}
	alias = alias->next;
    }
    /*
    * Same with attributes of literal result elements.
    */
    if (elem->properties != NULL) {
	xmlAttrPtr attr = elem->properties;
	
	while (attr != NULL) {
	    if (attr->ns == NULL) {
		attr = attr->next;
		continue;
	    }
	    alias = cctxt->nsAliases;
	    while (alias != NULL) {
		if ( /* If both namespaces are NULL... */
		    ( (elem->ns == NULL) &&
		    ((alias->literalNs == NULL) ||
		    (alias->literalNs->href == NULL)) ) ||
		    /* ... or both namespace are equal */
		    ( (elem->ns != NULL) &&
		    (alias->literalNs != NULL) &&
		    xmlStrEqual(elem->ns->href, alias->literalNs->href) ) )
		{
		    if ((alias->targetNs != NULL) &&
			(alias->targetNs->href != NULL))
		    {		    
			if (elem->doc == alias->docOfTargetNs) {
			    elem->ns = alias->targetNs;
			} else {
			    ns = xmlSearchNs(elem->doc, elem,
				alias->targetNs->prefix);
			    if ((ns == NULL) ||
				(! xmlStrEqual(ns->href, alias->targetNs->href)))
			    {
				ns = xsltTreeAcquireStoredNs(elem->doc,
				    alias->targetNs->href,
				    alias->targetNs->prefix);
				
				if (ns == NULL) {
				    xsltTransformError(NULL, cctxt->style, elem,
					"Internal error in "
					"xsltLREBuildEffectiveNs(): "
					"failed to acquire a stored "
					"ns-declaration.\n");
				    cctxt->style->errors++;
				    return(-1);
				    
				}
			    }
			    elem->ns = ns;
			}
		    } else {
		    /*
		    * Move into or leave in the NULL namespace.
			*/
			elem->ns = NULL;
		    }
		    break;
		}
		alias = alias->next;
	    }
	    
	    attr = attr->next;
	}
    }
    return(0);
}