void smbd_setup_sig_term_handler(void)
{
	struct tevent_signal *se;

	se = tevent_add_signal(smbd_event_context(),
			       smbd_event_context(),
			       SIGTERM, 0,
			       smbd_sig_term_handler,
			       NULL);
	if (!se) {
		exit_server("failed to setup SIGTERM handler");
	}
}