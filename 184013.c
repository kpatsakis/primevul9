smtp_proceed_wiz(struct smtp_session *s, const char *args)
{
	smtp_reply(s, "500 %s %s: this feature is not supported yet ;-)",
	    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
	    esc_description(ESC_INVALID_COMMAND));
}