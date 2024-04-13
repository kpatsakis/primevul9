PS_OPEN_FUNC(files)
{
	ps_files *data;
	const char *p, *last;
	const char *argv[3];
	int argc = 0;
	size_t dirdepth = 0;
	int filemode = 0600;

	if (*save_path == '\0') {
		/* if save path is an empty string, determine the temporary dir */
		save_path = php_get_temporary_directory(TSRMLS_C);

		if (php_check_open_basedir(save_path TSRMLS_CC)) {
			return FAILURE;
		}
	}

	/* split up input parameter */
	last = save_path;
	p = strchr(save_path, ';');
	while (p) {
		argv[argc++] = last;
		last = ++p;
		p = strchr(p, ';');
		if (argc > 1) break;
	}
	argv[argc++] = last;

	if (argc > 1) {
		errno = 0;
		dirdepth = (size_t) strtol(argv[0], NULL, 10);
		if (errno == ERANGE) {
			php_error(E_WARNING, "The first parameter in session.save_path is invalid");
			return FAILURE;
		}
	}

	if (argc > 2) {
		errno = 0;
		filemode = strtol(argv[1], NULL, 8);
		if (errno == ERANGE || filemode < 0 || filemode > 07777) {
			php_error(E_WARNING, "The second parameter in session.save_path is invalid");
			return FAILURE;
		}
	}
	save_path = argv[argc - 1];

	data = ecalloc(1, sizeof(*data));

	data->fd = -1;
	data->dirdepth = dirdepth;
	data->filemode = filemode;
	data->basedir_len = strlen(save_path);
	data->basedir = estrndup(save_path, data->basedir_len);

	if (PS_GET_MOD_DATA()) {
		ps_close_files(mod_data TSRMLS_CC);
	}
	PS_SET_MOD_DATA(data);

	return SUCCESS;
}