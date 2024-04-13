static bool _pam_check_remark_auth_err(struct pwb_context *ctx,
				       const struct wbcAuthErrorInfo *e,
				       const char *nt_status_string,
				       int *pam_err)
{
	const char *ntstatus = NULL;
	const char *error_string = NULL;

	if (!e || !pam_err) {
		return false;
	}

	ntstatus = e->nt_string;
	if (!ntstatus) {
		return false;
	}

	if (strcasecmp(ntstatus, nt_status_string) == 0) {

		error_string = _get_ntstatus_error_string(nt_status_string);
		if (error_string) {
			_make_remark(ctx, PAM_ERROR_MSG, error_string);
			*pam_err = e->pam_error;
			return true;
		}

		if (e->display_string) {
			_make_remark(ctx, PAM_ERROR_MSG, _(e->display_string));
			*pam_err = e->pam_error;
			return true;
		}

		_make_remark(ctx, PAM_ERROR_MSG, nt_status_string);
		*pam_err = e->pam_error;

		return true;
	}

	return false;
};