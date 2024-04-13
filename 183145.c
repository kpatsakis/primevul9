xmlStringLenGetNodeList(const xmlDoc *doc, const xmlChar *value, int len) {
    xmlNodePtr ret = NULL, last = NULL;
    xmlNodePtr node;
    xmlChar *val;
    const xmlChar *cur = value, *end = cur + len;
    const xmlChar *q;
    xmlEntityPtr ent;
    xmlBufPtr buf;

    if (value == NULL) return(NULL);

    buf = xmlBufCreateSize(0);
    if (buf == NULL) return(NULL);
    xmlBufSetAllocationScheme(buf, XML_BUFFER_ALLOC_HYBRID);

    q = cur;
    while ((cur < end) && (*cur != 0)) {
	if (cur[0] == '&') {
	    int charval = 0;
	    xmlChar tmp;

	    /*
	     * Save the current text.
	     */
            if (cur != q) {
		if (xmlBufAdd(buf, q, cur - q))
		    goto out;
	    }
	    q = cur;
	    if ((cur + 2 < end) && (cur[1] == '#') && (cur[2] == 'x')) {
		cur += 3;
		if (cur < end)
		    tmp = *cur;
		else
		    tmp = 0;
		while (tmp != ';') { /* Non input consuming loop */
		    if ((tmp >= '0') && (tmp <= '9'))
			charval = charval * 16 + (tmp - '0');
		    else if ((tmp >= 'a') && (tmp <= 'f'))
			charval = charval * 16 + (tmp - 'a') + 10;
		    else if ((tmp >= 'A') && (tmp <= 'F'))
			charval = charval * 16 + (tmp - 'A') + 10;
		    else {
			xmlTreeErr(XML_TREE_INVALID_HEX, (xmlNodePtr) doc,
			           NULL);
			charval = 0;
			break;
		    }
		    cur++;
		    if (cur < end)
			tmp = *cur;
		    else
			tmp = 0;
		}
		if (tmp == ';')
		    cur++;
		q = cur;
	    } else if ((cur + 1 < end) && (cur[1] == '#')) {
		cur += 2;
		if (cur < end)
		    tmp = *cur;
		else
		    tmp = 0;
		while (tmp != ';') { /* Non input consuming loops */
		    if ((tmp >= '0') && (tmp <= '9'))
			charval = charval * 10 + (tmp - '0');
		    else {
			xmlTreeErr(XML_TREE_INVALID_DEC, (xmlNodePtr) doc,
			           NULL);
			charval = 0;
			break;
		    }
		    cur++;
		    if (cur < end)
			tmp = *cur;
		    else
			tmp = 0;
		}
		if (tmp == ';')
		    cur++;
		q = cur;
	    } else {
		/*
		 * Read the entity string
		 */
		cur++;
		q = cur;
		while ((cur < end) && (*cur != 0) && (*cur != ';')) cur++;
		if ((cur >= end) || (*cur == 0)) {
		    xmlTreeErr(XML_TREE_UNTERMINATED_ENTITY, (xmlNodePtr) doc,
		               (const char *) q);
		    goto out;
		}
		if (cur != q) {
		    /*
		     * Predefined entities don't generate nodes
		     */
		    val = xmlStrndup(q, cur - q);
		    ent = xmlGetDocEntity(doc, val);
		    if ((ent != NULL) &&
			(ent->etype == XML_INTERNAL_PREDEFINED_ENTITY)) {

			if (xmlBufCat(buf, ent->content))
			    goto out;

		    } else {
			/*
			 * Flush buffer so far
			 */
			if (!xmlBufIsEmpty(buf)) {
			    node = xmlNewDocText(doc, NULL);
			    if (node == NULL) {
				if (val != NULL) xmlFree(val);
				goto out;
			    }
			    node->content = xmlBufDetach(buf);

			    if (last == NULL) {
				last = ret = node;
			    } else {
				last = xmlAddNextSibling(last, node);
			    }
			}

			/*
			 * Create a new REFERENCE_REF node
			 */
			node = xmlNewReference(doc, val);
			if (node == NULL) {
			    if (val != NULL) xmlFree(val);
			    goto out;
			}
			else if ((ent != NULL) && (ent->children == NULL)) {
			    xmlNodePtr temp;

			    ent->children = xmlStringGetNodeList(doc,
				    (const xmlChar*)node->content);
			    ent->owner = 1;
			    temp = ent->children;
			    while (temp) {
				temp->parent = (xmlNodePtr)ent;
				ent->last = temp;
				temp = temp->next;
			    }
			}
			if (last == NULL) {
			    last = ret = node;
			} else {
			    last = xmlAddNextSibling(last, node);
			}
		    }
		    xmlFree(val);
		}
		cur++;
		q = cur;
	    }
	    if (charval != 0) {
		xmlChar buffer[10];
		int l;

		l = xmlCopyCharMultiByte(buffer, charval);
		buffer[l] = 0;

		if (xmlBufCat(buf, buffer))
		    goto out;
		charval = 0;
	    }
	} else
	    cur++;
    }

    if (cur != q) {
        /*
	 * Handle the last piece of text.
	 */
	if (xmlBufAdd(buf, q, cur - q))
	    goto out;
    }

    if (!xmlBufIsEmpty(buf)) {
	node = xmlNewDocText(doc, NULL);
	if (node == NULL) goto out;
	node->content = xmlBufDetach(buf);

	if (last == NULL) {
	    ret = node;
	} else {
	    xmlAddNextSibling(last, node);
	}
    } else if (ret == NULL) {
        ret = xmlNewDocText(doc, BAD_CAST "");
    }

out:
    xmlBufFree(buf);
    return(ret);
}