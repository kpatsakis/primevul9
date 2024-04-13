CWD_API FILE *virtual_popen(const char *command, const char *type TSRMLS_DC) /* {{{ */
{
	int command_length;
	int dir_length, extra = 0;
	char *command_line;
	char *ptr, *dir;
	FILE *retval;

	command_length = strlen(command);

	dir_length = CWDG(cwd).cwd_length;
	dir = CWDG(cwd).cwd;
	while (dir_length > 0) {
		if (*dir == '\'') extra+=3;
		dir++;
		dir_length--;
	}
	dir_length = CWDG(cwd).cwd_length;
	dir = CWDG(cwd).cwd;

	ptr = command_line = (char *) malloc(command_length + sizeof("cd '' ; ") + dir_length + extra+1+1);
	if (!command_line) {
		return NULL;
	}
	memcpy(ptr, "cd ", sizeof("cd ")-1);
	ptr += sizeof("cd ")-1;

	if (CWDG(cwd).cwd_length == 0) {
		*ptr++ = DEFAULT_SLASH;
	} else {
		*ptr++ = '\'';
		while (dir_length > 0) {
			switch (*dir) {
			case '\'':
				*ptr++ = '\'';
				*ptr++ = '\\';
				*ptr++ = '\'';
				/* fall-through */
			default:
				*ptr++ = *dir;
			}
			dir++;
			dir_length--;
		}
		*ptr++ = '\'';
	}

	*ptr++ = ' ';
	*ptr++ = ';';
	*ptr++ = ' ';

	memcpy(ptr, command, command_length+1);
	retval = popen(command_line, type);

	free(command_line);
	return retval;
}