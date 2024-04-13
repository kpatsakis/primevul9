int pam_sm_open_session(pam_handle_t *pamh, int flags,
			int argc, const char **argv)
{
	int ret = PAM_SUCCESS;
	struct pwb_context *ctx = NULL;

	ret = _pam_winbind_init_context(pamh, flags, argc, argv, &ctx);
	if (ret) {
		goto out;
	}

	_PAM_LOG_FUNCTION_ENTER("pam_sm_open_session", ctx);

	if (ctx->ctrl & WINBIND_MKHOMEDIR) {
		/* check and create homedir */
		ret = _pam_mkhomedir(ctx);
	}
 out:
	_PAM_LOG_FUNCTION_LEAVE("pam_sm_open_session", ctx, ret);

	TALLOC_FREE(ctx);

	return ret;
}