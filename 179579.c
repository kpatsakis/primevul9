input_split(struct input_ctx *ictx)

{
	const char	*errstr;
	char		*ptr, *out;
	int		 n;

	ictx->param_list_len = 0;
	if (ictx->param_len == 0)
		return (0);

	ptr = ictx->param_buf;
	while ((out = strsep(&ptr, ";")) != NULL) {
		if (*out == '\0')
			n = -1;
		else {
			n = strtonum(out, 0, INT_MAX, &errstr);
			if (errstr != NULL)
				return (-1);
		}

		ictx->param_list[ictx->param_list_len++] = n;
		if (ictx->param_list_len == nitems(ictx->param_list))
			return (-1);
	}

	return (0);
}