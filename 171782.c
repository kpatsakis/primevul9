static char winbind_get_separator(struct pwb_context *ctx)
{
	wbcErr wbc_status;
	static struct wbcInterfaceDetails *details = NULL;

	wbc_status = wbcInterfaceDetails(&details);
	if (!WBC_ERROR_IS_OK(wbc_status)) {
		_pam_log(ctx, LOG_ERR,
			 "Could not retrieve winbind interface details: %s",
			 wbcErrorString(wbc_status));
		return '\0';
	}

	if (!details) {
		return '\0';
	}

	return details->winbind_separator;
}