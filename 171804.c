static int wbc_auth_error_to_pam_error(struct pwb_context *ctx,
				       struct wbcAuthErrorInfo *e,
				       wbcErr status,
				       const char *username,
				       const char *fn)
{
	int ret = PAM_AUTH_ERR;

	if (WBC_ERROR_IS_OK(status)) {
		_pam_log_debug(ctx, LOG_DEBUG, "request %s succeeded",
			fn);
		ret = PAM_SUCCESS;
		return pam_winbind_request_log(ctx, ret, username, fn);
	}

	if (e) {
		if (e->pam_error != PAM_SUCCESS) {
			_pam_log(ctx, LOG_ERR,
				 "request %s failed: %s, "
				 "PAM error: %s (%d), NTSTATUS: %s, "
				 "Error message was: %s",
				 fn,
				 wbcErrorString(status),
				 _pam_error_code_str(e->pam_error),
				 e->pam_error,
				 e->nt_string,
				 e->display_string);
			ret = e->pam_error;
			return pam_winbind_request_log(ctx, ret, username, fn);
		}

		_pam_log(ctx, LOG_ERR, "request %s failed, but PAM error 0!", fn);

		ret = PAM_SERVICE_ERR;
		return pam_winbind_request_log(ctx, ret, username, fn);
	}

	ret = wbc_error_to_pam_error(status);
	return pam_winbind_request_log(ctx, ret, username, fn);
}