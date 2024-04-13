int gnutls_init(gnutls_session_t * session, unsigned int flags)
{
	int ret;
	record_parameters_st *epoch;
	
	FAIL_IF_LIB_ERROR;

	*session = gnutls_calloc(1, sizeof(struct gnutls_session_int));
	if (*session == NULL)
		return GNUTLS_E_MEMORY_ERROR;

	ret = _gnutls_epoch_alloc(*session, 0, &epoch);
	if (ret < 0) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	/* Set all NULL algos on epoch 0 */
	_gnutls_epoch_set_null_algos(*session, epoch);

	(*session)->security_parameters.epoch_next = 1;

	(*session)->security_parameters.entity =
	    (flags & GNUTLS_SERVER ? GNUTLS_SERVER : GNUTLS_CLIENT);

	/* the default certificate type for TLS */
	(*session)->security_parameters.cert_type = DEFAULT_CERT_TYPE;

	/* Initialize buffers */
	_gnutls_buffer_init(&(*session)->internals.handshake_hash_buffer);
	_gnutls_buffer_init(&(*session)->internals.hb_remote_data);
	_gnutls_buffer_init(&(*session)->internals.hb_local_data);
	_gnutls_buffer_init(&(*session)->internals.record_presend_buffer);

	_mbuffer_head_init(&(*session)->internals.record_buffer);
	_mbuffer_head_init(&(*session)->internals.record_send_buffer);
	_mbuffer_head_init(&(*session)->internals.record_recv_buffer);

	_mbuffer_head_init(&(*session)->internals.handshake_send_buffer);
	_gnutls_handshake_recv_buffer_init(*session);

	(*session)->internals.expire_time = DEFAULT_EXPIRE_TIME;	/* one hour default */

	gnutls_handshake_set_max_packet_length((*session),
					       MAX_HANDSHAKE_PACKET_SIZE);

	/* set the socket pointers to -1;
	 */
	(*session)->internals.transport_recv_ptr =
	    (gnutls_transport_ptr_t) - 1;
	(*session)->internals.transport_send_ptr =
	    (gnutls_transport_ptr_t) - 1;

	/* set the default maximum record size for TLS
	 */
	(*session)->security_parameters.max_record_recv_size =
	    DEFAULT_MAX_RECORD_SIZE;
	(*session)->security_parameters.max_record_send_size =
	    DEFAULT_MAX_RECORD_SIZE;

	/* everything else not initialized here is initialized
	 * as NULL or 0. This is why calloc is used.
	 */

	_gnutls_handshake_internal_state_init(*session);

	(*session)->internals.extensions_sent_size = 0;

	/* emulate old gnutls behavior for old applications that do not use the priority_*
	 * functions.
	 */
	(*session)->internals.priorities.sr = SR_PARTIAL;

#ifdef HAVE_WRITEV
#ifdef MSG_NOSIGNAL
	if (flags & GNUTLS_NO_SIGNAL)
		gnutls_transport_set_vec_push_function(*session, system_writev_nosignal);
	else
#endif
		gnutls_transport_set_vec_push_function(*session, system_writev);
#else
	gnutls_transport_set_push_function(*session, system_write);
#endif
	(*session)->internals.pull_timeout_func = gnutls_system_recv_timeout;
	(*session)->internals.pull_func = system_read;
	(*session)->internals.errno_func = system_errno;

	/* heartbeat timeouts */
	(*session)->internals.hb_retrans_timeout_ms = 1000;
	(*session)->internals.hb_total_timeout_ms = 60000;

	if (flags & GNUTLS_DATAGRAM) {
		(*session)->internals.dtls.mtu = DTLS_DEFAULT_MTU;
		(*session)->internals.transport = GNUTLS_DGRAM;

		gnutls_dtls_set_timeouts(*session, DTLS_RETRANS_TIMEOUT, 60000);
	} else {
		(*session)->internals.transport = GNUTLS_STREAM;
	}

	/* Enable useful extensions */
	if ((flags & GNUTLS_CLIENT) && !(flags & GNUTLS_NO_EXTENSIONS)) {
#ifdef ENABLE_SESSION_TICKETS
		gnutls_session_ticket_enable_client(*session);
#endif
#ifdef ENABLE_OCSP
		gnutls_ocsp_status_request_enable_client(*session, NULL, 0,
							 NULL);
#endif
	}

	(*session)->internals.flags = flags;

	return 0;
}