SPL_METHOD(DirectoryIterator, seek)
{
	spl_filesystem_object *intern    = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval                  *retval    = NULL;
	long                   pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pos) == FAILURE) {
		return;
	}

	if (intern->u.dir.index > pos) {
		/* we first rewind */
		zend_call_method_with_0_params(&this_ptr, Z_OBJCE_P(getThis()), &intern->u.dir.func_rewind, "rewind", &retval);
		if (retval) {
			zval_ptr_dtor(&retval);
			retval = NULL;
		}
	}

	while (intern->u.dir.index < pos) {
		int valid = 0;
		zend_call_method_with_0_params(&this_ptr, Z_OBJCE_P(getThis()), &intern->u.dir.func_valid, "valid", &retval);
		if (retval) {
			valid = zend_is_true(retval);
			zval_ptr_dtor(&retval);
			retval = NULL;
		}
		if (!valid) {
			break;
		}
		zend_call_method_with_0_params(&this_ptr, Z_OBJCE_P(getThis()), &intern->u.dir.func_next, "next", &retval);
		if (retval) {
			zval_ptr_dtor(&retval);
		}
	}
} /* }}} */