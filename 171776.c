static void _pam_warn_krb5_failure(struct pwb_context *ctx,
				   const char *username,
				   uint32_t info3_user_flgs)
{
	if (PAM_WB_KRB5_CLOCK_SKEW(info3_user_flgs)) {
		_make_remark(ctx, PAM_ERROR_MSG,
			     _("Failed to establish your Kerberos Ticket cache "
			       "due time differences\n"
			       "with the domain controller.  "
			       "Please verify the system time.\n"));
		_pam_log_debug(ctx, LOG_DEBUG,
			       "User %s: Clock skew when getting Krb5 TGT\n",
			       username);
	}
}