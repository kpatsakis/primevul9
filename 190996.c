void smbd_setup_sig_hup_handler(void)
{
	struct tevent_signal *se;

	se = tevent_add_signal(smbd_event_context(),
			       smbd_event_context(),
			       SIGHUP, 0,
			       smbd_sig_hup_handler,
			       NULL);
	if (!se) {
		exit_server("failed to setup SIGHUP handler");
	}
}