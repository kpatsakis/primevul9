smtp_rfc4954_auth_plain(struct smtp_session *s, char *arg)
{
	char		 buf[1024], *user, *pass;
	int		 len;

	switch (s->state) {
	case STATE_HELO:
		if (arg == NULL) {
			smtp_enter_state(s, STATE_AUTH_INIT);
			smtp_reply(s, "334 ");
			return;
		}
		smtp_enter_state(s, STATE_AUTH_INIT);
		/* FALLTHROUGH */

	case STATE_AUTH_INIT:
		/* String is not NUL terminated, leave room. */
		if ((len = base64_decode(arg, (unsigned char *)buf,
			    sizeof(buf) - 1)) == -1)
			goto abort;
		/* buf is a byte string, NUL terminate. */
		buf[len] = '\0';

		/*
		 * Skip "foo" in "foo\0user\0pass", if present.
		 */
		user = memchr(buf, '\0', len);
		if (user == NULL || user >= buf + len - 2)
			goto abort;
		user++; /* skip NUL */
		if (strlcpy(s->username, user, sizeof(s->username))
		    >= sizeof(s->username))
			goto abort;

		pass = memchr(user, '\0', len - (user - buf));
		if (pass == NULL || pass >= buf + len - 2)
			goto abort;
		pass++; /* skip NUL */

		m_create(p_lka,  IMSG_SMTP_AUTHENTICATE, 0, 0, -1);
		m_add_id(p_lka, s->id);
		m_add_string(p_lka, s->listener->authtable);
		m_add_string(p_lka, user);
		m_add_string(p_lka, pass);
		m_close(p_lka);
		tree_xset(&wait_parent_auth, s->id, s);
		return;

	default:
		fatal("smtp_rfc4954_auth_plain: unknown state");
	}

abort:
	smtp_reply(s, "501 %s %s: Syntax error",
	    esc_code(ESC_STATUS_PERMFAIL, ESC_SYNTAX_ERROR),
	    esc_description(ESC_SYNTAX_ERROR));
	smtp_enter_state(s, STATE_HELO);
}