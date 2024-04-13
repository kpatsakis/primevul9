parse_pwfile_line(char *start, struct token_pass *tp)
{
	size_t span, escspan;
	char *line = start;
	size_t offset = 0;

	span = strspn(line, whitespace_and_eol_chars);
	dprintf("whitespace span is %zd", span);
	if (span == 0 && line[span] == '\0')
		return -1;
	line += span;

	tp->token = NULL;
	tp->pass = line;

	offset = 0;
	do {
		span = strcspn(line + offset, whitespace_and_eol_chars);
		escspan = strescspn(line + offset);
		if (escspan < span)
			offset += escspan + 2;
	} while(escspan < span);
	span += offset;
	dprintf("non-whitespace span is %zd", span);

	if (line[span] == '\0') {
		dprintf("returning %ld", (line + span) - start);
		return (line + span) - start;
	}
	line[span] = '\0';

	line += span + 1;
	span = strspn(line, whitespace_and_eol_chars);
	dprintf("whitespace span is %zd", span);
	line += span;
	tp->token = tp->pass;
	tp->pass = line;

	offset = 0;
	do {
		span = strcspn(line + offset, whitespace_and_eol_chars);
		escspan = strescspn(line + offset);
		if (escspan < span)
			offset += escspan + 2;
	} while(escspan < span);
	span += offset;
	dprintf("non-whitespace span is %zd", span);
	if (line[span] != '\0')
		line[span++] = '\0';

	resolve_escapes(tp->token);
	dprintf("Setting token pass %p to { %p, %p }", tp, tp->token, tp->pass);
	dprintf("token:\"%s\"", tp->token);
	dprintf("pass:\"%s\"", tp->pass);
	dprintf("returning %ld", (line + span) - start);
	return (line + span) - start;
}