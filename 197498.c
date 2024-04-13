static int parse_arg_object_to_string(zval **arg, char **p, int *pl, int type TSRMLS_DC) /* {{{ */
{
	if (Z_OBJ_HANDLER_PP(arg, cast_object)) {
		zval *obj;
		MAKE_STD_ZVAL(obj);
		if (Z_OBJ_HANDLER_P(*arg, cast_object)(*arg, obj, type TSRMLS_CC) == SUCCESS) {
			zval_ptr_dtor(arg);
			*arg = obj;
			*pl = Z_STRLEN_PP(arg);
			*p = Z_STRVAL_PP(arg);
			return SUCCESS;
		}
		efree(obj);
	}
	/* Standard PHP objects */
	if (Z_OBJ_HT_PP(arg) == &std_object_handlers || !Z_OBJ_HANDLER_PP(arg, cast_object)) {
		SEPARATE_ZVAL_IF_NOT_REF(arg);
		if (zend_std_cast_object_tostring(*arg, *arg, type TSRMLS_CC) == SUCCESS) {
			*pl = Z_STRLEN_PP(arg);
			*p = Z_STRVAL_PP(arg);
			return SUCCESS;
		}
	}
	if (!Z_OBJ_HANDLER_PP(arg, cast_object) && Z_OBJ_HANDLER_PP(arg, get)) {
		int use_copy;
		zval *z = Z_OBJ_HANDLER_PP(arg, get)(*arg TSRMLS_CC);
		Z_ADDREF_P(z);
		if(Z_TYPE_P(z) != IS_OBJECT) {
			zval_dtor(*arg);
			Z_TYPE_P(*arg) = IS_NULL;
			zend_make_printable_zval(z, *arg, &use_copy);
			if (!use_copy) {
				ZVAL_ZVAL(*arg, z, 1, 1);
			}
			*pl = Z_STRLEN_PP(arg);
			*p = Z_STRVAL_PP(arg);
			return SUCCESS;
		}
		zval_ptr_dtor(&z);
	}
	return FAILURE;
}