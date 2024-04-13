static void _pam_setup_krb5_env(struct pwb_context *ctx,
				struct wbcLogonUserInfo *info)
{
	char *var = NULL;
	int ret;
	uint32_t i;
	const char *krb5ccname = NULL;

	if (off(ctx->ctrl, WINBIND_KRB5_AUTH)) {
		return;
	}

	if (!info) {
		return;
	}

	for (i=0; i < info->num_blobs; i++) {
		if (strcasecmp(info->blobs[i].name, "krb5ccname") == 0) {
			krb5ccname = (const char *)info->blobs[i].blob.data;
			break;
		}
	}

	if (!krb5ccname || (strlen(krb5ccname) == 0)) {
		return;
	}

	_pam_log_debug(ctx, LOG_DEBUG,
		       "request returned KRB5CCNAME: %s", krb5ccname);

	if (asprintf(&var, "KRB5CCNAME=%s", krb5ccname) == -1) {
		return;
	}

	ret = pam_putenv(ctx->pamh, var);
	if (ret) {
		_pam_log(ctx, LOG_ERR,
			 "failed to set KRB5CCNAME to %s: %s",
			 var, pam_strerror(ctx->pamh, ret));
	}
	free(var);
}