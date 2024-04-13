#define GETS_FETCH_SIZE 8196LU
static char *php_mail_gets(readfn_t f, void *stream, unsigned long size, GETS_DATA *md) /* {{{ */
{

	/*	write to the gets stream if it is set,
		otherwise forward to c-clients gets */
	if (IMAPG(gets_stream)) {
		char buf[GETS_FETCH_SIZE];

		while (size) {
			unsigned long read;

			if (size > GETS_FETCH_SIZE) {
				read = GETS_FETCH_SIZE;
				size -=GETS_FETCH_SIZE;
			} else {
				read = size;
				size = 0;
			}

			if (!f(stream, read, buf)) {
				php_error_docref(NULL, E_WARNING, "Failed to read from socket");
				break;
			} else if (read != php_stream_write(IMAPG(gets_stream), buf, read)) {
				php_error_docref(NULL, E_WARNING, "Failed to write to stream");
				break;
			}
		}
		return NULL;
	} else {
		char *buf = pemalloc(size + 1, 1);

		if (f(stream, size, buf)) {
			buf[size] = '\0';
		} else {
			php_error_docref(NULL, E_WARNING, "Failed to read from socket");
			free(buf);
			buf = NULL;
		}
		return buf;
	}