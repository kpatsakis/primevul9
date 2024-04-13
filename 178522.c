void session_init(Session *session, const void *prefs_account, gboolean is_smtp)
{
	session->type = SESSION_UNKNOWN;
	session->sock = NULL;
	session->server = NULL;
	session->port = 0;
#ifdef USE_GNUTLS
	session->ssl_type = SSL_NONE;
	session->use_tls_sni = TRUE;
#endif
	session->nonblocking = TRUE;
	session->state = SESSION_READY;
	session->last_access_time = time(NULL);

	session->tv_prev = g_date_time_new_now_local();

	session->conn_id = 0;

	session->io_tag = 0;

	session->read_buf_p = session->read_buf;
	session->read_buf_len = 0;

	session->read_msg_buf = g_string_sized_new(1024);
	session->read_data_buf = g_byte_array_new();

	session->write_buf = NULL;
	session->write_buf_p = NULL;
	session->write_buf_len = 0;

	session->write_data = NULL;
	session->write_data_p = NULL;
	session->write_data_len = 0;

	session->timeout_tag = 0;
	session->timeout_interval = 0;

	session->data = NULL;
	session->account = prefs_account;
	session->is_smtp = is_smtp;

	session->ping_tag = -1;

	session->proxy_info = NULL;
}