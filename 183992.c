header_address_rewrite_buffer(char *buffer, const char *address, size_t len)
{
	size_t	i;
	int	address_len;
	int	escape, quote, comment, bracket;
	int	has_bracket, has_group;
	int	pos_bracket_beg, pos_bracket_end, pos_component_beg, pos_component_end;
	int	insert_beg, insert_end;
	char	copy[APPEND_DOMAIN_BUFFER_SIZE];

	escape = quote = comment = bracket = 0;
	has_bracket = has_group = 0;
	pos_bracket_beg = pos_bracket_end = pos_component_beg = pos_component_end = 0;
	for (i = 0; buffer[i]; ++i) {
		if (buffer[i] == '(' && !escape && !quote)
			comment++;
		if (buffer[i] == '"' && !escape && !comment)
			quote = !quote;
		if (buffer[i] == ')' && !escape && !quote && comment)
			comment--;
		if (buffer[i] == '\\' && !escape && !comment && !quote)
			escape = 1;
		else
			escape = 0;
		if (buffer[i] == '<' && !escape && !comment && !quote && !bracket) {
			bracket++;
			has_bracket = 1;
			pos_bracket_beg = i+1;
		}
		if (buffer[i] == '>' && !escape && !comment && !quote && bracket) {
			bracket--;
			pos_bracket_end = i;
		}
		if (buffer[i] == ':' && !escape && !comment && !quote)
			has_group = 1;

		/* update insert point if not in comment and not on a whitespace */
		if (!comment && buffer[i] != ')' && !isspace((unsigned char)buffer[i]))
			pos_component_end = i;
	}

	/* parse error, do not attempt to modify */
	if (escape || quote || comment || bracket)
		return;

	/* address is group, skip */
	if (has_group)
		return;

	/* there's an address between brackets, just replace everything brackets */
	if (has_bracket) {
		insert_beg = pos_bracket_beg;
		insert_end = pos_bracket_end;
	}
	else {
		if (pos_component_end == 0)
			pos_component_beg = 0;
		else {
			for (pos_component_beg = pos_component_end; pos_component_beg >= 0; --pos_component_beg)
				if (buffer[pos_component_beg] == ')' || isspace(buffer[pos_component_beg]))
					break;
			pos_component_beg += 1;
			pos_component_end += 1;
		}
		insert_beg = pos_component_beg;
		insert_end = pos_component_end;
	}

	/* check that masquerade won' t overflow */
	address_len = strlen(address);
	if (strlen(buffer) - (insert_end - insert_beg) + address_len >= len)
		return;

	(void)strlcpy(copy, buffer, sizeof copy);
	(void)strlcpy(copy+insert_beg, address, sizeof (copy) - insert_beg);
	(void)strlcat(copy, buffer+insert_end, sizeof (copy));
	memcpy(buffer, copy, len);
}