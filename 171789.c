int pam_sm_setcred(pam_handle_t *pamh, int flags,
		   int argc, const char **argv)
{
	int ret = PAM_SYSTEM_ERR;
	struct pwb_context *ctx = NULL;

	ret = _pam_winbind_init_context(pamh, flags, argc, argv, &ctx);
	if (ret) {
		goto out;
	}

	_PAM_LOG_FUNCTION_ENTER("pam_sm_setcred", ctx);

	switch (flags & ~PAM_SILENT) {

		case PAM_DELETE_CRED:
			ret = _pam_delete_cred(pamh, flags, argc, argv);
			break;
		case PAM_REFRESH_CRED:
			_pam_log_debug(ctx, LOG_WARNING,
				       "PAM_REFRESH_CRED not implemented");
			ret = PAM_SUCCESS;
			break;
		case PAM_REINITIALIZE_CRED:
			_pam_log_debug(ctx, LOG_WARNING,
				       "PAM_REINITIALIZE_CRED not implemented");
			ret = PAM_SUCCESS;
			break;
		case PAM_ESTABLISH_CRED:
			_pam_log_debug(ctx, LOG_WARNING,
				       "PAM_ESTABLISH_CRED not implemented");
			ret = PAM_SUCCESS;
			break;
		default:
			ret = PAM_SYSTEM_ERR;
			break;
	}

 out:

	_PAM_LOG_FUNCTION_LEAVE("pam_sm_setcred", ctx, ret);

	TALLOC_FREE(ctx);

	return ret;
}