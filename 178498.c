void session_destroy(Session *session)
{
	cm_return_if_fail(session != NULL);
	cm_return_if_fail(session->destroy != NULL);

	session_register_ping(session, NULL);

	session_close(session);
	session->destroy(session);
	g_free(session->server);
	g_string_free(session->read_msg_buf, TRUE);
	g_byte_array_free(session->read_data_buf, TRUE);
	g_free(session->read_data_terminator);
	g_free(session->write_buf);
#ifdef USE_GNUTLS
	g_free(session->gnutls_priority);
#endif
	g_date_time_unref(session->tv_prev);
	debug_print("session (%p): destroyed\n", session);

	g_free(session);
}