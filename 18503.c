read_password(FILE *in, FILE *out, char *buf, size_t bufsz)
{
	int infd = fileno(in);
	struct termios tio;
	char *ret;

	ret = fgets(buf, bufsz, in);

	if (isatty(infd)) {
		fprintf(out, "\n");
		fflush(out);

		tcgetattr(infd, &tio);
		tio.c_lflag |= ECHO;
		tcsetattr(infd, TCSAFLUSH, &tio);
	}
	if (ret == NULL)
		return -1;

	buf[strlen(buf)-1] = '\0';
	return 0;
}