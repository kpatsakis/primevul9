smtp_enter_state(struct smtp_session *s, int newstate)
{
	log_trace(TRACE_SMTP, "smtp: %p: %s -> %s", s,
	    smtp_strstate(s->state),
	    smtp_strstate(newstate));

	s->state = newstate;
}