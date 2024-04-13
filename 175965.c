static int is_map(zval *array)
{
	int i, count = zend_hash_num_elements(Z_ARRVAL_P(array));

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(array));
	for (i = 0; i < count; i++) {
		char *str_index;
		ulong num_index;

		if (zend_hash_get_current_key(Z_ARRVAL_P(array), &str_index, &num_index, 0) == HASH_KEY_IS_STRING ||
		    num_index != i) {
			return TRUE;
		}
		zend_hash_move_forward(Z_ARRVAL_P(array));
	}
	return FALSE;
}