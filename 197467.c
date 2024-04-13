static int zend_is_callable_check_class(const char *name, int name_len, zend_fcall_info_cache *fcc, int *strict_class, char **error TSRMLS_DC) /* {{{ */
{
	int ret = 0;
	zend_class_entry **pce;
	char *lcname = zend_str_tolower_dup(name, name_len);

	*strict_class = 0;
	if (name_len == sizeof("self") - 1 &&
	    !memcmp(lcname, "self", sizeof("self") - 1)) {
		if (!EG(scope)) {
			if (error) *error = estrdup("cannot access self:: when no class scope is active");
		} else {
			fcc->called_scope = EG(called_scope);
			fcc->calling_scope = EG(scope);
			if (!fcc->object_ptr) {
				fcc->object_ptr = EG(This);
			}
			ret = 1;
		}
	} else if (name_len == sizeof("parent") - 1 &&
		       !memcmp(lcname, "parent", sizeof("parent") - 1)) {
		if (!EG(scope)) {
			if (error) *error = estrdup("cannot access parent:: when no class scope is active");
		} else if (!EG(scope)->parent) {
			if (error) *error = estrdup("cannot access parent:: when current class scope has no parent");
		} else {
			fcc->called_scope = EG(called_scope);
			fcc->calling_scope = EG(scope)->parent;
			if (!fcc->object_ptr) {
				fcc->object_ptr = EG(This);
			}
			*strict_class = 1;
			ret = 1;
		}
	} else if (name_len == sizeof("static") - 1 &&
	           !memcmp(lcname, "static", sizeof("static") - 1)) {
		if (!EG(called_scope)) {
			if (error) *error = estrdup("cannot access static:: when no class scope is active");
		} else {
			fcc->called_scope = EG(called_scope);
			fcc->calling_scope = EG(called_scope);
			if (!fcc->object_ptr) {
				fcc->object_ptr = EG(This);
			}
			*strict_class = 1;
			ret = 1;
		}
	} else if (zend_lookup_class_ex(name, name_len, NULL, 1, &pce TSRMLS_CC) == SUCCESS) {
		zend_class_entry *scope = EG(active_op_array) ? EG(active_op_array)->scope : NULL;

		fcc->calling_scope = *pce;
		if (scope && !fcc->object_ptr && EG(This) &&
		    instanceof_function(Z_OBJCE_P(EG(This)), scope TSRMLS_CC) &&
		    instanceof_function(scope, fcc->calling_scope TSRMLS_CC)) {
			fcc->object_ptr = EG(This);
			fcc->called_scope = Z_OBJCE_P(fcc->object_ptr);
		} else {
			fcc->called_scope = fcc->object_ptr ? Z_OBJCE_P(fcc->object_ptr) : fcc->calling_scope;
		}
		*strict_class = 1;
		ret = 1;
	} else {
		if (error) zend_spprintf(error, 0, "class '%.*s' not found", name_len, name);
	}
	efree(lcname);
	return ret;
}