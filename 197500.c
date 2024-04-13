ZEND_API void zend_check_magic_method_implementation(const zend_class_entry *ce, const zend_function *fptr, int error_type TSRMLS_DC) /* {{{ */
{
	char lcname[16];
	int name_len;

	/* we don't care if the function name is longer, in fact lowercasing only
	 * the beginning of the name speeds up the check process */
	name_len = strlen(fptr->common.function_name);
	zend_str_tolower_copy(lcname, fptr->common.function_name, MIN(name_len, sizeof(lcname)-1));
	lcname[sizeof(lcname)-1] = '\0'; /* zend_str_tolower_copy won't necessarily set the zero byte */

	if (name_len == sizeof(ZEND_DESTRUCTOR_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_DESTRUCTOR_FUNC_NAME, sizeof(ZEND_DESTRUCTOR_FUNC_NAME) - 1) && fptr->common.num_args != 0) {
		zend_error(error_type, "Destructor %s::%s() cannot take arguments", ce->name, ZEND_DESTRUCTOR_FUNC_NAME);
	} else if (name_len == sizeof(ZEND_CLONE_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_CLONE_FUNC_NAME, sizeof(ZEND_CLONE_FUNC_NAME) - 1) && fptr->common.num_args != 0) {
		zend_error(error_type, "Method %s::%s() cannot accept any arguments", ce->name, ZEND_CLONE_FUNC_NAME);
	} else if (name_len == sizeof(ZEND_GET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_GET_FUNC_NAME, sizeof(ZEND_GET_FUNC_NAME) - 1)) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ce->name, ZEND_GET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_GET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_SET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_SET_FUNC_NAME, sizeof(ZEND_SET_FUNC_NAME) - 1)) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::%s() must take exactly 2 arguments", ce->name, ZEND_SET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_SET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_UNSET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_UNSET_FUNC_NAME, sizeof(ZEND_UNSET_FUNC_NAME) - 1)) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ce->name, ZEND_UNSET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_UNSET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_ISSET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_ISSET_FUNC_NAME, sizeof(ZEND_ISSET_FUNC_NAME) - 1)) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ce->name, ZEND_ISSET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_ISSET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_CALL_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME) - 1)) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::%s() must take exactly 2 arguments", ce->name, ZEND_CALL_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_CALL_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_CALLSTATIC_FUNC_NAME) - 1 &&
		!memcmp(lcname, ZEND_CALLSTATIC_FUNC_NAME, sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1)
	) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::%s() must take exactly 2 arguments", ce->name, ZEND_CALLSTATIC_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_CALLSTATIC_FUNC_NAME);
		}
 	} else if (name_len == sizeof(ZEND_TOSTRING_FUNC_NAME) - 1 &&
 		!memcmp(lcname, ZEND_TOSTRING_FUNC_NAME, sizeof(ZEND_TOSTRING_FUNC_NAME)-1) && fptr->common.num_args != 0
	) {
		zend_error(error_type, "Method %s::%s() cannot take arguments", ce->name, ZEND_TOSTRING_FUNC_NAME);
	} else if (name_len == sizeof(ZEND_DEBUGINFO_FUNC_NAME) - 1 &&
		!memcmp(lcname, ZEND_DEBUGINFO_FUNC_NAME, sizeof(ZEND_DEBUGINFO_FUNC_NAME)-1) && fptr->common.num_args != 0) {
		zend_error(error_type, "Method %s::%s() cannot take arguments", ce->name, ZEND_DEBUGINFO_FUNC_NAME);
	}
}