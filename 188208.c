 */
static void php_print_gpcse_array(char *name, uint name_length TSRMLS_DC)
{
	zval **data, **tmp, tmp2;
	char *string_key;
	uint string_len;
	ulong num_key;

	zend_is_auto_global(name, name_length TSRMLS_CC);

	if (zend_hash_find(&EG(symbol_table), name, name_length+1, (void **) &data)!=FAILURE
		&& (Z_TYPE_PP(data)==IS_ARRAY)) {
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(data));
		while (zend_hash_get_current_data(Z_ARRVAL_PP(data), (void **) &tmp) == SUCCESS) {
			if (!sapi_module.phpinfo_as_text) {
				php_info_print("<tr>");
				php_info_print("<td class=\"e\">");
			}

			php_info_print(name);
			php_info_print("[\"");

			switch (zend_hash_get_current_key_ex(Z_ARRVAL_PP(data), &string_key, &string_len, &num_key, 0, NULL)) {
				case HASH_KEY_IS_STRING:
					if (!sapi_module.phpinfo_as_text) {
						php_info_print_html_esc(string_key, string_len-1);
					} else {
						php_info_print(string_key);
					}
					break;
				case HASH_KEY_IS_LONG:
					php_info_printf("%ld", num_key);
					break;
			}
			php_info_print("\"]");
			if (!sapi_module.phpinfo_as_text) {
				php_info_print("</td><td class=\"v\">");
			} else {
				php_info_print(" => ");
			}
			if (Z_TYPE_PP(tmp) == IS_ARRAY) {
				if (!sapi_module.phpinfo_as_text) {
					php_info_print("<pre>");
					zend_print_zval_r_ex((zend_write_func_t) php_info_print_html_esc, *tmp, 0 TSRMLS_CC);
					php_info_print("</pre>");
				} else {
					zend_print_zval_r(*tmp, 0 TSRMLS_CC);
				}
			} else {
				tmp2 = **tmp;
				if (Z_TYPE_PP(tmp) != IS_STRING) {
					tmp = NULL;
					zval_copy_ctor(&tmp2);
					convert_to_string(&tmp2);
				}

				if (!sapi_module.phpinfo_as_text) {
					if (Z_STRLEN(tmp2) == 0) {
						php_info_print("<i>no value</i>");
					} else {
						php_info_print_html_esc(Z_STRVAL(tmp2), Z_STRLEN(tmp2));
					}
				} else {
					php_info_print(Z_STRVAL(tmp2));
				}

				if (!tmp) {
					zval_dtor(&tmp2);
				}
			}
			if (!sapi_module.phpinfo_as_text) {
				php_info_print("</td></tr>\n");
			} else {
				php_info_print("\n");
			}
			zend_hash_move_forward(Z_ARRVAL_PP(data));
		}
	}