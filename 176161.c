void gdCtxPrintf(gdIOCtx * out, const char *format, ...)
{
	char *buf;
	int len;
	va_list args;

	va_start(args, format);
	len = vspprintf(&buf, 0, format, args);
	va_end(args);
	out->putBuf(out, buf, len);
	efree(buf);
}