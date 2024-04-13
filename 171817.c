static void _pam_warn_password_expiry(struct pwb_context *ctx,
				      const struct wbcAuthUserInfo *info,
				      const struct wbcUserPasswordPolicyInfo *policy,
				      int warn_pwd_expire,
				      bool *already_expired)
{
	time_t now = time(NULL);
	time_t next_change = 0;

	if (!info || !policy) {
		return;
	}

	if (already_expired) {
		*already_expired = false;
	}

	/* accounts with WBC_ACB_PWNOEXP set never receive a warning */
	if (info->acct_flags & WBC_ACB_PWNOEXP) {
		return;
	}

	/* no point in sending a warning if this is a grace logon */
	if (PAM_WB_GRACE_LOGON(info->user_flags)) {
		return;
	}

	/* check if the info3 must change timestamp has been set */
	next_change = info->pass_must_change_time;

	if (_pam_send_password_expiry_message(ctx, next_change, now,
					      warn_pwd_expire,
					      already_expired)) {
		return;
	}

	/* now check for the global password policy */
	/* good catch from Ralf Haferkamp: an expiry of "never" is translated
	 * to -1 */
	if ((policy->expire == (int64_t)-1) ||
	    (policy->expire == 0)) {
		return;
	}

	next_change = info->pass_last_set_time + policy->expire;

	if (_pam_send_password_expiry_message(ctx, next_change, now,
					      warn_pwd_expire,
					      already_expired)) {
		return;
	}

	/* no warning sent */
}