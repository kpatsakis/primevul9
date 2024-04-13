static int _make_remark_format(struct pwb_context *ctx, int type, const char *format, ...)
{
	int ret;
	va_list args;

	va_start(args, format);
	ret = _make_remark_v(ctx, type, format, args);
	va_end(args);
	return ret;
}