htmlNodeDumpFormatOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
	                 xmlNodePtr cur, const char *encoding, int format) {
    xmlNodePtr root;
    xmlAttrPtr attr;
    const htmlElemDesc * info;

    xmlInitParser();

    if ((cur == NULL) || (buf == NULL)) {
	return;
    }

    root = cur;
    while (1) {
        switch (cur->type) {
        case XML_HTML_DOCUMENT_NODE:
        case XML_DOCUMENT_NODE:
            if (((xmlDocPtr) cur)->intSubset != NULL) {
                htmlDtdDumpOutput(buf, (xmlDocPtr) cur, NULL);
            }
            if (cur->children != NULL) {
                cur = cur->children;
                continue;
            }
            break;

        case XML_ELEMENT_NODE:
            /*
             * Get specific HTML info for that node.
             */
            if (cur->ns == NULL)
                info = htmlTagLookup(cur->name);
            else
                info = NULL;

            xmlOutputBufferWriteString(buf, "<");
            if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
                xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
                xmlOutputBufferWriteString(buf, ":");
            }
            xmlOutputBufferWriteString(buf, (const char *)cur->name);
            if (cur->nsDef)
                xmlNsListDumpOutput(buf, cur->nsDef);
            attr = cur->properties;
            while (attr != NULL) {
                htmlAttrDumpOutput(buf, doc, attr, encoding);
                attr = attr->next;
            }

            if ((info != NULL) && (info->empty)) {
                xmlOutputBufferWriteString(buf, ">");
            } else if (cur->children == NULL) {
                if ((info != NULL) && (info->saveEndTag != 0) &&
                    (xmlStrcmp(BAD_CAST info->name, BAD_CAST "html")) &&
                    (xmlStrcmp(BAD_CAST info->name, BAD_CAST "body"))) {
                    xmlOutputBufferWriteString(buf, ">");
                } else {
                    xmlOutputBufferWriteString(buf, "></");
                    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
                        xmlOutputBufferWriteString(buf,
                                (const char *)cur->ns->prefix);
                        xmlOutputBufferWriteString(buf, ":");
                    }
                    xmlOutputBufferWriteString(buf, (const char *)cur->name);
                    xmlOutputBufferWriteString(buf, ">");
                }
            } else {
                xmlOutputBufferWriteString(buf, ">");
                if ((format) && (info != NULL) && (!info->isinline) &&
                    (cur->children->type != HTML_TEXT_NODE) &&
                    (cur->children->type != HTML_ENTITY_REF_NODE) &&
                    (cur->children != cur->last) &&
                    (cur->name != NULL) &&
                    (cur->name[0] != 'p')) /* p, pre, param */
                    xmlOutputBufferWriteString(buf, "\n");
                cur = cur->children;
                continue;
            }

            if ((format) && (cur->next != NULL) &&
                (info != NULL) && (!info->isinline)) {
                if ((cur->next->type != HTML_TEXT_NODE) &&
                    (cur->next->type != HTML_ENTITY_REF_NODE) &&
                    (cur->parent != NULL) &&
                    (cur->parent->name != NULL) &&
                    (cur->parent->name[0] != 'p')) /* p, pre, param */
                    xmlOutputBufferWriteString(buf, "\n");
            }

            break;

        case XML_ATTRIBUTE_NODE:
            htmlAttrDumpOutput(buf, doc, (xmlAttrPtr) cur, encoding);
            break;

        case HTML_TEXT_NODE:
            if (cur->content == NULL)
                break;
            if (((cur->name == (const xmlChar *)xmlStringText) ||
                 (cur->name != (const xmlChar *)xmlStringTextNoenc)) &&
                ((cur->parent == NULL) ||
                 ((xmlStrcasecmp(cur->parent->name, BAD_CAST "script")) &&
                  (xmlStrcasecmp(cur->parent->name, BAD_CAST "style"))))) {
                xmlChar *buffer;

                buffer = xmlEncodeEntitiesReentrant(doc, cur->content);
                if (buffer != NULL) {
                    xmlOutputBufferWriteString(buf, (const char *)buffer);
                    xmlFree(buffer);
                }
            } else {
                xmlOutputBufferWriteString(buf, (const char *)cur->content);
            }
            break;

        case HTML_COMMENT_NODE:
            if (cur->content != NULL) {
                xmlOutputBufferWriteString(buf, "<!--");
                xmlOutputBufferWriteString(buf, (const char *)cur->content);
                xmlOutputBufferWriteString(buf, "-->");
            }
            break;

        case HTML_PI_NODE:
            if (cur->name != NULL) {
                xmlOutputBufferWriteString(buf, "<?");
                xmlOutputBufferWriteString(buf, (const char *)cur->name);
                if (cur->content != NULL) {
                    xmlOutputBufferWriteString(buf, " ");
                    xmlOutputBufferWriteString(buf,
                            (const char *)cur->content);
                }
                xmlOutputBufferWriteString(buf, ">");
            }
            break;

        case HTML_ENTITY_REF_NODE:
            xmlOutputBufferWriteString(buf, "&");
            xmlOutputBufferWriteString(buf, (const char *)cur->name);
            xmlOutputBufferWriteString(buf, ";");
            break;

        case HTML_PRESERVE_NODE:
            if (cur->content != NULL) {
                xmlOutputBufferWriteString(buf, (const char *)cur->content);
            }
            break;

        default:
            break;
        }

        while (1) {
            if (cur == root)
                return;
            if (cur->next != NULL) {
                cur = cur->next;
                break;
            }

            cur = cur->parent;

            if ((cur->type == XML_HTML_DOCUMENT_NODE) ||
                (cur->type == XML_DOCUMENT_NODE)) {
                xmlOutputBufferWriteString(buf, "\n");
            } else {
                if ((format) && (cur->ns == NULL))
                    info = htmlTagLookup(cur->name);
                else
                    info = NULL;

                if ((format) && (info != NULL) && (!info->isinline) &&
                    (cur->last->type != HTML_TEXT_NODE) &&
                    (cur->last->type != HTML_ENTITY_REF_NODE) &&
                    (cur->children != cur->last) &&
                    (cur->name != NULL) &&
                    (cur->name[0] != 'p')) /* p, pre, param */
                    xmlOutputBufferWriteString(buf, "\n");

                xmlOutputBufferWriteString(buf, "</");
                if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
                    xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
                    xmlOutputBufferWriteString(buf, ":");
                }
                xmlOutputBufferWriteString(buf, (const char *)cur->name);
                xmlOutputBufferWriteString(buf, ">");

                if ((format) && (info != NULL) && (!info->isinline) &&
                    (cur->next != NULL)) {
                    if ((cur->next->type != HTML_TEXT_NODE) &&
                        (cur->next->type != HTML_ENTITY_REF_NODE) &&
                        (cur->parent != NULL) &&
                        (cur->parent->name != NULL) &&
                        (cur->parent->name[0] != 'p')) /* p, pre, param */
                        xmlOutputBufferWriteString(buf, "\n");
                }
            }
        }
    }
}