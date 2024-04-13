static int _pam_delete_cred(pam_handle_t *pamh, int flags,
			 int argc, const char **argv)
{
	int retval = PAM_SUCCESS;
	struct pwb_context *ctx = NULL;
	struct wbcLogoffUserParams logoff;
	struct wbcAuthErrorInfo *error = NULL;
	const char *user;
	wbcErr wbc_status = WBC_ERR_SUCCESS;

	ZERO_STRUCT(logoff);

	retval = _pam_winbind_init_context(pamh, flags, argc, argv, &ctx);
	if (retval) {
		goto out;
	}

	_PAM_LOG_FUNCTION_ENTER("_pam_delete_cred", ctx);

	if (ctx->ctrl & WINBIND_KRB5_AUTH) {

		/* destroy the ccache here */

		uint32_t wbc_flags = 0;
		const char *ccname = NULL;
		struct passwd *pwd = NULL;

		retval = pam_get_user(pamh, &user, _("Username: "));
		if (retval) {
			_pam_log(ctx, LOG_ERR,
				 "could not identify user");
			goto out;
		}

		if (user == NULL) {
			_pam_log(ctx, LOG_ERR,
				 "username was NULL!");
			retval = PAM_USER_UNKNOWN;
			goto out;
		}

		_pam_log_debug(ctx, LOG_DEBUG,
			       "username [%s] obtained", user);

		ccname = pam_getenv(pamh, "KRB5CCNAME");
		if (ccname == NULL) {
			_pam_log_debug(ctx, LOG_DEBUG,
				       "user has no KRB5CCNAME environment");
		}

		pwd = getpwnam(user);
		if (pwd == NULL) {
			retval = PAM_USER_UNKNOWN;
			goto out;
		}

		wbc_flags = WBFLAG_PAM_KRB5 |
			WBFLAG_PAM_CONTACT_TRUSTDOM;

		logoff.username		= user;

		if (ccname) {
			wbc_status = wbcAddNamedBlob(&logoff.num_blobs,
						     &logoff.blobs,
						     "ccfilename",
						     0,
						     discard_const_p(uint8_t, ccname),
						     strlen(ccname)+1);
			if (!WBC_ERROR_IS_OK(wbc_status)) {
				goto out;
			}
		}

		wbc_status = wbcAddNamedBlob(&logoff.num_blobs,
					     &logoff.blobs,
					     "flags",
					     0,
					     (uint8_t *)&wbc_flags,
					     sizeof(wbc_flags));
		if (!WBC_ERROR_IS_OK(wbc_status)) {
			goto out;
		}

		wbc_status = wbcAddNamedBlob(&logoff.num_blobs,
					     &logoff.blobs,
					     "user_uid",
					     0,
					     (uint8_t *)&pwd->pw_uid,
					     sizeof(pwd->pw_uid));
		if (!WBC_ERROR_IS_OK(wbc_status)) {
			goto out;
		}

		wbc_status = wbcLogoffUserEx(&logoff, &error);
		retval = wbc_auth_error_to_pam_error(ctx, error, wbc_status,
						     user, "wbcLogoffUser");
		wbcFreeMemory(error);
		wbcFreeMemory(logoff.blobs);
		logoff.blobs = NULL;

		if (!WBC_ERROR_IS_OK(wbc_status)) {
			_pam_log(ctx, LOG_INFO,
				 "failed to logoff user %s: %s\n",
					 user, wbcErrorString(wbc_status));
		}
	}

out:
	if (logoff.blobs) {
		wbcFreeMemory(logoff.blobs);
	}

	if (!WBC_ERROR_IS_OK(wbc_status)) {
		retval = wbc_auth_error_to_pam_error(ctx, error, wbc_status,
		     user, "wbcLogoffUser");
	}

	/*
	 * Delete the krb5 ccname variable from the PAM environment
	 * if it was set by winbind.
	 */
	if ((ctx->ctrl & WINBIND_KRB5_AUTH) && pam_getenv(pamh, "KRB5CCNAME")) {
		pam_putenv(pamh, "KRB5CCNAME");
	}

	_PAM_LOG_FUNCTION_LEAVE("_pam_delete_cred", ctx, retval);

	TALLOC_FREE(ctx);

	return retval;
}