static int winbind_chauthtok_request(struct pwb_context *ctx,
				     const char *user,
				     const char *oldpass,
				     const char *newpass,
				     time_t pwd_last_set)
{
	wbcErr wbc_status;
	struct wbcChangePasswordParams params;
	struct wbcAuthErrorInfo *error = NULL;
	struct wbcUserPasswordPolicyInfo *policy = NULL;
	enum wbcPasswordChangeRejectReason reject_reason = -1;
	uint32_t flags = 0;

	int i;
	const char *codes[] = {
		"NT_STATUS_BACKUP_CONTROLLER",
		"NT_STATUS_DOMAIN_CONTROLLER_NOT_FOUND",
		"NT_STATUS_NO_LOGON_SERVERS",
		"NT_STATUS_ACCESS_DENIED",
		"NT_STATUS_PWD_TOO_SHORT", /* TODO: tell the min pwd length ? */
		"NT_STATUS_PWD_TOO_RECENT", /* TODO: tell the minage ? */
		"NT_STATUS_PWD_HISTORY_CONFLICT" /* TODO: tell the history length ? */
	};
	int ret = PAM_AUTH_ERR;

	ZERO_STRUCT(params);

	if (ctx->ctrl & WINBIND_KRB5_AUTH) {
		flags |= WBFLAG_PAM_KRB5 |
			 WBFLAG_PAM_CONTACT_TRUSTDOM;
	}

	if (ctx->ctrl & WINBIND_CACHED_LOGIN) {
		flags |= WBFLAG_PAM_CACHED_LOGIN;
	}

	params.account_name		= user;
	params.level			= WBC_CHANGE_PASSWORD_LEVEL_PLAIN;
	params.old_password.plaintext	= oldpass;
	params.new_password.plaintext	= newpass;
	params.flags			= flags;

	wbc_status = wbcChangeUserPasswordEx(&params, &error, &reject_reason, &policy);
	ret = wbc_auth_error_to_pam_error(ctx, error, wbc_status,
					  user, "wbcChangeUserPasswordEx");

	if (WBC_ERROR_IS_OK(wbc_status)) {
		_pam_log(ctx, LOG_NOTICE,
			 "user '%s' password changed", user);
		return PAM_SUCCESS;
	}

	if (!error) {
		wbcFreeMemory(policy);
		return ret;
	}

	for (i=0; i<ARRAY_SIZE(codes); i++) {
		int _ret = ret;
		if (_pam_check_remark_auth_err(ctx, error, codes[i], &_ret)) {
			ret = _ret;
			goto done;
		}
	}

	if (!strcasecmp(error->nt_string,
			"NT_STATUS_PASSWORD_RESTRICTION")) {

		char *pwd_restriction_string = NULL;
		SMB_TIME_T min_pwd_age = 0;

		if (policy) {
			min_pwd_age	= policy->min_passwordage;
		}

		/* FIXME: avoid to send multiple PAM messages after another */
		switch (reject_reason) {
			case -1:
				break;
			case WBC_PWD_CHANGE_NO_ERROR:
				if ((min_pwd_age > 0) &&
				    (pwd_last_set + min_pwd_age > time(NULL))) {
					PAM_WB_REMARK_DIRECT(ctx,
					     "NT_STATUS_PWD_TOO_RECENT");
				}
				break;
			case WBC_PWD_CHANGE_PASSWORD_TOO_SHORT:
				PAM_WB_REMARK_DIRECT(ctx,
					"NT_STATUS_PWD_TOO_SHORT");
				break;
			case WBC_PWD_CHANGE_PWD_IN_HISTORY:
				PAM_WB_REMARK_DIRECT(ctx,
					"NT_STATUS_PWD_HISTORY_CONFLICT");
				break;
			case WBC_PWD_CHANGE_NOT_COMPLEX:
				_make_remark(ctx, PAM_ERROR_MSG,
					     _("Password does not meet "
					       "complexity requirements"));
				break;
			default:
				_pam_log_debug(ctx, LOG_DEBUG,
					       "unknown password change "
					       "reject reason: %d",
					       reject_reason);
				break;
		}

		pwd_restriction_string =
			_pam_compose_pwd_restriction_string(ctx, policy);
		if (pwd_restriction_string) {
			_make_remark(ctx, PAM_ERROR_MSG,
				     pwd_restriction_string);
			TALLOC_FREE(pwd_restriction_string);
		}
	}
 done:
	wbcFreeMemory(error);
	wbcFreeMemory(policy);

	return ret;
}