smtp_rfc4954_auth_login(struct smtp_session *s, char *arg)
{
	char		buf[LINE_MAX];

	switch (s->state) {
	case STATE_HELO:
		smtp_enter_state(s, STATE_AUTH_USERNAME);
		if (arg != NULL && *arg != '\0') {
			smtp_rfc4954_auth_login(s, arg);
			return;
		}
		smtp_reply(s, "334 VXNlcm5hbWU6");
		return;

	case STATE_AUTH_USERNAME:
		memset(s->username, 0, sizeof(s->username));
		if (base64_decode(arg, (unsigned char *)s->username,
				  sizeof(s->username) - 1) == -1)
			goto abort;

		smtp_enter_state(s, STATE_AUTH_PASSWORD);
		smtp_reply(s, "334 UGFzc3dvcmQ6");
		return;

	case STATE_AUTH_PASSWORD:
		memset(buf, 0, sizeof(buf));
		if (base64_decode(arg, (unsigned char *)buf,
				  sizeof(buf)-1) == -1)
			goto abort;

		m_create(p_lka,  IMSG_SMTP_AUTHENTICATE, 0, 0, -1);
		m_add_id(p_lka, s->id);
		m_add_string(p_lka, s->listener->authtable);
		m_add_string(p_lka, s->username);
		m_add_string(p_lka, buf);
		m_close(p_lka);
		tree_xset(&wait_parent_auth, s->id, s);
		return;

	default:
		fatal("smtp_rfc4954_auth_login: unknown state");
	}

abort:
	smtp_reply(s, "501 %s %s: Syntax error",
	    esc_code(ESC_STATUS_PERMFAIL, ESC_SYNTAX_ERROR),
	    esc_description(ESC_SYNTAX_ERROR));
	smtp_enter_state(s, STATE_HELO);
}