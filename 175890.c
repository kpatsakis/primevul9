zval *sdl_guess_convert_zval(encodeTypePtr enc, xmlNodePtr data TSRMLS_DC)
{
	sdlTypePtr type;

	type = enc->sdl_type;
	if (type == NULL) {
		return guess_zval_convert(enc, data TSRMLS_CC);
	}
/*FIXME: restriction support
	if (type && type->restrictions &&
	    data &&  data->children && data->children->content) {
		if (type->restrictions->whiteSpace && type->restrictions->whiteSpace->value) {
			if (strcmp(type->restrictions->whiteSpace->value,"replace") == 0) {
				whiteSpace_replace(data->children->content);
			} else if (strcmp(type->restrictions->whiteSpace->value,"collapse") == 0) {
				whiteSpace_collapse(data->children->content);
			}
		}
		if (type->restrictions->enumeration) {
			if (!zend_hash_exists(type->restrictions->enumeration,data->children->content,strlen(data->children->content)+1)) {
				soap_error1(E_WARNING, "Encoding: Restriction: invalid enumeration value \"%s\"", data->children->content);
			}
		}
		if (type->restrictions->minLength &&
		    strlen(data->children->content) < type->restrictions->minLength->value) {
		  soap_error0(E_WARNING, "Encoding: Restriction: length less than 'minLength'");
		}
		if (type->restrictions->maxLength &&
		    strlen(data->children->content) > type->restrictions->maxLength->value) {
		  soap_error0(E_WARNING, "Encoding: Restriction: length greater than 'maxLength'");
		}
		if (type->restrictions->length &&
		    strlen(data->children->content) != type->restrictions->length->value) {
		  soap_error0(E_WARNING, "Encoding: Restriction: length is not equal to 'length'");
		}
	}
*/
	switch (type->kind) {
		case XSD_TYPEKIND_SIMPLE:
			if (type->encode && enc != &type->encode->details) {
				return master_to_zval_int(type->encode, data TSRMLS_CC);
			} else {
				return guess_zval_convert(enc, data TSRMLS_CC);
			}
			break;
		case XSD_TYPEKIND_LIST:
			return to_zval_list(enc, data TSRMLS_CC);
		case XSD_TYPEKIND_UNION:
			return to_zval_union(enc, data TSRMLS_CC);
		case XSD_TYPEKIND_COMPLEX:
		case XSD_TYPEKIND_RESTRICTION:
		case XSD_TYPEKIND_EXTENSION:
			if (type->encode &&
			    (type->encode->details.type == IS_ARRAY ||
			     type->encode->details.type == SOAP_ENC_ARRAY)) {
				return to_zval_array(enc, data TSRMLS_CC);
			}
			return to_zval_object(enc, data TSRMLS_CC);
		default:
	  	soap_error0(E_ERROR, "Encoding: Internal Error");
			return guess_zval_convert(enc, data TSRMLS_CC);
	}
}