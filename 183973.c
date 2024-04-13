smtp_proceed_starttls(struct smtp_session *s, const char *args)
{
	smtp_reply(s, "220 %s Ready to start TLS",
	    esc_code(ESC_STATUS_OK, ESC_OTHER_STATUS));
	smtp_enter_state(s, STATE_TLS);
}