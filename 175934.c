xmlNsPtr encode_add_ns(xmlNodePtr node, const char* ns)
{
	xmlNsPtr xmlns;

	if (ns == NULL) {
	  return NULL;
	}

	xmlns = xmlSearchNsByHref(node->doc, node, BAD_CAST(ns));
	if (xmlns != NULL && xmlns->prefix == NULL) {
		xmlns = xmlSearchNsPrefixByHref(node->doc, node, BAD_CAST(ns));
	}
	if (xmlns == NULL) {
		xmlChar* prefix;
        TSRMLS_FETCH();

		if (zend_hash_find(&SOAP_GLOBAL(defEncNs), (char*)ns, strlen(ns) + 1, (void **)&prefix) == SUCCESS) {
			xmlns = xmlNewNs(node->doc->children, BAD_CAST(ns), prefix);
		} else {
			smart_str prefix = {0};
			int num = ++SOAP_GLOBAL(cur_uniq_ns);

			while (1) {
				smart_str_appendl(&prefix, "ns", 2);
				smart_str_append_long(&prefix, num);
				smart_str_0(&prefix);
				if (xmlSearchNs(node->doc, node, BAD_CAST(prefix.c)) == NULL) {
					break;
				}
				smart_str_free(&prefix);
				prefix.c = NULL;
				prefix.len = 0;
				num = ++SOAP_GLOBAL(cur_uniq_ns);
			}

			xmlns = xmlNewNs(node->doc->children, BAD_CAST(ns), BAD_CAST(prefix.c));
			smart_str_free(&prefix);
		}
	}
	return xmlns;
}