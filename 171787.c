static bool _pam_send_password_expiry_message(struct pwb_context *ctx,
					      time_t next_change,
					      time_t now,
					      int warn_pwd_expire,
					      bool *already_expired)
{
	int days = 0;
	struct tm tm_now, tm_next_change;

	if (already_expired) {
		*already_expired = false;
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
		_make_remark(ctx, PAM_TEXT_INFO,
			     _("Your password expires today"));
		return true;
	}

	if (days > 0 && days < warn_pwd_expire) {
		_make_remark_format(ctx, PAM_TEXT_INFO,
				    _("Your password will expire in %d %s"),
				    days, (days > 1) ? _("days"):_("day"));
		return true;
	}

	return false;
}