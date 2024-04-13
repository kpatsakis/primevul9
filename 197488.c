static const char *zend_parse_arg_impl(int arg_num, zval **arg, va_list *va, const char **spec, char **error, int *severity TSRMLS_DC) /* {{{ */
{
	const char *spec_walk = *spec;
	char c = *spec_walk++;
	int check_null = 0;

	/* scan through modifiers */
	while (1) {
		if (*spec_walk == '/') {
			SEPARATE_ZVAL_IF_NOT_REF(arg);
		} else if (*spec_walk == '!') {
			check_null = 1;
		} else {
			break;
		}
		spec_walk++;
	}

	switch (c) {
		case 'l':
		case 'L':
			{
				long *p = va_arg(*va, long *);

				if (check_null) {
					zend_bool *p = va_arg(*va, zend_bool *);
					*p = (Z_TYPE_PP(arg) == IS_NULL);
				}

				switch (Z_TYPE_PP(arg)) {
					case IS_STRING:
						{
							double d;
							int type;

							if ((type = is_numeric_string(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), p, &d, -1)) == 0) {
								return "long";
							} else if (type == IS_DOUBLE) {
								if (c == 'L') {
									if (d > LONG_MAX) {
										*p = LONG_MAX;
										break;
									} else if (d < LONG_MIN) {
										*p = LONG_MIN;
										break;
									}
								}

								*p = zend_dval_to_lval(d);
							}
						}
						break;

					case IS_DOUBLE:
						if (c == 'L') {
							if (Z_DVAL_PP(arg) > LONG_MAX) {
								*p = LONG_MAX;
								break;
							} else if (Z_DVAL_PP(arg) < LONG_MIN) {
								*p = LONG_MIN;
								break;
							}
						}
					case IS_NULL:
					case IS_LONG:
					case IS_BOOL:
						convert_to_long_ex(arg);
						*p = Z_LVAL_PP(arg);
						break;

					case IS_ARRAY:
					case IS_OBJECT:
					case IS_RESOURCE:
					default:
						return "long";
				}
			}
			break;

		case 'd':
			{
				double *p = va_arg(*va, double *);

				if (check_null) {
					zend_bool *p = va_arg(*va, zend_bool *);
					*p = (Z_TYPE_PP(arg) == IS_NULL);
				}

				switch (Z_TYPE_PP(arg)) {
					case IS_STRING:
						{
							long l;
							int type;

							if ((type = is_numeric_string(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), &l, p, -1)) == 0) {
								return "double";
							} else if (type == IS_LONG) {
								*p = (double) l;
							}
						}
						break;

					case IS_NULL:
					case IS_LONG:
					case IS_DOUBLE:
					case IS_BOOL:
						convert_to_double_ex(arg);
						*p = Z_DVAL_PP(arg);
						break;

					case IS_ARRAY:
					case IS_OBJECT:
					case IS_RESOURCE:
					default:
						return "double";
				}
			}
			break;

		case 'p':
		case 's':
			{
				char **p = va_arg(*va, char **);
				int *pl = va_arg(*va, int *);
				switch (Z_TYPE_PP(arg)) {
					case IS_NULL:
						if (check_null) {
							*p = NULL;
							*pl = 0;
							break;
						}
						/* break omitted intentionally */

					case IS_STRING:
					case IS_LONG:
					case IS_DOUBLE:
					case IS_BOOL:
						convert_to_string_ex(arg);
						if (UNEXPECTED(Z_ISREF_PP(arg) != 0)) {
							/* it's dangerous to return pointers to string
							   buffer of referenced variable, because it can
							   be clobbered throug magic callbacks */
							SEPARATE_ZVAL(arg);
						}
						*p = Z_STRVAL_PP(arg);
						*pl = Z_STRLEN_PP(arg);
						if (c == 'p' && CHECK_ZVAL_NULL_PATH(*arg)) {
							return "a valid path";
						}
						break;

					case IS_OBJECT:
						if (parse_arg_object_to_string(arg, p, pl, IS_STRING TSRMLS_CC) == SUCCESS) {
							if (c == 'p' && CHECK_ZVAL_NULL_PATH(*arg)) {
								return "a valid path";
							}
							break;
						}

					case IS_ARRAY:
					case IS_RESOURCE:
					default:
						return c == 's' ? "string" : "a valid path";
				}
			}
			break;

		case 'b':
			{
				zend_bool *p = va_arg(*va, zend_bool *);

				if (check_null) {
					zend_bool *p = va_arg(*va, zend_bool *);
					*p = (Z_TYPE_PP(arg) == IS_NULL);
				}

				switch (Z_TYPE_PP(arg)) {
					case IS_NULL:
					case IS_STRING:
					case IS_LONG:
					case IS_DOUBLE:
					case IS_BOOL:
						convert_to_boolean_ex(arg);
						*p = Z_BVAL_PP(arg);
						break;

					case IS_ARRAY:
					case IS_OBJECT:
					case IS_RESOURCE:
					default:
						return "boolean";
				}
			}
			break;

		case 'r':
			{
				zval **p = va_arg(*va, zval **);
				if (check_null && Z_TYPE_PP(arg) == IS_NULL) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_RESOURCE) {
					*p = *arg;
				} else {
					return "resource";
				}
			}
			break;
		case 'A':
		case 'a':
			{
				zval **p = va_arg(*va, zval **);
				if (check_null && Z_TYPE_PP(arg) == IS_NULL) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_ARRAY || (c == 'A' && Z_TYPE_PP(arg) == IS_OBJECT)) {
					*p = *arg;
				} else {
					return "array";
				}
			}
			break;
		case 'H':
		case 'h':
			{
				HashTable **p = va_arg(*va, HashTable **);
				if (check_null && Z_TYPE_PP(arg) == IS_NULL) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_ARRAY) {
					*p = Z_ARRVAL_PP(arg);
				} else if(c == 'H' && Z_TYPE_PP(arg) == IS_OBJECT) {
					*p = HASH_OF(*arg);
					if(*p == NULL) {
						return "array";
					}
				} else {
					return "array";
				}
			}
			break;

		case 'o':
			{
				zval **p = va_arg(*va, zval **);
				if (check_null && Z_TYPE_PP(arg) == IS_NULL) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_OBJECT) {
					*p = *arg;
				} else {
					return "object";
				}
			}
			break;

		case 'O':
			{
				zval **p = va_arg(*va, zval **);
				zend_class_entry *ce = va_arg(*va, zend_class_entry *);

				if (check_null && Z_TYPE_PP(arg) == IS_NULL) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_OBJECT &&
						(!ce || instanceof_function(Z_OBJCE_PP(arg), ce TSRMLS_CC))) {
					*p = *arg;
				} else {
					if (ce) {
						return ce->name;
					} else {
						return "object";
					}
				}
			}
			break;

		case 'C':
			{
				zend_class_entry **lookup, **pce = va_arg(*va, zend_class_entry **);
				zend_class_entry *ce_base = *pce;

				if (check_null && Z_TYPE_PP(arg) == IS_NULL) {
					*pce = NULL;
					break;
				}
				convert_to_string_ex(arg);
				if (zend_lookup_class(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), &lookup TSRMLS_CC) == FAILURE) {
					*pce = NULL;
				} else {
					*pce = *lookup;
				}
				if (ce_base) {
					if ((!*pce || !instanceof_function(*pce, ce_base TSRMLS_CC))) {
						zend_spprintf(error, 0, "to be a class name derived from %s, '%s' given",
							ce_base->name, Z_STRVAL_PP(arg));
						*pce = NULL;
						return "";
					}
				}
				if (!*pce) {
					zend_spprintf(error, 0, "to be a valid class name, '%s' given",
						Z_STRVAL_PP(arg));
					return "";
				}
				break;

			}
			break;

		case 'f':
			{
				zend_fcall_info *fci = va_arg(*va, zend_fcall_info *);
				zend_fcall_info_cache *fcc = va_arg(*va, zend_fcall_info_cache *);
				char *is_callable_error = NULL;

				if (check_null && Z_TYPE_PP(arg) == IS_NULL) {
					fci->size = 0;
					fcc->initialized = 0;
					break;
				}

				if (zend_fcall_info_init(*arg, 0, fci, fcc, NULL, &is_callable_error TSRMLS_CC) == SUCCESS) {
					if (is_callable_error) {
						*severity = E_STRICT;
						zend_spprintf(error, 0, "to be a valid callback, %s", is_callable_error);
						efree(is_callable_error);
						*spec = spec_walk;
						return "";
					}
					break;
				} else {
					if (is_callable_error) {
						*severity = E_WARNING;
						zend_spprintf(error, 0, "to be a valid callback, %s", is_callable_error);
						efree(is_callable_error);
						return "";
					} else {
						return "valid callback";
					}
				}
			}

		case 'z':
			{
				zval **p = va_arg(*va, zval **);
				if (check_null && Z_TYPE_PP(arg) == IS_NULL) {
					*p = NULL;
				} else {
					*p = *arg;
				}
			}
			break;

		case 'Z':
			{
				zval ***p = va_arg(*va, zval ***);
				if (check_null && Z_TYPE_PP(arg) == IS_NULL) {
					*p = NULL;
				} else {
					*p = arg;
				}
			}
			break;

		default:
			return "unknown";
	}

	*spec = spec_walk;

	return NULL;
}