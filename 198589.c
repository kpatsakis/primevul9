util_getpass (char **lineptr, size_t *len, FILE *stream)
{
#define MAX_PASS_SIZE	128
	char *buf;
	size_t i;
	int ch = 0;
#ifndef _WIN32
	struct termios old, new;

	fflush(stdout);
	if (tcgetattr (fileno (stdout), &old) != 0)
		return -1;
	new = old;
	new.c_lflag &= ~ECHO;
	if (tcsetattr (fileno (stdout), TCSAFLUSH, &new) != 0)
		return -1;
#endif

	buf = calloc(1, MAX_PASS_SIZE);
	if (!buf)
		return -1;

	for (i = 0; i < MAX_PASS_SIZE - 1; i++) {
#ifndef _WIN32
		ch = getchar();
#else
		ch = _getch();
#endif
		if (ch == 0 || ch == 3)
			break;
		if (ch == '\n' || ch == '\r')
			break;

		buf[i] = (char) ch;
	}
#ifndef _WIN32
	tcsetattr (fileno (stdout), TCSAFLUSH, &old);
	fputs("\n", stdout);
#endif
	if (ch == 0 || ch == 3) {
		free(buf);
		return -1;
	}

	if (*lineptr && (!len || *len < i+1)) {
		free(*lineptr);
		*lineptr = NULL;
	}

	if (*lineptr) {
		memcpy(*lineptr,buf,i+1);
		memset(buf, 0, MAX_PASS_SIZE);
		free(buf);
	} else {
		*lineptr = buf;
		if (len)
			*len = MAX_PASS_SIZE;
	}
	return i;
}