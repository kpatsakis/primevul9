smtp_proceed_noop(struct smtp_session *s, const char *args)
{
	smtp_reply(s, "250 %s Ok",
	    esc_code(ESC_STATUS_OK, ESC_OTHER_STATUS));
}