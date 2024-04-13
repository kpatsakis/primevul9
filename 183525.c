static bool _pam_require_krb5_auth_after_chauthtok(struct pwb_context *ctx,
						   const char *user)
{

	/* Make sure that we only do this if a) the chauthtok got initiated
	 * during a logon attempt (authenticate->acct_mgmt->chauthtok) b) any
	 * later password change via the "passwd" command if done by the user
	 * itself
	 * NB. If we login from gdm or xdm and the password expires,
	 * we change the password, but there is no memory cache.
	 * Thus, even for passthrough login, we should do the
	 * authentication again to update memory cache.
	 * --- BoYang
	 * */

	char *new_authtok_reqd_during_auth = NULL;
	struct passwd *pwd = NULL;

	pam_get_data(ctx->pamh, PAM_WINBIND_NEW_AUTHTOK_REQD_DURING_AUTH,
		      (const void **) &new_authtok_reqd_during_auth);
	pam_set_data(ctx->pamh, PAM_WINBIND_NEW_AUTHTOK_REQD_DURING_AUTH,
		     NULL, NULL);

	if (new_authtok_reqd_during_auth) {
		return true;
	}

	pwd = getpwnam(user);
	if (!pwd) {
		return false;
	}

	if (getuid() == pwd->pw_uid) {
		return true;
	}

	return false;
}