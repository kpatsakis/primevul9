xmlCopyPropInternal(xmlDocPtr doc, xmlNodePtr target, xmlAttrPtr cur) {
    xmlAttrPtr ret;

    if (cur == NULL) return(NULL);
    if ((target != NULL) && (target->type != XML_ELEMENT_NODE))
        return(NULL);
    if (target != NULL)
	ret = xmlNewDocProp(target->doc, cur->name, NULL);
    else if (doc != NULL)
	ret = xmlNewDocProp(doc, cur->name, NULL);
    else if (cur->parent != NULL)
	ret = xmlNewDocProp(cur->parent->doc, cur->name, NULL);
    else if (cur->children != NULL)
	ret = xmlNewDocProp(cur->children->doc, cur->name, NULL);
    else
	ret = xmlNewDocProp(NULL, cur->name, NULL);
    if (ret == NULL) return(NULL);
    ret->parent = target;

    if ((cur->ns != NULL) && (target != NULL)) {
      xmlNsPtr ns;

      ns = xmlSearchNs(target->doc, target, cur->ns->prefix);
      if (ns == NULL) {
        /*
         * Humm, we are copying an element whose namespace is defined
         * out of the new tree scope. Search it in the original tree
         * and add it at the top of the new tree
         */
        ns = xmlSearchNs(cur->doc, cur->parent, cur->ns->prefix);
        if (ns != NULL) {
          xmlNodePtr root = target;
          xmlNodePtr pred = NULL;

          while (root->parent != NULL) {
            pred = root;
            root = root->parent;
          }
          if (root == (xmlNodePtr) target->doc) {
            /* correct possibly cycling above the document elt */
            root = pred;
          }
          ret->ns = xmlNewNs(root, ns->href, ns->prefix);
        }
      } else {
        /*
         * we have to find something appropriate here since
         * we cant be sure, that the namespce we found is identified
         * by the prefix
         */
        if (xmlStrEqual(ns->href, cur->ns->href)) {
          /* this is the nice case */
          ret->ns = ns;
        } else {
          /*
           * we are in trouble: we need a new reconcilied namespace.
           * This is expensive
           */
          ret->ns = xmlNewReconciliedNs(target->doc, target, cur->ns);
        }
      }

    } else
        ret->ns = NULL;

    if (cur->children != NULL) {
	xmlNodePtr tmp;

	ret->children = xmlStaticCopyNodeList(cur->children, ret->doc, (xmlNodePtr) ret);
	ret->last = NULL;
	tmp = ret->children;
	while (tmp != NULL) {
	    /* tmp->parent = (xmlNodePtr)ret; */
	    if (tmp->next == NULL)
	        ret->last = tmp;
	    tmp = tmp->next;
	}
    }
    /*
     * Try to handle IDs
     */
    if ((target!= NULL) && (cur!= NULL) &&
	(target->doc != NULL) && (cur->doc != NULL) &&
	(cur->doc->ids != NULL) && (cur->parent != NULL)) {
	if (xmlIsID(cur->doc, cur->parent, cur)) {
	    xmlChar *id;

	    id = xmlNodeListGetString(cur->doc, cur->children, 1);
	    if (id != NULL) {
		xmlAddID(NULL, target->doc, id, ret);
		xmlFree(id);
	    }
	}
    }
    return(ret);
}