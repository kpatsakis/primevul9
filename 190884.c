
void mail_getquota(MAILSTREAM *stream, char *qroot, QUOTALIST *qlist)
{
	zval t_map, *return_value;

	return_value = *IMAPG(quota_return);

/* put parsing code here */
	for(; qlist; qlist = qlist->next) {
		array_init(&t_map);
		if (strncmp(qlist->name, "STORAGE", 7) == 0)
		{
			/* this is to add backwards compatibility */
			add_assoc_long_ex(return_value, "usage", sizeof("usage") - 1, qlist->usage);
			add_assoc_long_ex(return_value, "limit", sizeof("limit") - 1, qlist->limit);
		}

		add_assoc_long_ex(&t_map, "usage", sizeof("usage") - 1, qlist->usage);
		add_assoc_long_ex(&t_map, "limit", sizeof("limit") - 1, qlist->limit);
		add_assoc_zval_ex(return_value, qlist->name, strlen(qlist->name), &t_map);
	}