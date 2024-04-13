readpw(PK11SlotInfo *slot UNUSED,
       PRBool retry UNUSED,
       void *arg UNUSED)
{
	struct termios sio, tio;
	char line[LINE_MAX], *p;
	char *ret;

	ingress();
	memset(line, '\0', sizeof (line));

	if (tcgetattr(fileno(stdin), &sio) < 0) {
		warnx("Could not read password from standard input.");
		return NULL;
	}
	tio = sio;
	tio.c_lflag &= ~ECHO;
	if (tcsetattr(fileno(stdin), 0, &tio) < 0) {
		warnx("Could not read password from standard input.");
		return NULL;
	}

	fprintf(stdout, "Enter passphrase for private key: ");
	fflush(stdout);
	ret = fgets(line, sizeof(line), stdin);
	set_errno_guard();
	tcsetattr(fileno(stdin), 0, &sio);
	fprintf(stdout, "\n");
	fflush(stdout);
	if (ret == NULL)
		return NULL;

	p = line + strcspn(line, "\r\n");
	if (p == NULL)
		p = line + strcspn(line, "\n");
	if (p != NULL)
		*p = '\0';

	ret = strdup(line);
	memset(line, '\0', sizeof (line));
	if (!ret) {
		warnx("Could not read passphrase.");
		return NULL;
	}
	egress();
	return ret;
}