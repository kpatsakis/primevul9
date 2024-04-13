static int valid_user(struct pwb_context *ctx,
		      const char *user)
{
	/* check not only if the user is available over NSS calls, also make
	 * sure it's really a winbind user, this is important when stacking PAM
	 * modules in the 'account' or 'password' facility. */

	wbcErr wbc_status;
	struct passwd *pwd = NULL;
	struct passwd *wb_pwd = NULL;

	pwd = getpwnam(user);
	if (pwd == NULL) {
		return 1;
	}

	wbc_status = wbcGetpwnam(user, &wb_pwd);
	wbcFreeMemory(wb_pwd);
	if (!WBC_ERROR_IS_OK(wbc_status)) {
		_pam_log(ctx, LOG_DEBUG, "valid_user: wbcGetpwnam gave %s\n",
			wbcErrorString(wbc_status));
	}

	switch (wbc_status) {
		case WBC_ERR_UNKNOWN_USER:
			return 1;
		case WBC_ERR_SUCCESS:
			return 0;
		default:
			break;
	}
	return -1;
}