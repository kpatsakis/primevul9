
static void php_info_print_stream_hash(const char *name, HashTable *ht TSRMLS_DC) /* {{{ */
{
	char *key;
	uint len;

	if (ht) {
		if (zend_hash_num_elements(ht)) {
			HashPosition pos;

			if (!sapi_module.phpinfo_as_text) {
				php_info_printf("<tr><td class=\"e\">Registered %s</td><td class=\"v\">", name);
			} else {
				php_info_printf("\nRegistered %s => ", name);
			}

			zend_hash_internal_pointer_reset_ex(ht, &pos);
			while (zend_hash_get_current_key_ex(ht, &key, &len, NULL, 0, &pos) == HASH_KEY_IS_STRING)
			{
				if (!sapi_module.phpinfo_as_text) {
					php_info_print_html_esc(key, len-1);
				} else {
					php_info_print(key);
				}
				zend_hash_move_forward_ex(ht, &pos);
				if (zend_hash_get_current_key_ex(ht, &key, &len, NULL, 0, &pos) == HASH_KEY_IS_STRING) {
					php_info_print(", ");
				} else {
					break;
				}
			}

			if (!sapi_module.phpinfo_as_text) {
				php_info_print("</td></tr>\n");
			}
		} else {
			char reg_name[128];
			snprintf(reg_name, sizeof(reg_name), "Registered %s", name);
			php_info_print_table_row(2, reg_name, "none registered");
		}
	} else {
		php_info_print_table_row(2, name, "disabled");
	}