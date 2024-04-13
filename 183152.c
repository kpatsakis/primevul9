xmlNewPropInternal(xmlNodePtr node, xmlNsPtr ns,
                   const xmlChar * name, const xmlChar * value,
                   int eatname)
{
    xmlAttrPtr cur;
    xmlDocPtr doc = NULL;

    if ((node != NULL) && (node->type != XML_ELEMENT_NODE)) {
        if ((eatname == 1) &&
	    ((node->doc == NULL) ||
	     (!(xmlDictOwns(node->doc->dict, name)))))
            xmlFree((xmlChar *) name);
        return (NULL);
    }

    /*
     * Allocate a new property and fill the fields.
     */
    cur = (xmlAttrPtr) xmlMalloc(sizeof(xmlAttr));
    if (cur == NULL) {
        if ((eatname == 1) &&
	    ((node == NULL) || (node->doc == NULL) ||
	     (!(xmlDictOwns(node->doc->dict, name)))))
            xmlFree((xmlChar *) name);
        xmlTreeErrMemory("building attribute");
        return (NULL);
    }
    memset(cur, 0, sizeof(xmlAttr));
    cur->type = XML_ATTRIBUTE_NODE;

    cur->parent = node;
    if (node != NULL) {
        doc = node->doc;
        cur->doc = doc;
    }
    cur->ns = ns;

    if (eatname == 0) {
        if ((doc != NULL) && (doc->dict != NULL))
            cur->name = (xmlChar *) xmlDictLookup(doc->dict, name, -1);
        else
            cur->name = xmlStrdup(name);
    } else
        cur->name = name;

    if (value != NULL) {
        xmlNodePtr tmp;

        if(!xmlCheckUTF8(value)) {
            xmlTreeErr(XML_TREE_NOT_UTF8, (xmlNodePtr) doc,
                       NULL);
            if (doc != NULL)
                doc->encoding = xmlStrdup(BAD_CAST "ISO-8859-1");
        }
        cur->children = xmlNewDocText(doc, value);
        cur->last = NULL;
        tmp = cur->children;
        while (tmp != NULL) {
            tmp->parent = (xmlNodePtr) cur;
            if (tmp->next == NULL)
                cur->last = tmp;
            tmp = tmp->next;
        }
    }

    /*
     * Add it at the end to preserve parsing order ...
     */
    if (node != NULL) {
        if (node->properties == NULL) {
            node->properties = cur;
        } else {
            xmlAttrPtr prev = node->properties;

            while (prev->next != NULL)
                prev = prev->next;
            prev->next = cur;
            cur->prev = prev;
        }
    }

    if ((value != NULL) && (node != NULL) &&
        (xmlIsID(node->doc, node, cur) == 1))
        xmlAddID(NULL, node->doc, value, cur);

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
        xmlRegisterNodeDefaultValue((xmlNodePtr) cur);
    return (cur);
}