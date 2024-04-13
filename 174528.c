email_write_header_string(FILE *stream, const char *data)
{
	for (; *data; ++data) {
		char ch = *data;
		if (ch < ' ') {
			fputc(' ', stream);
		} else {
			fputc(ch, stream);
		}
	}
}