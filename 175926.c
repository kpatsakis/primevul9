static void model_to_zval_any(zval *ret, xmlNodePtr node TSRMLS_DC)
{
	zval* any = NULL;
	char* name = NULL;

	while (node != NULL) {
		if (get_zval_property(ret, (char*)node->name TSRMLS_CC) == NULL) {
			zval* val = master_to_zval(get_conversion(XSD_ANYXML), node TSRMLS_CC);
			
			if (any && Z_TYPE_P(any) != IS_ARRAY) {
				/* Convert into array */
				zval *arr;

				MAKE_STD_ZVAL(arr);
				array_init(arr);
				if (name) {
					add_assoc_zval(arr, name, any);
				} else {
					add_next_index_zval(arr, any);
				}
				any = arr;
			}

			if (Z_TYPE_P(val) == IS_STRING && *Z_STRVAL_P(val) == '<') {
				name = NULL;
				while (node->next != NULL) {
					zval* val2 = master_to_zval(get_conversion(XSD_ANYXML), node->next TSRMLS_CC);
					if (Z_TYPE_P(val2) != IS_STRING ||  *Z_STRVAL_P(val) != '<') {
						break;
					}
					add_string_to_string(val, val, val2);
					zval_ptr_dtor(&val2);
					node = node->next;
				}
			} else {
				name = (char*)node->name;
			}

			if (any == NULL) {
				if (name) {
					/* Convert into array */
					zval *arr;

					MAKE_STD_ZVAL(arr);
					array_init(arr);
					add_assoc_zval(arr, name, val);
					any = arr;
					name = NULL;
				} else {
					any = val;
				}
			} else {
				/* Add array element */
				if (name) {
					zval **el;
					if (zend_hash_find(Z_ARRVAL_P(any), name, strlen(name)+1, (void**)&el) == SUCCESS) {
						if (Z_TYPE_PP(el) != IS_ARRAY) {
							/* Convert into array */
							zval *arr;
	
							MAKE_STD_ZVAL(arr);
							array_init(arr);
							add_next_index_zval(arr, *el);
							*el = arr;
						}
						add_next_index_zval(*el, val);
					} else {
						add_assoc_zval(any, name, val);
					}
				} else {
					add_next_index_zval(any, val);
				}
				name = NULL;
			}
		}
		node = node->next;
	}
	if (any) {
		set_zval_property(ret, name ? name : "any", any TSRMLS_CC);
	}
}