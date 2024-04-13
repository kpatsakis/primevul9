void util_print_binary(FILE *f, const u8 *buf, int count)
{
	int i;

	for (i = 0; i < count; i++) {
		unsigned char c = buf[i];
		const char *format;
		if (!isprint(c))
			format = "\\x%02X";
		else
			format = "%c";
		fprintf(f, format, c);
	}
	(void) fflush(f);
}