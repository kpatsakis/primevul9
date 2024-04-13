SECU_FilePasswd(PK11SlotInfo *slot, PRBool retry, void *arg)
{
	cms_context *cms = (cms_context *)arg;
	int fd;
	char *file = NULL;
	char *token_name = slot ? PK11_GetTokenName(slot) : NULL;
	struct token_pass *phrases = NULL;
	size_t nphrases = 0;
	char *phrase = NULL;
	char *start;
	char *ret = NULL;
	char *path;

	ingress();
	dprintf("token_name: %s", token_name);
	path = cms->pwdata.data;

	if (!path || retry)
		goto err;

	phrases = calloc(1, sizeof(struct token_pass));
	if (!phrases)
		goto err;

	fd = open(path, O_RDONLY|O_CLOEXEC);
	if (fd < 0) {
		goto err_phrases;
	} else {
		size_t file_len = 0;
		int rc;
		rc = read_file(fd, &file, &file_len);
		set_errno_guard();
		close(fd);

		if (rc < 0 || file_len < 1)
			goto err_file;
		file[file_len-1] = '\0';
		dprintf("file_len:%zd", file_len);
		dprintf("file:\"%s\"", file);

		unbreak_line_continuations(file, file_len);
	}

	start = file;
	while (start && start[0]) {
		size_t span;
		struct token_pass *new_phrases;
		int rc;
		char c;

		new_phrases = reallocarray(phrases, nphrases + 1, sizeof(struct token_pass));
		if (!new_phrases)
			goto err_phrases;
		phrases = new_phrases;
		memset(&new_phrases[nphrases], 0, sizeof(struct token_pass));

		span = strspn(start, whitespace_and_eol_chars);
		dprintf("whitespace span is %zd", span);
		start += span;
		span = strcspn(start, eol_chars);
		dprintf("non-whitespace span is %zd", span);

		c = start[span];
		start[span] = '\0';
		dprintf("file:\"%s\"", file);
		rc = parse_pwfile_line(start, &phrases[nphrases++]);
		dprintf("parse_pwfile_line returned %d", rc);
		if (rc < 0)
			goto err_phrases;

		if (c != '\0')
			span++;
		start += span;
		dprintf("start is file[%ld] == '\\x%02hhx'", start - file, start[0]);
	}

	qsort(phrases, nphrases, sizeof(struct token_pass), token_pass_cmp);
	cms->pwdata.source = PW_DATABASE;
	xfree(cms->pwdata.data);
	cms->pwdata.pwdb.phrases = phrases;
	cms->pwdata.pwdb.nphrases = nphrases;

	for (size_t i = 0; i < nphrases; i++) {
		if (phrases[i].token == NULL || phrases[i].token[0] == '\0'
		    || (token_name && !strcmp(token_name, phrases[i].token))) {
			phrase = phrases[i].pass;
			break;
		}
	}

	if (phrase) {
		ret = PORT_Strdup(phrase);
		if (!ret)
			errno = ENOMEM;
	}

err_file:
	xfree(file);
err_phrases:
	xfree(phrases);
err:
	dprintf("ret:\"%s\"", ret ? ret : "(null)");
	egress();
	return ret;
}