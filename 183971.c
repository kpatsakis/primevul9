smtp_connected(struct smtp_session *s)
{
	smtp_enter_state(s, STATE_CONNECTED);

	log_info("%016"PRIx64" smtp connected address=%s host=%s",
	    s->id, ss_to_text(&s->ss), s->rdns);

	smtp_filter_begin(s);

	smtp_report_link_connect(s, s->rdns, s->fcrdns, &s->ss,
	    &s->listener->ss);

	smtp_filter_phase(FILTER_CONNECT, s, ss_to_text(&s->ss));
}