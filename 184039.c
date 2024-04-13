smtp_filter_phase(enum filter_phase phase, struct smtp_session *s, const char *param)
{
	uint8_t i;

	s->filter_phase = phase;
	s->filter_param = param;

	if (SESSION_FILTERED(s)) {
		smtp_query_filters(phase, s, param ? param : "");
		return;
	}

	if (s->filter_phase == FILTER_CONNECT) {
		smtp_proceed_connected(s);
		return;
	}

	for (i = 0; i < nitems(commands); ++i)
		if (commands[i].filter_phase == s->filter_phase) {
			commands[i].proceed(s, param);
			break;
		}
}