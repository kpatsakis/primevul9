static int _make_remark_v(struct pwb_context *ctx,
			  int type,
			  const char *format,
			  va_list args)
{
	char *var;
	int ret;

	ret = vasprintf(&var, format, args);
	if (ret < 0) {
		_pam_log(ctx, LOG_ERR, "memory allocation failure");
		return ret;
	}

	ret = _make_remark(ctx, type, var);
	SAFE_FREE(var);
	return ret;
}