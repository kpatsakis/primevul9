static zend_bool soap_check_zval_ref(zval *data, xmlNodePtr node TSRMLS_DC) {
	xmlNodePtr *node_ptr;

	if (SOAP_GLOBAL(ref_map)) {
		if (Z_TYPE_P(data) == IS_OBJECT) {
			data = (zval*)zend_objects_get_address(data TSRMLS_CC);
		}
		if (zend_hash_index_find(SOAP_GLOBAL(ref_map), (ulong)data, (void**)&node_ptr) == SUCCESS) {
			xmlAttrPtr attr = (*node_ptr)->properties;
			char *id;
			smart_str prefix = {0};

			if (*node_ptr == node) {
				return 0;
			}
			xmlNodeSetName(node, (*node_ptr)->name);
			xmlSetNs(node, (*node_ptr)->ns);
			if (SOAP_GLOBAL(soap_version) == SOAP_1_1) {
				while (1) {
					attr = get_attribute(attr, "id");
					if (attr == NULL || attr->ns == NULL) {
						break;
					}
					attr = attr->next;
				}
				if (attr) {
					id = (char*)attr->children->content;
					smart_str_appendc(&prefix, '#');
					smart_str_appends(&prefix, id);
					smart_str_0(&prefix);
					id = prefix.c;
				} else {
					SOAP_GLOBAL(cur_uniq_ref)++;
					smart_str_appendl(&prefix, "#ref", 4);
					smart_str_append_long(&prefix, SOAP_GLOBAL(cur_uniq_ref));
					smart_str_0(&prefix);
					id = prefix.c;
					xmlSetProp((*node_ptr), BAD_CAST("id"), BAD_CAST(id+1));
				}
				xmlSetProp(node, BAD_CAST("href"), BAD_CAST(id));
			} else {
				attr = get_attribute_ex(attr, "id", SOAP_1_2_ENC_NAMESPACE);
				if (attr) {
					id = (char*)attr->children->content;
					smart_str_appendc(&prefix, '#');
					smart_str_appends(&prefix, id);
					smart_str_0(&prefix);
					id = prefix.c;
				} else {
					SOAP_GLOBAL(cur_uniq_ref)++;
					smart_str_appendl(&prefix, "#ref", 4);
					smart_str_append_long(&prefix, SOAP_GLOBAL(cur_uniq_ref));
					smart_str_0(&prefix);
					id = prefix.c;
					set_ns_prop((*node_ptr), SOAP_1_2_ENC_NAMESPACE, "id", id+1);
				}
				set_ns_prop(node, SOAP_1_2_ENC_NAMESPACE, "ref", id);
			}
			smart_str_free(&prefix);
			return 1;
		} else {
			zend_hash_index_update(SOAP_GLOBAL(ref_map), (ulong)data, (void**)&node, sizeof(xmlNodePtr), NULL);
		}
	}
	return 0;
}