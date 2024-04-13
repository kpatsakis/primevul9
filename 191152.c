htmlSetMetaEncoding(htmlDocPtr doc, const xmlChar *encoding) {
    htmlNodePtr cur, meta = NULL, head = NULL;
    const xmlChar *content = NULL;
    char newcontent[100];

    newcontent[0] = 0;

    if (doc == NULL)
	return(-1);

    /* html isn't a real encoding it's just libxml2 way to get entities */
    if (!xmlStrcasecmp(encoding, BAD_CAST "html"))
        return(-1);

    if (encoding != NULL) {
	snprintf(newcontent, sizeof(newcontent), "text/html; charset=%s",
                (char *)encoding);
	newcontent[sizeof(newcontent) - 1] = 0;
    }

    cur = doc->children;

    /*
     * Search the html
     */
    while (cur != NULL) {
	if ((cur->type == XML_ELEMENT_NODE) && (cur->name != NULL)) {
	    if (xmlStrcasecmp(cur->name, BAD_CAST"html") == 0)
		break;
	    if (xmlStrcasecmp(cur->name, BAD_CAST"head") == 0)
		goto found_head;
	    if (xmlStrcasecmp(cur->name, BAD_CAST"meta") == 0)
		goto found_meta;
	}
	cur = cur->next;
    }
    if (cur == NULL)
	return(-1);
    cur = cur->children;

    /*
     * Search the head
     */
    while (cur != NULL) {
	if ((cur->type == XML_ELEMENT_NODE) && (cur->name != NULL)) {
	    if (xmlStrcasecmp(cur->name, BAD_CAST"head") == 0)
		break;
	    if (xmlStrcasecmp(cur->name, BAD_CAST"meta") == 0) {
                head = cur->parent;
		goto found_meta;
            }
	}
	cur = cur->next;
    }
    if (cur == NULL)
	return(-1);
found_head:
    head = cur;
    if (cur->children == NULL)
        goto create;
    cur = cur->children;

found_meta:
    /*
     * Search and update all the remaining the meta elements carrying
     * encoding information
     */
    while (cur != NULL) {
	if ((cur->type == XML_ELEMENT_NODE) && (cur->name != NULL)) {
	    if (xmlStrcasecmp(cur->name, BAD_CAST"meta") == 0) {
		xmlAttrPtr attr = cur->properties;
		int http;
		const xmlChar *value;

		content = NULL;
		http = 0;
		while (attr != NULL) {
		    if ((attr->children != NULL) &&
		        (attr->children->type == XML_TEXT_NODE) &&
		        (attr->children->next == NULL)) {
			value = attr->children->content;
			if ((!xmlStrcasecmp(attr->name, BAD_CAST"http-equiv"))
			 && (!xmlStrcasecmp(value, BAD_CAST"Content-Type")))
			    http = 1;
			else
                        {
                           if ((value != NULL) &&
                               (!xmlStrcasecmp(attr->name, BAD_CAST"content")))
			       content = value;
                        }
		        if ((http != 0) && (content != NULL))
			    break;
		    }
		    attr = attr->next;
		}
		if ((http != 0) && (content != NULL)) {
		    meta = cur;
		    break;
		}

	    }
	}
	cur = cur->next;
    }
create:
    if (meta == NULL) {
        if ((encoding != NULL) && (head != NULL)) {
            /*
             * Create a new Meta element with the right attributes
             */

            meta = xmlNewDocNode(doc, NULL, BAD_CAST"meta", NULL);
            if (head->children == NULL)
                xmlAddChild(head, meta);
            else
                xmlAddPrevSibling(head->children, meta);
            xmlNewProp(meta, BAD_CAST"http-equiv", BAD_CAST"Content-Type");
            xmlNewProp(meta, BAD_CAST"content", BAD_CAST newcontent);
        }
    } else {
        /* remove the meta tag if NULL is passed */
        if (encoding == NULL) {
            xmlUnlinkNode(meta);
            xmlFreeNode(meta);
        }
        /* change the document only if there is a real encoding change */
        else if (xmlStrcasestr(content, encoding) == NULL) {
            xmlSetProp(meta, BAD_CAST"content", BAD_CAST newcontent);
        }
    }


    return(0);
}