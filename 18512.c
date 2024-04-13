get_password(FILE *input, FILE *output, char *prompt, PRBool (*ok)(char *))
{
	int infd = fileno(input);
	char phrase[200];
	size_t size = sizeof(phrase);

	ingress();
	memset(phrase, 0, size);

	while(true) {
		int rc;

		print_prompt(input, output, prompt);
		rc = read_password(input, output, phrase, size);
		if (rc < 0)
			return NULL;

		if (!ok)
			break;

		if ((*ok)(phrase))
			break;

		if (!isatty(infd))
			return NULL;
		fprintf(output, "Password does not meet requirements.\n");
		fflush(output);
	}

	egress();
	return (char *)PORT_Strdup(phrase);
}