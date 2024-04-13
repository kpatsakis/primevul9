smtp_command(struct smtp_session *s, char *line)
{
	char			       *args;
	int				cmd, i;

	log_trace(TRACE_SMTP, "smtp: %p: <<< %s", s, line);

	/*
	 * These states are special.
	 */
	if (s->state == STATE_AUTH_INIT) {
		smtp_report_protocol_client(s, "********");
		smtp_rfc4954_auth_plain(s, line);
		return;
	}
	if (s->state == STATE_AUTH_USERNAME || s->state == STATE_AUTH_PASSWORD) {
		smtp_report_protocol_client(s, "********");
		smtp_rfc4954_auth_login(s, line);
		return;
	}

	if (s->state == STATE_HELO && strncasecmp(line, "AUTH PLAIN ", 11) == 0)
		smtp_report_protocol_client(s, "AUTH PLAIN ********");
	else
		smtp_report_protocol_client(s, line);


	/*
	 * Unlike other commands, "mail from" and "rcpt to" contain a
	 * space in the command name.
	 */
	if (strncasecmp("mail from:", line, 10) == 0 ||
	    strncasecmp("rcpt to:", line, 8) == 0)
		args = strchr(line, ':');
	else
		args = strchr(line, ' ');

	if (args) {
		*args++ = '\0';
		while (isspace((unsigned char)*args))
			args++;
	}

	cmd = -1;
	for (i = 0; commands[i].code != -1; i++)
		if (!strcasecmp(line, commands[i].cmd)) {
			cmd = commands[i].code;
			break;
		}

	s->last_cmd = cmd;
	switch (cmd) {
	/*
	 * INIT
	 */
	case CMD_HELO:
		if (!smtp_check_helo(s, args))
			break;
		smtp_filter_phase(FILTER_HELO, s, args);
		break;

	case CMD_EHLO:
		if (!smtp_check_ehlo(s, args))
			break;
		smtp_filter_phase(FILTER_EHLO, s, args);
		break;

	/*
	 * SETUP
	 */
	case CMD_STARTTLS:
		if (!smtp_check_starttls(s, args))
			break;

		smtp_filter_phase(FILTER_STARTTLS, s, NULL);
		break;

	case CMD_AUTH:
		if (!smtp_check_auth(s, args))
			break;
		smtp_filter_phase(FILTER_AUTH, s, args);
		break;

	case CMD_MAIL_FROM:
		if (!smtp_check_mail_from(s, args))
			break;
		smtp_filter_phase(FILTER_MAIL_FROM, s, args);
		break;

	/*
	 * TRANSACTION
	 */
	case CMD_RCPT_TO:
		if (!smtp_check_rcpt_to(s, args))
			break;
		smtp_filter_phase(FILTER_RCPT_TO, s, args);
		break;

	case CMD_RSET:
		if (!smtp_check_rset(s, args))
			break;
		smtp_filter_phase(FILTER_RSET, s, NULL);
		break;

	case CMD_DATA:
		if (!smtp_check_data(s, args))
			break;
		smtp_filter_phase(FILTER_DATA, s, NULL);
		break;

	/*
	 * ANY
	 */
	case CMD_QUIT:
		if (!smtp_check_noparam(s, args))
			break;		
		smtp_filter_phase(FILTER_QUIT, s, NULL);
		break;

	case CMD_NOOP:
		if (!smtp_check_noparam(s, args))
			break;		
		smtp_filter_phase(FILTER_NOOP, s, NULL);
		break;

	case CMD_HELP:
		if (!smtp_check_noparam(s, args))
			break;		
		smtp_proceed_help(s, NULL);
		break;

	case CMD_WIZ:
		if (!smtp_check_noparam(s, args))
			break;		
		smtp_proceed_wiz(s, NULL);
		break;

	default:
		smtp_reply(s, "500 %s %s: Command unrecognized",
			    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
			    esc_description(ESC_INVALID_COMMAND));
		break;
	}
}