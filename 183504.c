static bool _pam_winbind_change_pwd(struct pwb_context *ctx)
{
	struct pam_message msg, *pmsg;
	struct pam_response *resp = NULL;
	int ret;
	bool retval = false;
	pmsg = &msg;
	msg.msg_style = PAM_RADIO_TYPE;
	msg.msg = _("Do you want to change your password now?");
	ret = converse(ctx->pamh, 1, &pmsg, &resp);
	if (resp == NULL) {
		if (ret == PAM_SUCCESS) {
			_pam_log(ctx, LOG_CRIT, "pam_winbind: system error!\n");
			return false;
		}
	}
	if (ret != PAM_SUCCESS) {
		return false;
	}
	_pam_log(ctx, LOG_CRIT, "Received [%s] reply from application.\n", resp->resp);

	if ((resp->resp != NULL) && (strcasecmp(resp->resp, "yes") == 0)) {
		retval = true;
	}

	_pam_drop_reply(resp, 1);
	return retval;
}