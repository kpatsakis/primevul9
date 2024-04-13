seamless_send(const char *command, const char *format, ...)
{
	STREAM s;
	size_t len;
	va_list argp;
	char *escaped, buf[1025];

	len = snprintf(buf, sizeof(buf) - 1, "%s,%u,", command, seamless_serial);

	assert(len < (sizeof(buf) - 1));

	va_start(argp, format);
	len += vsnprintf(buf + len, sizeof(buf) - len - 1, format, argp);
	va_end(argp);

	assert(len < (sizeof(buf) - 1));

	escaped = utils_string_escape(buf);
	len = snprintf(buf, sizeof(buf), "%s", escaped);
	free(escaped);
	assert(len < (sizeof(buf) - 1));

	buf[len] = '\n';
	buf[len + 1] = '\0';

	len++;

	s = channel_init(seamless_channel, len);
	out_uint8p(s, buf, len) s_mark_end(s);

	DEBUG_SEAMLESS(("seamlessrdp sending:%s", buf));

#if 0
	printf("seamless send:\n");
	hexdump(s->channel_hdr + 8, s->end - s->channel_hdr - 8);
#endif

	channel_send(s, seamless_channel);

	return seamless_serial++;
}