smtp_strstate(int state)
{
	static char	buf[32];

	switch (state) {
	CASE(STATE_NEW);
	CASE(STATE_CONNECTED);
	CASE(STATE_TLS);
	CASE(STATE_HELO);
	CASE(STATE_AUTH_INIT);
	CASE(STATE_AUTH_USERNAME);
	CASE(STATE_AUTH_PASSWORD);
	CASE(STATE_AUTH_FINALIZE);
	CASE(STATE_BODY);
	CASE(STATE_QUIT);
	default:
		(void)snprintf(buf, sizeof(buf), "STATE_??? (%d)", state);
		return (buf);
	}
}