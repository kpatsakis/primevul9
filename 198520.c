void dump_binary(void *buf, size_t buf_len)
{
#ifdef _WIN32
	_setmode(fileno(stdout), _O_BINARY);
#endif
	fwrite(buf, 1, buf_len, stdout);
#ifdef _WIN32
	_setmode(fileno(stdout), _O_TEXT);
#endif
}