static xmlNodePtr to_xml_any(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret = NULL;

	if (Z_TYPE_P(data) == IS_ARRAY) {
		HashPosition pos;
		zval **el;
		encodePtr enc = get_conversion(XSD_ANYXML);
		char *name;
		uint name_len;
		ulong idx;

		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(data), &pos);
		     zend_hash_get_current_data_ex(Z_ARRVAL_P(data), (void **) &el, &pos) == SUCCESS;
		     zend_hash_move_forward_ex(Z_ARRVAL_P(data), &pos)) {
			ret = master_to_xml(enc, *el, style, parent TSRMLS_CC);
		    if (ret &&
		        ret->name != xmlStringTextNoenc &&
		        zend_hash_get_current_key_ex(Z_ARRVAL_P(data), &name, &name_len, &idx, 0, &pos) == HASH_KEY_IS_STRING) {
				xmlNodeSetName(ret, BAD_CAST(name));
		    }
		}
		return ret;
	}
	if (Z_TYPE_P(data) == IS_STRING) {
		ret = xmlNewTextLen(BAD_CAST(Z_STRVAL_P(data)), Z_STRLEN_P(data));
	} else {
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		ret = xmlNewTextLen(BAD_CAST(Z_STRVAL(tmp)), Z_STRLEN(tmp));
		zval_dtor(&tmp);
	}

	ret->name = xmlStringTextNoenc;
	ret->parent = parent;
	ret->doc = parent->doc;
	ret->prev = parent->last;
	ret->next = NULL;
	if (parent->last) {
		parent->last->next = ret;
	} else {
		parent->children = ret;
	}
	parent->last = ret;

	return ret;
}