static xmlNodePtr check_and_resolve_href(xmlNodePtr data)
{
	if (data && data->properties) {
		xmlAttrPtr href;

		href = data->properties;
		while (1) {
			href = get_attribute(href, "href");
			if (href == NULL || href->ns == NULL) {break;}
			href = href->next;
		}
		if (href) {
			/*  Internal href try and find node */
			if (href->children->content[0] == '#') {
				xmlNodePtr ret = get_node_with_attribute_recursive(data->doc->children, NULL, "id", (char*)&href->children->content[1]);
				if (!ret) {
					soap_error1(E_ERROR, "Encoding: Unresolved reference '%s'", href->children->content);
				}
				return ret;
			} else {
				/*  TODO: External href....? */
				soap_error1(E_ERROR, "Encoding: External reference '%s'", href->children->content);
			}
		}
		/* SOAP 1.2 enc:id enc:ref */
		href = get_attribute_ex(data->properties, "ref", SOAP_1_2_ENC_NAMESPACE);
		if (href) {
			xmlChar* id;
			xmlNodePtr ret;

			if (href->children->content[0] == '#') {
				id = href->children->content+1;
			} else {
				id = href->children->content;
			}
			ret = get_node_with_attribute_recursive_ex(data->doc->children, NULL, NULL, "id", (char*)id, SOAP_1_2_ENC_NAMESPACE);
			if (!ret) {
				soap_error1(E_ERROR, "Encoding: Unresolved reference '%s'", href->children->content);
			} else if (ret == data) {
				soap_error1(E_ERROR, "Encoding: Violation of id and ref information items '%s'", href->children->content);
			}
			return ret;
		}
	}
	return data;
}