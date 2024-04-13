static int zend_parse_va_args(int num_args, const char *type_spec, va_list *va, int flags TSRMLS_DC) /* {{{ */
{
	const  char *spec_walk;
	int c, i;
	int min_num_args = -1;
	int max_num_args = 0;
	int post_varargs = 0;
	zval **arg;
	int arg_count;
	int quiet = flags & ZEND_PARSE_PARAMS_QUIET;
	zend_bool have_varargs = 0;
	zval ****varargs = NULL;
	int *n_varargs = NULL;

	for (spec_walk = type_spec; *spec_walk; spec_walk++) {
		c = *spec_walk;
		switch (c) {
			case 'l': case 'd':
			case 's': case 'b':
			case 'r': case 'a':
			case 'o': case 'O':
			case 'z': case 'Z':
			case 'C': case 'h':
			case 'f': case 'A':
			case 'H': case 'p':
				max_num_args++;
				break;

			case '|':
				min_num_args = max_num_args;
				break;

			case '/':
			case '!':
				/* Pass */
				break;

			case '*':
			case '+':
				if (have_varargs) {
					if (!quiet) {
						zend_function *active_function = EG(current_execute_data)->function_state.function;
						const char *class_name = active_function->common.scope ? active_function->common.scope->name : "";
						zend_error(E_WARNING, "%s%s%s(): only one varargs specifier (* or +) is permitted",
								class_name,
								class_name[0] ? "::" : "",
								active_function->common.function_name);
					}
					return FAILURE;
				}
				have_varargs = 1;
				/* we expect at least one parameter in varargs */
				if (c == '+') {
					max_num_args++;
				}
				/* mark the beginning of varargs */
				post_varargs = max_num_args;
				break;

			default:
				if (!quiet) {
					zend_function *active_function = EG(current_execute_data)->function_state.function;
					const char *class_name = active_function->common.scope ? active_function->common.scope->name : "";
					zend_error(E_WARNING, "%s%s%s(): bad type specifier while parsing parameters",
							class_name,
							class_name[0] ? "::" : "",
							active_function->common.function_name);
				}
				return FAILURE;
		}
	}

	if (min_num_args < 0) {
		min_num_args = max_num_args;
	}

	if (have_varargs) {
		/* calculate how many required args are at the end of the specifier list */
		post_varargs = max_num_args - post_varargs;
		max_num_args = -1;
	}

	if (num_args < min_num_args || (num_args > max_num_args && max_num_args > 0)) {
		if (!quiet) {
			zend_function *active_function = EG(current_execute_data)->function_state.function;
			const char *class_name = active_function->common.scope ? active_function->common.scope->name : "";
			zend_error(E_WARNING, "%s%s%s() expects %s %d parameter%s, %d given",
					class_name,
					class_name[0] ? "::" : "",
					active_function->common.function_name,
					min_num_args == max_num_args ? "exactly" : num_args < min_num_args ? "at least" : "at most",
					num_args < min_num_args ? min_num_args : max_num_args,
					(num_args < min_num_args ? min_num_args : max_num_args) == 1 ? "" : "s",
					num_args);
		}
		return FAILURE;
	}

	arg_count = (int)(zend_uintptr_t) *(zend_vm_stack_top(TSRMLS_C) - 1);

	if (num_args > arg_count) {
		zend_error(E_WARNING, "%s(): could not obtain parameters for parsing",
			get_active_function_name(TSRMLS_C));
		return FAILURE;
	}

	i = 0;
	while (num_args-- > 0) {
		if (*type_spec == '|') {
			type_spec++;
		}

		if (*type_spec == '*' || *type_spec == '+') {
			int num_varargs = num_args + 1 - post_varargs;

			/* eat up the passed in storage even if it won't be filled in with varargs */
			varargs = va_arg(*va, zval ****);
			n_varargs = va_arg(*va, int *);
			type_spec++;

			if (num_varargs > 0) {
				int iv = 0;
				zval **p = (zval **) (zend_vm_stack_top(TSRMLS_C) - 1 - (arg_count - i));

				*n_varargs = num_varargs;

				/* allocate space for array and store args */
				*varargs = safe_emalloc(num_varargs, sizeof(zval **), 0);
				while (num_varargs-- > 0) {
					(*varargs)[iv++] = p++;
				}

				/* adjust how many args we have left and restart loop */
				num_args = num_args + 1 - iv;
				i += iv;
				continue;
			} else {
				*varargs = NULL;
				*n_varargs = 0;
			}
		}

		arg = (zval **) (zend_vm_stack_top(TSRMLS_C) - 1 - (arg_count-i));

		if (zend_parse_arg(i+1, arg, va, &type_spec, quiet TSRMLS_CC) == FAILURE) {
			/* clean up varargs array if it was used */
			if (varargs && *varargs) {
				efree(*varargs);
				*varargs = NULL;
			}
			return FAILURE;
		}
		i++;
	}

	return SUCCESS;
}