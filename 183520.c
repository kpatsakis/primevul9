static bool _pam_send_password_expiry_message(struct pwb_context *ctx,
					      time_t next_change,
					      time_t now,
					      int warn_pwd_expire,
					      bool *already_expired,
					      bool *change_pwd)
{
	int days = 0;
	struct tm tm_now, tm_next_change;
	bool retval = false;
	int ret;

	if (already_expired) {
		*already_expired = false;
	}

	if (change_pwd) {
		*change_pwd = false;
	}

	if (next_change <= now) {
		PAM_WB_REMARK_DIRECT(ctx, "NT_STATUS_PASSWORD_EXPIRED");
		if (already_expired) {
			*already_expired = true;
		}
		return true;
	}

	if ((next_change < 0) ||
	    (next_change > now + warn_pwd_expire * SECONDS_PER_DAY)) {
		return false;
	}

	if ((localtime_r(&now, &tm_now) == NULL) ||
	    (localtime_r(&next_change, &tm_next_change) == NULL)) {
		return false;
	}

	days = (tm_next_change.tm_yday+tm_next_change.tm_year*365) -
	       (tm_now.tm_yday+tm_now.tm_year*365);

	if (days == 0) {
		ret = _make_remark(ctx, PAM_TEXT_INFO,
				_("Your password expires today.\n"));

		/*
		 * If change_pwd and already_expired is null.
		 * We are just sending a notification message.
		 * We don't expect any response in this case.
		 */

		if (!change_pwd && !already_expired) {
			return true;
		}

		/*
		 * successfully sent the warning message.
		 * Give the user a chance to change pwd.
		 */
		if (ret == PAM_SUCCESS) {
			if (change_pwd) {
				retval = _pam_winbind_change_pwd(ctx);
				if (retval) {
					*change_pwd = true;
				}
			}
		}
		return true;
	}

	if (days > 0 && days < warn_pwd_expire) {

		ret = _make_remark_format(ctx, PAM_TEXT_INFO,
					_("Your password will expire in %d %s.\n"),
					days, (days > 1) ? _("days"):_("day"));
		/*
		 * If change_pwd and already_expired is null.
		 * We are just sending a notification message.
		 * We don't expect any response in this case.
		 */

		if (!change_pwd && !already_expired) {
			return true;
		}

		/*
		 * successfully sent the warning message.
		 * Give the user a chance to change pwd.
		 */
		if (ret == PAM_SUCCESS) {
			if (change_pwd) {
				retval = _pam_winbind_change_pwd(ctx);
				if (retval) {
					*change_pwd = true;
				}
			}
		}
		return true;
	}

	return false;
}