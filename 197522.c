static int zend_is_callable_check_func(int check_flags, zval *callable, zend_fcall_info_cache *fcc, int strict_class, char **error TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce_org = fcc->calling_scope;
	int retval = 0;
	char *mname, *lmname;
	const char *colon;
	int clen, mlen;
	zend_class_entry *last_scope;
	HashTable *ftable;
	int call_via_handler = 0;

	if (error) {
		*error = NULL;
	}

	fcc->calling_scope = NULL;
	fcc->function_handler = NULL;

	if (!ce_org) {
		/* Skip leading \ */
		if (Z_STRVAL_P(callable)[0] == '\\') {
			mlen = Z_STRLEN_P(callable) - 1;
			lmname = zend_str_tolower_dup(Z_STRVAL_P(callable) + 1, mlen);
		} else {
			mlen = Z_STRLEN_P(callable);
			lmname = zend_str_tolower_dup(Z_STRVAL_P(callable), mlen);
		}
		/* Check if function with given name exists.
		 * This may be a compound name that includes namespace name */
		if (zend_hash_find(EG(function_table), lmname, mlen+1, (void**)&fcc->function_handler) == SUCCESS) {
			efree(lmname);
			return 1;
		}
		efree(lmname);
	}

	/* Split name into class/namespace and method/function names */
	if ((colon = zend_memrchr(Z_STRVAL_P(callable), ':', Z_STRLEN_P(callable))) != NULL &&
		colon > Z_STRVAL_P(callable) &&
		*(colon-1) == ':'
	) {
		colon--;
		clen = colon - Z_STRVAL_P(callable);
		mlen = Z_STRLEN_P(callable) - clen - 2;

		if (colon == Z_STRVAL_P(callable)) {
			if (error) zend_spprintf(error, 0, "invalid function name");
			return 0;
		}

		/* This is a compound name.
		 * Try to fetch class and then find static method. */
		last_scope = EG(scope);
		if (ce_org) {
			EG(scope) = ce_org;
		}

		if (!zend_is_callable_check_class(Z_STRVAL_P(callable), clen, fcc, &strict_class, error TSRMLS_CC)) {
			EG(scope) = last_scope;
			return 0;
		}
		EG(scope) = last_scope;

		ftable = &fcc->calling_scope->function_table;
		if (ce_org && !instanceof_function(ce_org, fcc->calling_scope TSRMLS_CC)) {
			if (error) zend_spprintf(error, 0, "class '%s' is not a subclass of '%s'", ce_org->name, fcc->calling_scope->name);
			return 0;
		}
		mname = Z_STRVAL_P(callable) + clen + 2;
	} else if (ce_org) {
		/* Try to fetch find static method of given class. */
		mlen = Z_STRLEN_P(callable);
		mname = Z_STRVAL_P(callable);
		ftable = &ce_org->function_table;
		fcc->calling_scope = ce_org;
	} else {
		/* We already checked for plain function before. */
		if (error && !(check_flags & IS_CALLABLE_CHECK_SILENT)) {
			zend_spprintf(error, 0, "function '%s' not found or invalid function name", Z_STRVAL_P(callable));
		}
		return 0;
	}

	lmname = zend_str_tolower_dup(mname, mlen);
	if (strict_class &&
	    fcc->calling_scope &&
	    mlen == sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1 &&
	    !memcmp(lmname, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME) - 1)) {
		fcc->function_handler = fcc->calling_scope->constructor;
		if (fcc->function_handler) {
			retval = 1;
		}
	} else if (zend_hash_find(ftable, lmname, mlen+1, (void**)&fcc->function_handler) == SUCCESS) {
		retval = 1;
		if ((fcc->function_handler->op_array.fn_flags & ZEND_ACC_CHANGED) &&
		    !strict_class && EG(scope) &&
		    instanceof_function(fcc->function_handler->common.scope, EG(scope) TSRMLS_CC)) {
			zend_function *priv_fbc;

			if (zend_hash_find(&EG(scope)->function_table, lmname, mlen+1, (void **) &priv_fbc)==SUCCESS
				&& priv_fbc->common.fn_flags & ZEND_ACC_PRIVATE
				&& priv_fbc->common.scope == EG(scope)) {
				fcc->function_handler = priv_fbc;
			}
		}
		if ((check_flags & IS_CALLABLE_CHECK_NO_ACCESS) == 0 &&
		    (fcc->calling_scope &&
		     ((fcc->object_ptr && fcc->calling_scope->__call) ||
		      (!fcc->object_ptr && fcc->calling_scope->__callstatic)))) {
			if (fcc->function_handler->op_array.fn_flags & ZEND_ACC_PRIVATE) {
				if (!zend_check_private(fcc->function_handler, fcc->object_ptr ? Z_OBJCE_P(fcc->object_ptr) : EG(scope), lmname, mlen TSRMLS_CC)) {
					retval = 0;
					fcc->function_handler = NULL;
					goto get_function_via_handler;
				}
			} else if (fcc->function_handler->common.fn_flags & ZEND_ACC_PROTECTED) {
				if (!zend_check_protected(fcc->function_handler->common.scope, EG(scope))) {
					retval = 0;
					fcc->function_handler = NULL;
					goto get_function_via_handler;
				}
			}
		}
	} else {
get_function_via_handler:
		if (fcc->object_ptr && fcc->calling_scope == ce_org) {
			if (strict_class && ce_org->__call) {
				fcc->function_handler = emalloc(sizeof(zend_internal_function));
				fcc->function_handler->internal_function.type = ZEND_INTERNAL_FUNCTION;
				fcc->function_handler->internal_function.module = (ce_org->type == ZEND_INTERNAL_CLASS) ? ce_org->info.internal.module : NULL;
				fcc->function_handler->internal_function.handler = zend_std_call_user_call;
				fcc->function_handler->internal_function.arg_info = NULL;
				fcc->function_handler->internal_function.num_args = 0;
				fcc->function_handler->internal_function.scope = ce_org;
				fcc->function_handler->internal_function.fn_flags = ZEND_ACC_CALL_VIA_HANDLER;
				fcc->function_handler->internal_function.function_name = estrndup(mname, mlen);
				call_via_handler = 1;
				retval = 1;
			} else if (Z_OBJ_HT_P(fcc->object_ptr)->get_method) {
				fcc->function_handler = Z_OBJ_HT_P(fcc->object_ptr)->get_method(&fcc->object_ptr, mname, mlen, NULL TSRMLS_CC);
				if (fcc->function_handler) {
					if (strict_class &&
					    (!fcc->function_handler->common.scope ||
					     !instanceof_function(ce_org, fcc->function_handler->common.scope TSRMLS_CC))) {
						if ((fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) != 0) {
							if (fcc->function_handler->type != ZEND_OVERLOADED_FUNCTION) {
								efree((char*)fcc->function_handler->common.function_name);
							}
							efree(fcc->function_handler);
						}
					} else {
						retval = 1;
						call_via_handler = (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) != 0;
					}
				}
			}
		} else if (fcc->calling_scope) {
			if (fcc->calling_scope->get_static_method) {
				fcc->function_handler = fcc->calling_scope->get_static_method(fcc->calling_scope, mname, mlen TSRMLS_CC);
			} else {
				fcc->function_handler = zend_std_get_static_method(fcc->calling_scope, mname, mlen, NULL TSRMLS_CC);
			}
			if (fcc->function_handler) {
				retval = 1;
				call_via_handler = (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) != 0;
				if (call_via_handler && !fcc->object_ptr && EG(This) &&
				    Z_OBJ_HT_P(EG(This))->get_class_entry &&
				    instanceof_function(Z_OBJCE_P(EG(This)), fcc->calling_scope TSRMLS_CC)) {
					fcc->object_ptr = EG(This);
				}
			}
		}
	}

	if (retval) {
		if (fcc->calling_scope && !call_via_handler) {
			if (!fcc->object_ptr && (fcc->function_handler->common.fn_flags & ZEND_ACC_ABSTRACT)) {
				if (error) {
					zend_spprintf(error, 0, "cannot call abstract method %s::%s()", fcc->calling_scope->name, fcc->function_handler->common.function_name);
					retval = 0;
				} else {
					zend_error(E_ERROR, "Cannot call abstract method %s::%s()", fcc->calling_scope->name, fcc->function_handler->common.function_name);
				}
			} else if (!fcc->object_ptr && !(fcc->function_handler->common.fn_flags & ZEND_ACC_STATIC)) {
				int severity;
				char *verb;
				if (fcc->function_handler->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
					severity = E_STRICT;
					verb = "should not";
				} else {
					/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
					severity = E_ERROR;
					verb = "cannot";
				}
				if ((check_flags & IS_CALLABLE_CHECK_IS_STATIC) != 0) {
					retval = 0;
				}
				if (EG(This) && instanceof_function(Z_OBJCE_P(EG(This)), fcc->calling_scope TSRMLS_CC)) {
					fcc->object_ptr = EG(This);
					if (error) {
						zend_spprintf(error, 0, "non-static method %s::%s() %s be called statically, assuming $this from compatible context %s", fcc->calling_scope->name, fcc->function_handler->common.function_name, verb, Z_OBJCE_P(EG(This))->name);
						if (severity == E_ERROR) {
							retval = 0;
						}
					} else if (retval) {
						zend_error(severity, "Non-static method %s::%s() %s be called statically, assuming $this from compatible context %s", fcc->calling_scope->name, fcc->function_handler->common.function_name, verb, Z_OBJCE_P(EG(This))->name);
					}
				} else {
					if (error) {
						zend_spprintf(error, 0, "non-static method %s::%s() %s be called statically", fcc->calling_scope->name, fcc->function_handler->common.function_name, verb);
						if (severity == E_ERROR) {
							retval = 0;
						}
					} else if (retval) {
						zend_error(severity, "Non-static method %s::%s() %s be called statically", fcc->calling_scope->name, fcc->function_handler->common.function_name, verb);
					}
				}
			}
			if (retval && (check_flags & IS_CALLABLE_CHECK_NO_ACCESS) == 0) {
				if (fcc->function_handler->op_array.fn_flags & ZEND_ACC_PRIVATE) {
					if (!zend_check_private(fcc->function_handler, fcc->object_ptr ? Z_OBJCE_P(fcc->object_ptr) : EG(scope), lmname, mlen TSRMLS_CC)) {
						if (error) {
							if (*error) {
								efree(*error);
							}
							zend_spprintf(error, 0, "cannot access private method %s::%s()", fcc->calling_scope->name, fcc->function_handler->common.function_name);
						}
						retval = 0;
					}
				} else if ((fcc->function_handler->common.fn_flags & ZEND_ACC_PROTECTED)) {
					if (!zend_check_protected(fcc->function_handler->common.scope, EG(scope))) {
						if (error) {
							if (*error) {
								efree(*error);
							}
							zend_spprintf(error, 0, "cannot access protected method %s::%s()", fcc->calling_scope->name, fcc->function_handler->common.function_name);
						}
						retval = 0;
					}
				}
			}
		}
	} else if (error && !(check_flags & IS_CALLABLE_CHECK_SILENT)) {
		if (fcc->calling_scope) {
			if (error) zend_spprintf(error, 0, "class '%s' does not have a method '%s'", fcc->calling_scope->name, mname);
		} else {
			if (error) zend_spprintf(error, 0, "function '%s' does not exist", mname);
		}
	}
	efree(lmname);

	if (fcc->object_ptr) {
		fcc->called_scope = Z_OBJCE_P(fcc->object_ptr);
	}
	if (retval) {
		fcc->initialized = 1;
	}
	return retval;
}