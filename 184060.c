smtp_proceed_quit(struct smtp_session *s, const char *args)
{
	smtp_reply(s, "221 %s Bye",
	    esc_code(ESC_STATUS_OK, ESC_OTHER_STATUS));
	smtp_enter_state(s, STATE_QUIT);
}