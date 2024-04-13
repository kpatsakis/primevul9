ZEND_API int zend_register_functions(zend_class_entry *scope, const zend_function_entry *functions, HashTable *function_table, int type TSRMLS_DC) /* {{{ */
{
	const zend_function_entry *ptr = functions;
	zend_function function, *reg_function;
	zend_internal_function *internal_function = (zend_internal_function *)&function;
	int count=0, unload=0;
	HashTable *target_function_table = function_table;
	int error_type;
	zend_function *ctor = NULL, *dtor = NULL, *clone = NULL, *__get = NULL, *__set = NULL, *__unset = NULL, *__isset = NULL, *__call = NULL, *__callstatic = NULL, *__tostring = NULL, *__debugInfo = NULL;
	const char *lowercase_name;
	int fname_len;
	const char *lc_class_name = NULL;
	int class_name_len = 0;
	zend_ulong hash;

	if (type==MODULE_PERSISTENT) {
		error_type = E_CORE_WARNING;
	} else {
		error_type = E_WARNING;
	}

	if (!target_function_table) {
		target_function_table = CG(function_table);
	}
	internal_function->type = ZEND_INTERNAL_FUNCTION;
	internal_function->module = EG(current_module);

	if (scope) {
		class_name_len = strlen(scope->name);
		if ((lc_class_name = zend_memrchr(scope->name, '\\', class_name_len))) {
			++lc_class_name;
			class_name_len -= (lc_class_name - scope->name);
			lc_class_name = zend_str_tolower_dup(lc_class_name, class_name_len);
		} else {
			lc_class_name = zend_str_tolower_dup(scope->name, class_name_len);
		}
	}

	while (ptr->fname) {
		internal_function->handler = ptr->handler;
		internal_function->function_name = (char*)ptr->fname;
		internal_function->scope = scope;
		internal_function->prototype = NULL;
		if (ptr->flags) {
			if (!(ptr->flags & ZEND_ACC_PPP_MASK)) {
				if (ptr->flags != ZEND_ACC_DEPRECATED || scope) {
					zend_error(error_type, "Invalid access level for %s%s%s() - access must be exactly one of public, protected or private", scope ? scope->name : "", scope ? "::" : "", ptr->fname);
				}
				internal_function->fn_flags = ZEND_ACC_PUBLIC | ptr->flags;
			} else {
				internal_function->fn_flags = ptr->flags;
			}
		} else {
			internal_function->fn_flags = ZEND_ACC_PUBLIC;
		}
		if (ptr->arg_info) {
			zend_internal_function_info *info = (zend_internal_function_info*)ptr->arg_info;

			internal_function->arg_info = (zend_arg_info*)ptr->arg_info+1;
			internal_function->num_args = ptr->num_args;
			/* Currently you cannot denote that the function can accept less arguments than num_args */
			if (info->required_num_args == -1) {
				internal_function->required_num_args = ptr->num_args;
			} else {
				internal_function->required_num_args = info->required_num_args;
			}
			if (info->return_reference) {
				internal_function->fn_flags |= ZEND_ACC_RETURN_REFERENCE;
			}
			if (ptr->arg_info[ptr->num_args].is_variadic) {
				internal_function->fn_flags |= ZEND_ACC_VARIADIC;
			}
		} else {
			internal_function->arg_info = NULL;
			internal_function->num_args = 0;
			internal_function->required_num_args = 0;
		}
		if (ptr->flags & ZEND_ACC_ABSTRACT) {
			if (scope) {
				/* This is a class that must be abstract itself. Here we set the check info. */
				scope->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
				if (!(scope->ce_flags & ZEND_ACC_INTERFACE)) {
					/* Since the class is not an interface it needs to be declared as a abstract class. */
					/* Since here we are handling internal functions only we can add the keyword flag. */
					/* This time we set the flag for the keyword 'abstract'. */
					scope->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
				}
			}
			if (ptr->flags & ZEND_ACC_STATIC && (!scope || !(scope->ce_flags & ZEND_ACC_INTERFACE))) {
				zend_error(error_type, "Static function %s%s%s() cannot be abstract", scope ? scope->name : "", scope ? "::" : "", ptr->fname);
			}
		} else {
			if (scope && (scope->ce_flags & ZEND_ACC_INTERFACE)) {
				efree((char*)lc_class_name);
				zend_error(error_type, "Interface %s cannot contain non abstract method %s()", scope->name, ptr->fname);
				return FAILURE;
			}
			if (!internal_function->handler) {
				if (scope) {
					efree((char*)lc_class_name);
				}
				zend_error(error_type, "Method %s%s%s() cannot be a NULL function", scope ? scope->name : "", scope ? "::" : "", ptr->fname);
				zend_unregister_functions(functions, count, target_function_table TSRMLS_CC);
				return FAILURE;
			}
		}
		fname_len = strlen(ptr->fname);
		lowercase_name = zend_new_interned_string(zend_str_tolower_dup(ptr->fname, fname_len), fname_len + 1, 1 TSRMLS_CC);
		hash = str_hash(lowercase_name, fname_len);
		if (zend_hash_quick_add(target_function_table, lowercase_name, fname_len+1, hash, &function, sizeof(zend_function), (void**)&reg_function) == FAILURE) {
			unload=1;
			str_efree(lowercase_name);
			break;
		}

		/* If types of arguments have to be checked */
		if (reg_function->common.arg_info && reg_function->common.num_args) {
			int i;
			for (i = 0; i < reg_function->common.num_args; i++) {
				if (reg_function->common.arg_info[i].class_name ||
				    reg_function->common.arg_info[i].type_hint) {
				    reg_function->common.fn_flags |= ZEND_ACC_HAS_TYPE_HINTS;
					break;
				}
			}
		}

		if (scope) {
			/* Look for ctor, dtor, clone
			 * If it's an old-style constructor, store it only if we don't have
			 * a constructor already.
			 */
			if ((fname_len == class_name_len) && !ctor && !memcmp(lowercase_name, lc_class_name, class_name_len+1)) {
				ctor = reg_function;
			} else if ((fname_len == sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME) - 1)) {
				ctor = reg_function;
			} else if ((fname_len == sizeof(ZEND_DESTRUCTOR_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_DESTRUCTOR_FUNC_NAME, sizeof(ZEND_DESTRUCTOR_FUNC_NAME) - 1)) {
				dtor = reg_function;
				if (internal_function->num_args) {
					zend_error(error_type, "Destructor %s::%s() cannot take arguments", scope->name, ptr->fname);
				}
			} else if ((fname_len == sizeof(ZEND_CLONE_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_CLONE_FUNC_NAME, sizeof(ZEND_CLONE_FUNC_NAME) - 1)) {
				clone = reg_function;
			} else if ((fname_len == sizeof(ZEND_CALL_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME) - 1)) {
				__call = reg_function;
			} else if ((fname_len == sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_CALLSTATIC_FUNC_NAME, sizeof(ZEND_CALLSTATIC_FUNC_NAME) - 1)) {
				__callstatic = reg_function;
			} else if ((fname_len == sizeof(ZEND_TOSTRING_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_TOSTRING_FUNC_NAME, sizeof(ZEND_TOSTRING_FUNC_NAME) - 1)) {
				__tostring = reg_function;
			} else if ((fname_len == sizeof(ZEND_GET_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_GET_FUNC_NAME, sizeof(ZEND_GET_FUNC_NAME) - 1)) {
				__get = reg_function;
			} else if ((fname_len == sizeof(ZEND_SET_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_SET_FUNC_NAME, sizeof(ZEND_SET_FUNC_NAME) - 1)) {
				__set = reg_function;
			} else if ((fname_len == sizeof(ZEND_UNSET_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_UNSET_FUNC_NAME, sizeof(ZEND_UNSET_FUNC_NAME) - 1)) {
				__unset = reg_function;
			} else if ((fname_len == sizeof(ZEND_ISSET_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_ISSET_FUNC_NAME, sizeof(ZEND_ISSET_FUNC_NAME) - 1)) {
				__isset = reg_function;
			} else if ((fname_len == sizeof(ZEND_DEBUGINFO_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_DEBUGINFO_FUNC_NAME, sizeof(ZEND_DEBUGINFO_FUNC_NAME) - 1)) {
				__debugInfo = reg_function;
			} else {
				reg_function = NULL;
			}
			if (reg_function) {
				zend_check_magic_method_implementation(scope, reg_function, error_type TSRMLS_CC);
			}
		}
		ptr++;
		count++;
		str_efree(lowercase_name);
	}
	if (unload) { /* before unloading, display all remaining bad function in the module */
		if (scope) {
			efree((char*)lc_class_name);
		}
		while (ptr->fname) {
			fname_len = strlen(ptr->fname);
			lowercase_name = zend_str_tolower_dup(ptr->fname, fname_len);
			if (zend_hash_exists(target_function_table, lowercase_name, fname_len+1)) {
				zend_error(error_type, "Function registration failed - duplicate name - %s%s%s", scope ? scope->name : "", scope ? "::" : "", ptr->fname);
			}
			efree((char*)lowercase_name);
			ptr++;
		}
		zend_unregister_functions(functions, count, target_function_table TSRMLS_CC);
		return FAILURE;
	}
	if (scope) {
		scope->constructor = ctor;
		scope->destructor = dtor;
		scope->clone = clone;
		scope->__call = __call;
		scope->__callstatic = __callstatic;
		scope->__tostring = __tostring;
		scope->__get = __get;
		scope->__set = __set;
		scope->__unset = __unset;
		scope->__isset = __isset;
		scope->__debugInfo = __debugInfo;
		if (ctor) {
			ctor->common.fn_flags |= ZEND_ACC_CTOR;
			if (ctor->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Constructor %s::%s() cannot be static", scope->name, ctor->common.function_name);
			}
			ctor->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (dtor) {
			dtor->common.fn_flags |= ZEND_ACC_DTOR;
			if (dtor->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Destructor %s::%s() cannot be static", scope->name, dtor->common.function_name);
			}
			dtor->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (clone) {
			clone->common.fn_flags |= ZEND_ACC_CLONE;
			if (clone->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Constructor %s::%s() cannot be static", scope->name, clone->common.function_name);
			}
			clone->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__call) {
			if (__call->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __call->common.function_name);
			}
			__call->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__callstatic) {
			if (!(__callstatic->common.fn_flags & ZEND_ACC_STATIC)) {
				zend_error(error_type, "Method %s::%s() must be static", scope->name, __callstatic->common.function_name);
			}
			__callstatic->common.fn_flags |= ZEND_ACC_STATIC;
		}
		if (__tostring) {
			if (__tostring->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __tostring->common.function_name);
			}
			__tostring->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__get) {
			if (__get->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __get->common.function_name);
			}
			__get->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__set) {
			if (__set->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __set->common.function_name);
			}
			__set->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__unset) {
			if (__unset->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __unset->common.function_name);
			}
			__unset->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__isset) {
			if (__isset->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __isset->common.function_name);
			}
			__isset->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__debugInfo) {
			if (__debugInfo->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __debugInfo->common.function_name);
			}
		}
		efree((char*)lc_class_name);
	}
	return SUCCESS;
}