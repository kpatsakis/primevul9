zval *master_to_zval(encodePtr encode, xmlNodePtr data TSRMLS_DC)
{
	data = check_and_resolve_href(data);

	if (encode == NULL) {
		encode = get_conversion(UNKNOWN_TYPE);
	} else {
		/* Use xsi:type if it is defined */
		xmlAttrPtr type_attr = get_attribute_ex(data->properties,"type", XSI_NAMESPACE);

		if (type_attr != NULL) {
			encodePtr  enc = get_encoder_from_prefix(SOAP_GLOBAL(sdl), data, type_attr->children->content);

			if (enc != NULL && enc != encode) {
			  encodePtr tmp = enc;
			  while (tmp &&
			         tmp->details.sdl_type != NULL &&
			         tmp->details.sdl_type->kind != XSD_TYPEKIND_COMPLEX) {
			    if (enc == tmp->details.sdl_type->encode ||
			        tmp == tmp->details.sdl_type->encode) {
			    	enc = NULL;
			    	break;
			    }
			    tmp = tmp->details.sdl_type->encode;
			  }
			  if (enc != NULL) {
			    encode = enc;
			  }
			}
		}
	}
	return master_to_zval_int(encode, data TSRMLS_CC);
}