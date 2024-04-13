static void _pam_set_data_string(struct pwb_context *ctx,
				 const char *data_name,
				 const char *value)
{
	int ret;

	if (!data_name || !value || (strlen(data_name) == 0) ||
	     (strlen(value) == 0)) {
		return;
	}

	ret = pam_set_data(ctx->pamh, data_name, talloc_strdup(NULL, value),
			   _pam_winbind_cleanup_func);
	if (ret) {
		_pam_log_debug(ctx, LOG_DEBUG,
			       "Could not set data %s: %s\n",
			       data_name, pam_strerror(ctx->pamh, ret));
	}
}