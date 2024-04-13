static zval *master_to_zval_int(encodePtr encode, xmlNodePtr data TSRMLS_DC)
{
	zval *ret = NULL;

	if (SOAP_GLOBAL(typemap)) {
		if (encode->details.type_str) {
			smart_str nscat = {0};
			encodePtr *new_enc;

			if (encode->details.ns) {
				smart_str_appends(&nscat, encode->details.ns);
				smart_str_appendc(&nscat, ':');
			}
			smart_str_appends(&nscat, encode->details.type_str);
			smart_str_0(&nscat);
			if (zend_hash_find(SOAP_GLOBAL(typemap), nscat.c, nscat.len + 1, (void**)&new_enc) == SUCCESS) {
				encode = *new_enc;
			}
			smart_str_free(&nscat);			
		} else {
			xmlAttrPtr type_attr = get_attribute_ex(data->properties,"type", XSI_NAMESPACE);

			if (type_attr != NULL) {
				encodePtr *new_enc;
				xmlNsPtr nsptr;
				char *ns, *cptype;
				smart_str nscat = {0};

				parse_namespace(type_attr->children->content, &cptype, &ns);
				nsptr = xmlSearchNs(data->doc, data, BAD_CAST(ns));
				if (nsptr != NULL) {
					smart_str_appends(&nscat, (char*)nsptr->href);
					smart_str_appendc(&nscat, ':');
				}
				smart_str_appends(&nscat, cptype);
				smart_str_0(&nscat);
				efree(cptype);
				if (ns) {efree(ns);}
				if (zend_hash_find(SOAP_GLOBAL(typemap), nscat.c, nscat.len + 1, (void**)&new_enc) == SUCCESS) {
					encode = *new_enc;
				}
				smart_str_free(&nscat);			
			}
		}
	}
	if (encode->to_zval) {
		ret = encode->to_zval(&encode->details, data TSRMLS_CC);
	}
	return ret;
}