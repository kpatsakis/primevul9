static int ath6kl_wmi_error_event_rx(struct wmi *wmi, u8 *datap, int len)
{
	const char *type = "unknown error";
	struct wmi_cmd_error_event *ev;
	ev = (struct wmi_cmd_error_event *) datap;

	switch (ev->err_code) {
	case INVALID_PARAM:
		type = "invalid parameter";
		break;
	case ILLEGAL_STATE:
		type = "invalid state";
		break;
	case INTERNAL_ERROR:
		type = "internal error";
		break;
	}

	ath6kl_dbg(ATH6KL_DBG_WMI, "programming error, cmd=%d %s\n",
		   ev->cmd_id, type);

	return 0;
}