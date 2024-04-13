xsltGetInheritedNsList(xsltStylesheetPtr style,
	               xsltTemplatePtr template,
	               xmlNodePtr node)
{
    xmlNsPtr cur;
    xmlNsPtr *ret = NULL;
    int nbns = 0;
    int maxns = 10;
    int i;    

    if ((style == NULL) || (template == NULL) || (node == NULL) ||
	(template->inheritedNsNr != 0) || (template->inheritedNs != NULL))
	return(0);
    while (node != NULL) {
        if (node->type == XML_ELEMENT_NODE) {
            cur = node->nsDef;
            while (cur != NULL) {
		if (xmlStrEqual(cur->href, XSLT_NAMESPACE))
		    goto skip_ns;

		if ((cur->prefix != NULL) &&
		    (xsltCheckExtPrefix(style, cur->prefix)))
		    goto skip_ns;
		/*
		* Check if this namespace was excluded.
		* Note that at this point only the exclusions defined
		* on the topmost stylesheet element are in the exclusion-list.
		*/
		for (i = 0;i < style->exclPrefixNr;i++) {
		    if (xmlStrEqual(cur->href, style->exclPrefixTab[i]))
			goto skip_ns;
		}
                if (ret == NULL) {
                    ret =
                        (xmlNsPtr *) xmlMalloc((maxns + 1) *
                                               sizeof(xmlNsPtr));
                    if (ret == NULL) {
                        xmlGenericError(xmlGenericErrorContext,
                                        "xsltGetInheritedNsList : out of memory!\n");
                        return(0);
                    }
                    ret[nbns] = NULL;
                }
		/*
		* Skip shadowed namespace bindings.
		*/
                for (i = 0; i < nbns; i++) {
                    if ((cur->prefix == ret[i]->prefix) ||
                        (xmlStrEqual(cur->prefix, ret[i]->prefix)))
                        break;
                }
                if (i >= nbns) {
                    if (nbns >= maxns) {
                        maxns *= 2;
                        ret = (xmlNsPtr *) xmlRealloc(ret,
                                                      (maxns +
                                                       1) *
                                                      sizeof(xmlNsPtr));
                        if (ret == NULL) {
                            xmlGenericError(xmlGenericErrorContext,
                                            "xsltGetInheritedNsList : realloc failed!\n");
                            return(0);
                        }
                    }
                    ret[nbns++] = cur;
                    ret[nbns] = NULL;
                }
skip_ns:
                cur = cur->next;
            }
        }
        node = node->parent;
    }
    if (nbns != 0) {
#ifdef WITH_XSLT_DEBUG_PARSING
        xsltGenericDebug(xsltGenericDebugContext,
                         "template has %d inherited namespaces\n", nbns);
#endif
	template->inheritedNsNr = nbns;
	template->inheritedNs = ret;
    }
    return (nbns);
}