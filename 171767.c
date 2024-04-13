static void _pam_warn_logon_type(struct pwb_context *ctx,
				 const char *username,
				 uint32_t info3_user_flgs)
{
	/* inform about logon type */
	if (PAM_WB_GRACE_LOGON(info3_user_flgs)) {

		_make_remark(ctx, PAM_ERROR_MSG,
			     _("Grace login. "
			       "Please change your password as soon you're "
			       "online again"));
		_pam_log_debug(ctx, LOG_DEBUG,
			       "User %s logged on using grace logon\n",
			       username);

	} else if (PAM_WB_CACHED_LOGON(info3_user_flgs)) {

		_make_remark(ctx, PAM_ERROR_MSG,
			     _("Domain Controller unreachable, "
			       "using cached credentials instead. "
			       "Network resources may be unavailable"));
		_pam_log_debug(ctx, LOG_DEBUG,
			       "User %s logged on using cached credentials\n",
			       username);
	}
}