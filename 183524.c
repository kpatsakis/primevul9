int pam_sm_authenticate(pam_handle_t *pamh, int flags,
			int argc, const char **argv)
{
	const char *username;
	const char *password;
	const char *member = NULL;
	const char *cctype = NULL;
	int warn_pwd_expire;
	int retval = PAM_AUTH_ERR;
	char *username_ret = NULL;
	char *new_authtok_required = NULL;
	char *real_username = NULL;
	struct pwb_context *ctx = NULL;

	retval = _pam_winbind_init_context(pamh, flags, argc, argv, &ctx);
	if (retval) {
		goto out;
	}

	_PAM_LOG_FUNCTION_ENTER("pam_sm_authenticate", ctx);

	/* Get the username */
	retval = pam_get_user(pamh, &username, NULL);
	if ((retval != PAM_SUCCESS) || (!username)) {
		_pam_log_debug(ctx, LOG_DEBUG,
			       "can not get the username");
		retval = PAM_SERVICE_ERR;
		goto out;
	}


#if defined(AIX)
	/* Decode the user name since AIX does not support logn user
	   names by default.  The name is encoded as _#uid.  */

	if (username[0] == '_') {
		uid_t id = atoi(&username[1]);
		struct passwd *pw = NULL;

		if ((id!=0) && ((pw = getpwuid(id)) != NULL)) {
			real_username = strdup(pw->pw_name);
		}
	}
#endif

	if (!real_username) {
		/* Just making a copy of the username we got from PAM */
		if ((real_username = strdup(username)) == NULL) {
			_pam_log_debug(ctx, LOG_DEBUG,
				       "memory allocation failure when copying "
				       "username");
			retval = PAM_SERVICE_ERR;
			goto out;
		}
	}

	/* Maybe this was a UPN */

	if (strchr(real_username, '@') != NULL) {
		char *samaccountname = NULL;

		samaccountname = winbind_upn_to_username(ctx,
							 real_username);
		if (samaccountname) {
			free(real_username);
			real_username = strdup(samaccountname);
		}
	}

	retval = _winbind_read_password(ctx, ctx->ctrl, NULL,
					_("Password: "), NULL,
					&password);

	if (retval != PAM_SUCCESS) {
		_pam_log(ctx, LOG_ERR,
			 "Could not retrieve user's password");
		retval = PAM_AUTHTOK_ERR;
		goto out;
	}

	/* Let's not give too much away in the log file */

#ifdef DEBUG_PASSWORD
	_pam_log_debug(ctx, LOG_INFO,
		       "Verify user '%s' with password '%s'",
		       real_username, password);
#else
	_pam_log_debug(ctx, LOG_INFO,
		       "Verify user '%s'", real_username);
#endif

	member = get_member_from_config(ctx);
	cctype = get_krb5_cc_type_from_config(ctx);
	warn_pwd_expire = get_warn_pwd_expire_from_config(ctx);

	/* Now use the username to look up password */
	retval = winbind_auth_request(ctx, real_username, password,
				      member, cctype, warn_pwd_expire,
				      NULL, NULL, NULL,
				      NULL, &username_ret);

	if (retval == PAM_NEW_AUTHTOK_REQD ||
	    retval == PAM_AUTHTOK_EXPIRED) {

		char *new_authtok_required_during_auth = NULL;

		new_authtok_required = talloc_asprintf(NULL, "%d", retval);
		if (!new_authtok_required) {
			retval = PAM_BUF_ERR;
			goto out;
		}

		pam_set_data(pamh, PAM_WINBIND_NEW_AUTHTOK_REQD,
			     new_authtok_required,
			     _pam_winbind_cleanup_func);

		retval = PAM_SUCCESS;

		new_authtok_required_during_auth = talloc_asprintf(NULL, "%d", true);
		if (!new_authtok_required_during_auth) {
			retval = PAM_BUF_ERR;
			goto out;
		}

		pam_set_data(pamh, PAM_WINBIND_NEW_AUTHTOK_REQD_DURING_AUTH,
			     new_authtok_required_during_auth,
			     _pam_winbind_cleanup_func);

		goto out;
	}

out:
	if (username_ret) {
		pam_set_item (pamh, PAM_USER, username_ret);
		_pam_log_debug(ctx, LOG_INFO,
			       "Returned user was '%s'", username_ret);
		free(username_ret);
	}

	if (real_username) {
		free(real_username);
	}

	if (!new_authtok_required) {
		pam_set_data(pamh, PAM_WINBIND_NEW_AUTHTOK_REQD, NULL, NULL);
	}

	if (retval != PAM_SUCCESS) {
		_pam_free_data_info3(pamh);
	}

	if (ctx != NULL) {
		_PAM_LOG_FUNCTION_LEAVE("pam_sm_authenticate", ctx, retval);
		TALLOC_FREE(ctx);
	}

	return retval;
}