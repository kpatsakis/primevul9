input_reply(struct input_ctx *ictx, const char *fmt, ...)
{
	va_list	ap;
	char   *reply;

	va_start(ap, fmt);
	vasprintf(&reply, fmt, ap);
	va_end(ap);

	bufferevent_write(ictx->wp->event, reply, strlen(reply));
	free(reply);
}