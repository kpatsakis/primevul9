static int spl_filesystem_object_cast(zval *readobj, zval *writeobj, int type TSRMLS_DC)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(readobj TSRMLS_CC);

	if (type == IS_STRING) {
		if (Z_OBJCE_P(readobj)->__tostring) {
			return std_object_handlers.cast_object(readobj, writeobj, type TSRMLS_CC);
		}

		switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			if (readobj == writeobj) {
				zval retval;
				zval *retval_ptr = &retval;

				ZVAL_STRINGL(retval_ptr, intern->file_name, intern->file_name_len, 1);
				zval_dtor(readobj);
				ZVAL_ZVAL(writeobj, retval_ptr, 0, 0);
			} else {
				ZVAL_STRINGL(writeobj, intern->file_name, intern->file_name_len, 1);
			}
			return SUCCESS;
		case SPL_FS_DIR:
			if (readobj == writeobj) {
				zval retval;
				zval *retval_ptr = &retval;

				ZVAL_STRING(retval_ptr, intern->u.dir.entry.d_name, 1);
				zval_dtor(readobj);
				ZVAL_ZVAL(writeobj, retval_ptr, 0, 0);
			} else {
				ZVAL_STRING(writeobj, intern->u.dir.entry.d_name, 1);
			}
			return SUCCESS;
		}
	} else if (type == IS_BOOL) {
		ZVAL_BOOL(writeobj, 1);
		return SUCCESS;
	}
	if (readobj == writeobj) {
		zval_dtor(readobj);
	}
	ZVAL_NULL(writeobj);
	return FAILURE;
}