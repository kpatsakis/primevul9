static int _make_remark(struct pwb_context *ctx,
			int type,
			const char *text)
{
	int retval = PAM_SUCCESS;

	struct pam_message *pmsg[1], msg[1];
	struct pam_response *resp;

	if (ctx->flags & WINBIND_SILENT) {
		return PAM_SUCCESS;
	}

	pmsg[0] = &msg[0];
	msg[0].msg = discard_const_p(char, text);
	msg[0].msg_style = type;

	resp = NULL;
	retval = converse(ctx->pamh, 1, pmsg, &resp);

	if (resp) {
		_pam_drop_reply(resp, 1);
	}
	return retval;
}