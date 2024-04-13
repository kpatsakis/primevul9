void util_hex_dump(FILE *f, const u8 *in, int len, const char *sep)
{
	int i;

	for (i = 0; i < len; i++) {
		if (sep != NULL && i)
			fprintf(f, "%s", sep);
		fprintf(f, "%02X", in[i]);
	}
}