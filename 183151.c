xmlGetNodePath(const xmlNode *node)
{
    const xmlNode *cur, *tmp, *next;
    xmlChar *buffer = NULL, *temp;
    size_t buf_len;
    xmlChar *buf;
    const char *sep;
    const char *name;
    char nametemp[100];
    int occur = 0, generic;

    if ((node == NULL) || (node->type == XML_NAMESPACE_DECL))
        return (NULL);

    buf_len = 500;
    buffer = (xmlChar *) xmlMallocAtomic(buf_len * sizeof(xmlChar));
    if (buffer == NULL) {
	xmlTreeErrMemory("getting node path");
        return (NULL);
    }
    buf = (xmlChar *) xmlMallocAtomic(buf_len * sizeof(xmlChar));
    if (buf == NULL) {
	xmlTreeErrMemory("getting node path");
        xmlFree(buffer);
        return (NULL);
    }

    buffer[0] = 0;
    cur = node;
    do {
        name = "";
        sep = "?";
        occur = 0;
        if ((cur->type == XML_DOCUMENT_NODE) ||
            (cur->type == XML_HTML_DOCUMENT_NODE)) {
            if (buffer[0] == '/')
                break;
            sep = "/";
            next = NULL;
        } else if (cur->type == XML_ELEMENT_NODE) {
	    generic = 0;
            sep = "/";
            name = (const char *) cur->name;
            if (cur->ns) {
		if (cur->ns->prefix != NULL) {
                    snprintf(nametemp, sizeof(nametemp) - 1, "%s:%s",
			(char *)cur->ns->prefix, (char *)cur->name);
		    nametemp[sizeof(nametemp) - 1] = 0;
		    name = nametemp;
		} else {
		    /*
		    * We cannot express named elements in the default
		    * namespace, so use "*".
		    */
		    generic = 1;
		    name = "*";
		}
            }
            next = cur->parent;

            /*
             * Thumbler index computation
	     * TODO: the ocurence test seems bogus for namespaced names
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if ((tmp->type == XML_ELEMENT_NODE) &&
		    (generic ||
		     (xmlStrEqual(cur->name, tmp->name) &&
		     ((tmp->ns == cur->ns) ||
		      ((tmp->ns != NULL) && (cur->ns != NULL) &&
		       (xmlStrEqual(cur->ns->prefix, tmp->ns->prefix)))))))
                    occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
                    if ((tmp->type == XML_ELEMENT_NODE) &&
			(generic ||
			 (xmlStrEqual(cur->name, tmp->name) &&
			 ((tmp->ns == cur->ns) ||
			  ((tmp->ns != NULL) && (cur->ns != NULL) &&
			   (xmlStrEqual(cur->ns->prefix, tmp->ns->prefix)))))))
                        occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;
        } else if (cur->type == XML_COMMENT_NODE) {
            sep = "/";
	    name = "comment()";
            next = cur->parent;

            /*
             * Thumbler index computation
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if (tmp->type == XML_COMMENT_NODE)
		    occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
		  if (tmp->type == XML_COMMENT_NODE)
		    occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;
        } else if ((cur->type == XML_TEXT_NODE) ||
                   (cur->type == XML_CDATA_SECTION_NODE)) {
            sep = "/";
	    name = "text()";
            next = cur->parent;

            /*
             * Thumbler index computation
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if ((tmp->type == XML_TEXT_NODE) ||
		    (tmp->type == XML_CDATA_SECTION_NODE))
		    occur++;
                tmp = tmp->prev;
            }
	    /*
	    * Evaluate if this is the only text- or CDATA-section-node;
	    * if yes, then we'll get "text()", otherwise "text()[1]".
	    */
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL) {
		    if ((tmp->type == XML_TEXT_NODE) ||
			(tmp->type == XML_CDATA_SECTION_NODE))
		    {
			occur = 1;
			break;
		    }
		    tmp = tmp->next;
		}
            } else
                occur++;
        } else if (cur->type == XML_PI_NODE) {
            sep = "/";
	    snprintf(nametemp, sizeof(nametemp) - 1,
		     "processing-instruction('%s')", (char *)cur->name);
            nametemp[sizeof(nametemp) - 1] = 0;
            name = nametemp;

	    next = cur->parent;

            /*
             * Thumbler index computation
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if ((tmp->type == XML_PI_NODE) &&
		    (xmlStrEqual(cur->name, tmp->name)))
                    occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
                    if ((tmp->type == XML_PI_NODE) &&
			(xmlStrEqual(cur->name, tmp->name)))
                        occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;

        } else if (cur->type == XML_ATTRIBUTE_NODE) {
            sep = "/@";
            name = (const char *) (((xmlAttrPtr) cur)->name);
            if (cur->ns) {
	        if (cur->ns->prefix != NULL)
                    snprintf(nametemp, sizeof(nametemp) - 1, "%s:%s",
			(char *)cur->ns->prefix, (char *)cur->name);
		else
		    snprintf(nametemp, sizeof(nametemp) - 1, "%s",
			(char *)cur->name);
                nametemp[sizeof(nametemp) - 1] = 0;
                name = nametemp;
            }
            next = ((xmlAttrPtr) cur)->parent;
        } else {
            next = cur->parent;
        }

        /*
         * Make sure there is enough room
         */
        if (xmlStrlen(buffer) + sizeof(nametemp) + 20 > buf_len) {
            buf_len =
                2 * buf_len + xmlStrlen(buffer) + sizeof(nametemp) + 20;
            temp = (xmlChar *) xmlRealloc(buffer, buf_len);
            if (temp == NULL) {
		xmlTreeErrMemory("getting node path");
                xmlFree(buf);
                xmlFree(buffer);
                return (NULL);
            }
            buffer = temp;
            temp = (xmlChar *) xmlRealloc(buf, buf_len);
            if (temp == NULL) {
		xmlTreeErrMemory("getting node path");
                xmlFree(buf);
                xmlFree(buffer);
                return (NULL);
            }
            buf = temp;
        }
        if (occur == 0)
            snprintf((char *) buf, buf_len, "%s%s%s",
                     sep, name, (char *) buffer);
        else
            snprintf((char *) buf, buf_len, "%s%s[%d]%s",
                     sep, name, occur, (char *) buffer);
        snprintf((char *) buffer, buf_len, "%s", (char *)buf);
        cur = next;
    } while (cur != NULL);
    xmlFree(buf);
    return (buffer);
}