print_prompt(FILE *in, FILE *out, char *prompt)
{
	int infd = fileno(in);
	struct termios tio;

	if (!isatty(infd))
		return;

	fprintf(out, "%s", prompt);
	fflush(out);

	tcgetattr(infd, &tio);
	tio.c_lflag &= ~ECHO;
	tcsetattr(infd, TCSAFLUSH, &tio);
}