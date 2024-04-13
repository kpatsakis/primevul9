static int parse_pred(const char *str, void *data,
		      int pos, struct filter_parse_error *pe,
		      struct filter_pred **pred_ptr)
{
	struct trace_event_call *call = data;
	struct ftrace_event_field *field;
	struct filter_pred *pred = NULL;
	char num_buf[24];	/* Big enough to hold an address */
	char *field_name;
	char q;
	u64 val;
	int len;
	int ret;
	int op;
	int s;
	int i = 0;

	/* First find the field to associate to */
	while (isspace(str[i]))
		i++;
	s = i;

	while (isalnum(str[i]) || str[i] == '_')
		i++;

	len = i - s;

	if (!len)
		return -1;

	field_name = kmemdup_nul(str + s, len, GFP_KERNEL);
	if (!field_name)
		return -ENOMEM;

	/* Make sure that the field exists */

	field = trace_find_event_field(call, field_name);
	kfree(field_name);
	if (!field) {
		parse_error(pe, FILT_ERR_FIELD_NOT_FOUND, pos + i);
		return -EINVAL;
	}

	while (isspace(str[i]))
		i++;

	/* Make sure this op is supported */
	for (op = 0; ops[op]; op++) {
		/* This is why '<=' must come before '<' in ops[] */
		if (strncmp(str + i, ops[op], strlen(ops[op])) == 0)
			break;
	}

	if (!ops[op]) {
		parse_error(pe, FILT_ERR_INVALID_OP, pos + i);
		goto err_free;
	}

	i += strlen(ops[op]);

	while (isspace(str[i]))
		i++;

	s = i;

	pred = kzalloc(sizeof(*pred), GFP_KERNEL);
	if (!pred)
		return -ENOMEM;

	pred->field = field;
	pred->offset = field->offset;
	pred->op = op;

	if (ftrace_event_is_function(call)) {
		/*
		 * Perf does things different with function events.
		 * It only allows an "ip" field, and expects a string.
		 * But the string does not need to be surrounded by quotes.
		 * If it is a string, the assigned function as a nop,
		 * (perf doesn't use it) and grab everything.
		 */
		if (strcmp(field->name, "ip") != 0) {
			parse_error(pe, FILT_ERR_IP_FIELD_ONLY, pos + i);
			goto err_free;
		}
		pred->fn = filter_pred_none;

		/*
		 * Quotes are not required, but if they exist then we need
		 * to read them till we hit a matching one.
		 */
		if (str[i] == '\'' || str[i] == '"')
			q = str[i];
		else
			q = 0;

		for (i++; str[i]; i++) {
			if (q && str[i] == q)
				break;
			if (!q && (str[i] == ')' || str[i] == '&' ||
				   str[i] == '|'))
				break;
		}
		/* Skip quotes */
		if (q)
			s++;
		len = i - s;
		if (len >= MAX_FILTER_STR_VAL) {
			parse_error(pe, FILT_ERR_OPERAND_TOO_LONG, pos + i);
			goto err_free;
		}

		pred->regex.len = len;
		strncpy(pred->regex.pattern, str + s, len);
		pred->regex.pattern[len] = 0;

	/* This is either a string, or an integer */
	} else if (str[i] == '\'' || str[i] == '"') {
		char q = str[i];

		/* Make sure the op is OK for strings */
		switch (op) {
		case OP_NE:
			pred->not = 1;
			/* Fall through */
		case OP_GLOB:
		case OP_EQ:
			break;
		default:
			parse_error(pe, FILT_ERR_ILLEGAL_FIELD_OP, pos + i);
			goto err_free;
		}

		/* Make sure the field is OK for strings */
		if (!is_string_field(field)) {
			parse_error(pe, FILT_ERR_EXPECT_DIGIT, pos + i);
			goto err_free;
		}

		for (i++; str[i]; i++) {
			if (str[i] == q)
				break;
		}
		if (!str[i]) {
			parse_error(pe, FILT_ERR_MISSING_QUOTE, pos + i);
			goto err_free;
		}

		/* Skip quotes */
		s++;
		len = i - s;
		if (len >= MAX_FILTER_STR_VAL) {
			parse_error(pe, FILT_ERR_OPERAND_TOO_LONG, pos + i);
			goto err_free;
		}

		pred->regex.len = len;
		strncpy(pred->regex.pattern, str + s, len);
		pred->regex.pattern[len] = 0;

		filter_build_regex(pred);

		if (field->filter_type == FILTER_COMM) {
			pred->fn = filter_pred_comm;

		} else if (field->filter_type == FILTER_STATIC_STRING) {
			pred->fn = filter_pred_string;
			pred->regex.field_len = field->size;

		} else if (field->filter_type == FILTER_DYN_STRING)
			pred->fn = filter_pred_strloc;
		else
			pred->fn = filter_pred_pchar;
		/* go past the last quote */
		i++;

	} else if (isdigit(str[i]) || str[i] == '-') {

		/* Make sure the field is not a string */
		if (is_string_field(field)) {
			parse_error(pe, FILT_ERR_EXPECT_STRING, pos + i);
			goto err_free;
		}

		if (op == OP_GLOB) {
			parse_error(pe, FILT_ERR_ILLEGAL_FIELD_OP, pos + i);
			goto err_free;
		}

		if (str[i] == '-')
			i++;

		/* We allow 0xDEADBEEF */
		while (isalnum(str[i]))
			i++;

		len = i - s;
		/* 0xfeedfacedeadbeef is 18 chars max */
		if (len >= sizeof(num_buf)) {
			parse_error(pe, FILT_ERR_OPERAND_TOO_LONG, pos + i);
			goto err_free;
		}

		strncpy(num_buf, str + s, len);
		num_buf[len] = 0;

		/* Make sure it is a value */
		if (field->is_signed)
			ret = kstrtoll(num_buf, 0, &val);
		else
			ret = kstrtoull(num_buf, 0, &val);
		if (ret) {
			parse_error(pe, FILT_ERR_ILLEGAL_INTVAL, pos + s);
			goto err_free;
		}

		pred->val = val;

		if (field->filter_type == FILTER_CPU)
			pred->fn = filter_pred_cpu;
		else {
			pred->fn = select_comparison_fn(pred->op, field->size,
							field->is_signed);
			if (pred->op == OP_NE)
				pred->not = 1;
		}

	} else {
		parse_error(pe, FILT_ERR_INVALID_VALUE, pos + i);
		goto err_free;
	}

	*pred_ptr = pred;
	return i;

err_free:
	kfree(pred);
	return -EINVAL;
}