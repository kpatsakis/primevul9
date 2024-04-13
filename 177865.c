 */
static void php_wddx_process_data(void *user_data, const XML_Char *s, int len)
{
	st_entry *ent;
	wddx_stack *stack = (wddx_stack *)user_data;

	if (!wddx_stack_is_empty(stack) && !stack->done) {
		wddx_stack_top(stack, (void**)&ent);
		switch (ent->type) {
			case ST_BINARY:
			case ST_STRING:
				if (Z_STRLEN(ent->data) == 0) {
					zval_ptr_dtor(&ent->data);
					ZVAL_STRINGL(&ent->data, (char *)s, len);
				} else {
					Z_STR(ent->data) = zend_string_extend(Z_STR(ent->data), Z_STRLEN(ent->data) + len, 0);
					memcpy(Z_STRVAL(ent->data) + Z_STRLEN(ent->data) - len, (char *)s, len);
					Z_STRVAL(ent->data)[Z_STRLEN(ent->data)] = '\0';
				}
				break;
			case ST_NUMBER:
				ZVAL_STRINGL(&ent->data, (char *)s, len);
				convert_scalar_to_number(&ent->data);
				break;

			case ST_BOOLEAN:
				if (!strcmp((char *)s, "true")) {
					Z_LVAL(ent->data) = 1;
				} else if (!strcmp((char *)s, "false")) {
					Z_LVAL(ent->data) = 0;
				} else {
					stack->top--;
					zval_ptr_dtor(&ent->data);
					if (ent->varname)
						efree(ent->varname);
					efree(ent);
				}
				break;

			case ST_DATETIME: {
				char *tmp;

				tmp = emalloc(len + 1);
				memcpy(tmp, (char *)s, len);
				tmp[len] = '\0';

				Z_LVAL(ent->data) = php_parse_date(tmp, NULL);
				/* date out of range < 1969 or > 2038 */
				if (Z_LVAL(ent->data) == -1) {
					ZVAL_STRINGL(&ent->data, (char *)s, len);
				}
				efree(tmp);
			}
				break;

			default:
				break;
		}
	}