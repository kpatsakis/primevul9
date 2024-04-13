static void safe_fputs(const char *data)
{
	const char *p;

	for (p = data; p && *p; p++) {
		if (iscntrl((unsigned char) *p))
			fputc('?', stdout);
		else
			fputc(*p, stdout);
	}
}