static int winbind_auth_request(struct pwb_context *ctx,
				const char *user,
				const char *pass,
				const char *member,
				const char *cctype,
				const int warn_pwd_expire,
				struct wbcAuthErrorInfo **p_error,
				struct wbcLogonUserInfo **p_info,
				struct wbcUserPasswordPolicyInfo **p_policy,
				time_t *pwd_last_set,
				char **user_ret)
{
	wbcErr wbc_status;

	struct wbcLogonUserParams logon;
	char membership_of[1024];
	uid_t user_uid = -1;
	uint32_t flags = WBFLAG_PAM_INFO3_TEXT |
			 WBFLAG_PAM_GET_PWD_POLICY;

	struct wbcLogonUserInfo *info = NULL;
	struct wbcAuthUserInfo *user_info = NULL;
	struct wbcAuthErrorInfo *error = NULL;
	struct wbcUserPasswordPolicyInfo *policy = NULL;

	int ret = PAM_AUTH_ERR;
	int i;
	const char *codes[] = {
		"NT_STATUS_PASSWORD_EXPIRED",
		"NT_STATUS_PASSWORD_MUST_CHANGE",
		"NT_STATUS_INVALID_WORKSTATION",
		"NT_STATUS_INVALID_LOGON_HOURS",
		"NT_STATUS_ACCOUNT_EXPIRED",
		"NT_STATUS_ACCOUNT_DISABLED",
		"NT_STATUS_ACCOUNT_LOCKED_OUT",
		"NT_STATUS_NOLOGON_WORKSTATION_TRUST_ACCOUNT",
		"NT_STATUS_NOLOGON_SERVER_TRUST_ACCOUNT",
		"NT_STATUS_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT",
		"NT_STATUS_DOMAIN_CONTROLLER_NOT_FOUND",
		"NT_STATUS_NO_LOGON_SERVERS",
		"NT_STATUS_WRONG_PASSWORD",
		"NT_STATUS_ACCESS_DENIED"
	};

	if (pwd_last_set) {
		*pwd_last_set = 0;
	}

	/* Krb5 auth always has to go against the KDC of the user's realm */

	if (ctx->ctrl & WINBIND_KRB5_AUTH) {
		flags		|= WBFLAG_PAM_CONTACT_TRUSTDOM;
	}

	if (ctx->ctrl & (WINBIND_KRB5_AUTH|WINBIND_CACHED_LOGIN)) {
		struct passwd *pwd = NULL;

		pwd = getpwnam(user);
		if (pwd == NULL) {
			return PAM_USER_UNKNOWN;
		}
		user_uid	= pwd->pw_uid;
	}

	if (ctx->ctrl & WINBIND_KRB5_AUTH) {

		_pam_log_debug(ctx, LOG_DEBUG,
			       "enabling krb5 login flag\n");

		flags		|= WBFLAG_PAM_KRB5 |
				   WBFLAG_PAM_FALLBACK_AFTER_KRB5;
	}

	if (ctx->ctrl & WINBIND_CACHED_LOGIN) {
		_pam_log_debug(ctx, LOG_DEBUG,
			       "enabling cached login flag\n");
		flags		|= WBFLAG_PAM_CACHED_LOGIN;
	}

	if (user_ret) {
		*user_ret = NULL;
		flags		|= WBFLAG_PAM_UNIX_NAME;
	}

	if (cctype != NULL) {
		_pam_log_debug(ctx, LOG_DEBUG,
			       "enabling request for a %s krb5 ccache\n",
			       cctype);
	}

	if (member != NULL) {

		ZERO_STRUCT(membership_of);

		if (!winbind_name_list_to_sid_string_list(ctx, user, member,
							  membership_of,
							  sizeof(membership_of))) {
			_pam_log_debug(ctx, LOG_ERR,
				       "failed to serialize membership of sid "
				       "\"%s\"\n", member);
			return PAM_AUTH_ERR;
		}
	}

	ZERO_STRUCT(logon);

	logon.username			= user;
	logon.password			= pass;

	if (cctype) {
		wbc_status = wbcAddNamedBlob(&logon.num_blobs,
					     &logon.blobs,
					     "krb5_cc_type",
					     0,
					     (uint8_t *)cctype,
					     strlen(cctype)+1);
		if (!WBC_ERROR_IS_OK(wbc_status)) {
			goto done;
		}
	}

	wbc_status = wbcAddNamedBlob(&logon.num_blobs,
				     &logon.blobs,
				     "flags",
				     0,
				     (uint8_t *)&flags,
				     sizeof(flags));
	if (!WBC_ERROR_IS_OK(wbc_status)) {
		goto done;
	}

	wbc_status = wbcAddNamedBlob(&logon.num_blobs,
				     &logon.blobs,
				     "user_uid",
				     0,
				     (uint8_t *)&user_uid,
				     sizeof(user_uid));
	if (!WBC_ERROR_IS_OK(wbc_status)) {
		goto done;
	}

	if (member) {
		wbc_status = wbcAddNamedBlob(&logon.num_blobs,
					     &logon.blobs,
					     "membership_of",
					     0,
					     (uint8_t *)membership_of,
					     sizeof(membership_of));
		if (!WBC_ERROR_IS_OK(wbc_status)) {
			goto done;
		}
	}

	wbc_status = wbcLogonUser(&logon, &info, &error, &policy);
	ret = wbc_auth_error_to_pam_error(ctx, error, wbc_status,
					  user, "wbcLogonUser");
	wbcFreeMemory(logon.blobs);
	logon.blobs = NULL;

	if (info && info->info) {
		user_info = info->info;
	}

	if (pwd_last_set && user_info) {
		*pwd_last_set = user_info->pass_last_set_time;
	}

	if (p_info && info) {
		*p_info = info;
	}

	if (p_policy && policy) {
		*p_policy = policy;
	}

	if (p_error && error) {
		/* We want to process the error in the caller. */
		*p_error = error;
		return ret;
	}

	for (i=0; i<ARRAY_SIZE(codes); i++) {
		int _ret = ret;
		if (_pam_check_remark_auth_err(ctx, error, codes[i], &_ret)) {
			ret = _ret;
			goto done;
		}
	}

	if ((ret == PAM_SUCCESS) && user_info && policy && info) {

		bool already_expired = false;

		/* warn a user if the password is about to expire soon */
		_pam_warn_password_expiry(ctx, user_info, policy,
					  warn_pwd_expire,
					  &already_expired);

		if (already_expired == true) {

			SMB_TIME_T last_set = user_info->pass_last_set_time;

			_pam_log_debug(ctx, LOG_DEBUG,
				       "Password has expired "
				       "(Password was last set: %lld, "
				       "the policy says it should expire here "
				       "%lld (now it's: %lu))\n",
				       (long long int)last_set,
				       (long long int)last_set +
				       policy->expire,
				       time(NULL));

			return PAM_AUTHTOK_EXPIRED;
		}

		/* inform about logon type */
		_pam_warn_logon_type(ctx, user, user_info->user_flags);

		/* inform about krb5 failures */
		_pam_warn_krb5_failure(ctx, user, user_info->user_flags);

		/* set some info3 info for other modules in the stack */
		_pam_set_data_info3(ctx, user_info);

		/* put krb5ccname into env */
		_pam_setup_krb5_env(ctx, info);

		/* If winbindd returned a username, return the pointer to it
		 * here. */
		_pam_setup_unix_username(ctx, user_ret, info);
	}

 done:
	if (logon.blobs) {
		wbcFreeMemory(logon.blobs);
	}
	if (info && info->blobs) {
		wbcFreeMemory(info->blobs);
	}
	if (error && !p_error) {
		wbcFreeMemory(error);
	}
	if (info && !p_info) {
		wbcFreeMemory(info);
	}
	if (policy && !p_policy) {
		wbcFreeMemory(policy);
	}

	return ret;
}