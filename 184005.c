smtp_auth_failure_resume(int fd, short event, void *p)
{
	struct smtp_session *s = p;

	smtp_reply(s, "535 Authentication failed");
	smtp_enter_state(s, STATE_HELO);
}