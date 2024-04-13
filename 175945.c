xmlNodePtr sdl_guess_convert_xml(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	sdlTypePtr type;
	xmlNodePtr ret = NULL;

	type = enc->sdl_type;

	if (type == NULL) {
		ret = guess_xml_convert(enc, data, style, parent TSRMLS_CC);
		if (style == SOAP_ENCODED) {
			set_ns_and_type(ret, enc);
		}
		return ret;
	}
/*FIXME: restriction support
	if (type) {
		if (type->restrictions && Z_TYPE_P(data) == IS_STRING) {
			if (type->restrictions->enumeration) {
				if (!zend_hash_exists(type->restrictions->enumeration,Z_STRVAL_P(data),Z_STRLEN_P(data)+1)) {
					soap_error1(E_WARNING, "Encoding: Restriction: invalid enumeration value \"%s\".", Z_STRVAL_P(data));
				}
			}
			if (type->restrictions->minLength &&
			    Z_STRLEN_P(data) < type->restrictions->minLength->value) {
		  	soap_error0(E_WARNING, "Encoding: Restriction: length less than 'minLength'");
			}
			if (type->restrictions->maxLength &&
			    Z_STRLEN_P(data) > type->restrictions->maxLength->value) {
		  	soap_error0(E_WARNING, "Encoding: Restriction: length greater than 'maxLength'");
			}
			if (type->restrictions->length &&
			    Z_STRLEN_P(data) != type->restrictions->length->value) {
		  	soap_error0(E_WARNING, "Encoding: Restriction: length is not equal to 'length'");
			}
		}
	}
*/
	switch(type->kind) {
		case XSD_TYPEKIND_SIMPLE:
			if (type->encode && enc != &type->encode->details) {
				ret = master_to_xml(type->encode, data, style, parent TSRMLS_CC);
			} else {
				ret = guess_xml_convert(enc, data, style, parent TSRMLS_CC);
			}
			break;
		case XSD_TYPEKIND_LIST:
			ret = to_xml_list(enc, data, style, parent TSRMLS_CC);
			break;
		case XSD_TYPEKIND_UNION:
			ret = to_xml_union(enc, data, style, parent TSRMLS_CC);
			break;
		case XSD_TYPEKIND_COMPLEX:
		case XSD_TYPEKIND_RESTRICTION:
		case XSD_TYPEKIND_EXTENSION:
			if (type->encode &&
			    (type->encode->details.type == IS_ARRAY ||
			     type->encode->details.type == SOAP_ENC_ARRAY)) {
				return to_xml_array(enc, data, style, parent TSRMLS_CC);
			} else {
				return to_xml_object(enc, data, style, parent TSRMLS_CC);
			}
			break;
		default:
	  	soap_error0(E_ERROR, "Encoding: Internal Error");
			break;
	}
	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, enc);
	}
	return ret;
}