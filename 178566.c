static inline int object_common2(UNSERIALIZE_PARAMETER, zend_long elements)
{
	zval retval;
	zval fname;

	if (Z_TYPE_P(rval) != IS_OBJECT) {
		return 0;
	}

	//??? TODO: resize before
	if (!process_nested_data(UNSERIALIZE_PASSTHRU, Z_OBJPROP_P(rval), elements, 1)) {
		return 0;
	}

	ZVAL_DEREF(rval);
	if (Z_OBJCE_P(rval) != PHP_IC_ENTRY &&
		zend_hash_str_exists(&Z_OBJCE_P(rval)->function_table, "__wakeup", sizeof("__wakeup")-1)) {
		ZVAL_STRINGL(&fname, "__wakeup", sizeof("__wakeup") - 1);
		BG(serialize_lock)++;
		call_user_function_ex(CG(function_table), rval, &fname, &retval, 0, 0, 1, NULL);
		BG(serialize_lock)--;
		zval_dtor(&fname);
		zval_dtor(&retval);
	}

	if (EG(exception)) {
		return 0;
	}

	return finish_nested_data(UNSERIALIZE_PASSTHRU);

}