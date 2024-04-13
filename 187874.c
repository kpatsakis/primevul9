static int server_recv_params(gnutls_session_t session,
			      const unsigned char *data, size_t len,
			      const gnutls_psk_server_credentials_t pskcred)
{
	int ret;
	const mac_entry_st *prf;
	gnutls_datum_t full_client_hello;
	uint8_t binder_value[MAX_HASH_SIZE];
	uint16_t psk_index, i;
	gnutls_datum_t binder_recvd = { NULL, 0 };
	gnutls_datum_t key = {NULL, 0};
	psk_ext_parser_st psk_parser;
	psk_ext_iter_st psk_iter;
	struct psk_st psk;
	psk_auth_info_t info;
	tls13_ticket_st ticket_data;
	/* These values should be set properly when session ticket is accepted. */
	uint32_t ticket_age = UINT32_MAX;
	struct timespec ticket_creation_time = { 0, 0 };
	bool resuming;

	ret = _gnutls13_psk_ext_parser_init(&psk_parser, data, len);
	if (ret < 0) {
		/* No PSKs advertised by client */
		if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
			return 0;
		return gnutls_assert_val(ret);
	}

	_gnutls13_psk_ext_iter_init(&psk_iter, &psk_parser);
	for (psk_index = 0; ; psk_index++) {
		ret = _gnutls13_psk_ext_iter_next_identity(&psk_iter, &psk);
		if (ret < 0) {
			/* We couldn't find any usable PSK */
			if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
				return 0;
			return gnutls_assert_val(ret);
		}

		/* This will unpack the session ticket if it is well
		 * formed and has the expected name */
		if (!(session->internals.flags & GNUTLS_NO_TICKETS) &&
		    _gnutls13_unpack_session_ticket(session, &psk.identity, &ticket_data) == 0) {
			prf = ticket_data.prf;

			session->internals.resumption_requested = 1;

			/* Check whether ticket is stale or not */
			ticket_age = psk.ob_ticket_age - ticket_data.age_add;
			if (ticket_age / 1000 > ticket_data.lifetime) {
				gnutls_assert();
				tls13_ticket_deinit(&ticket_data);
				continue;
			}

			ret = compute_psk_from_ticket(&ticket_data, &key);
			if (ret < 0) {
				gnutls_assert();
				tls13_ticket_deinit(&ticket_data);
				continue;
			}

			memcpy(&ticket_creation_time,
			       &ticket_data.creation_time,
			       sizeof(struct timespec));

			tls13_ticket_deinit(&ticket_data);

			resuming = 1;
			break;
		} else if (pskcred &&
			   psk.ob_ticket_age == 0 &&
			   psk.identity.size > 0 && psk.identity.size <= MAX_USERNAME_SIZE) {
			prf = pskcred->binder_algo;

			/* this fails only on configuration errors; as such we always
			 * return its error code in that case */
			ret = _gnutls_psk_pwd_find_entry(session, (char *) psk.identity.data, psk.identity.size, &key);
			if (ret < 0)
				return gnutls_assert_val(ret);

			resuming = 0;
			break;
		}
	}

	_gnutls13_psk_ext_iter_init(&psk_iter, &psk_parser);
	for (i = 0; i <= psk_index; i++) {
		ret = _gnutls13_psk_ext_iter_next_binder(&psk_iter, &binder_recvd);
		if (ret < 0) {
			gnutls_assert();
			/* We couldn't extract binder */
			if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
				ret = GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER;
			goto fail;
		}
	}

	/* Get full ClientHello */
	if (!_gnutls_ext_get_full_client_hello(session, &full_client_hello)) {
		ret = GNUTLS_E_INTERNAL_ERROR;
		gnutls_assert();
		goto fail;
	}

	/* Compute the binder value for this PSK */
	ret = compute_psk_binder(session, prf, psk_parser.binders_len+2, 0, 0,
				 &key, &full_client_hello, resuming,
				 binder_value);
	if (ret < 0) {
		gnutls_assert();
		goto fail;
	}

	if (_gnutls_mac_get_algo_len(prf) != binder_recvd.size ||
	    gnutls_memcmp(binder_value, binder_recvd.data, binder_recvd.size)) {
		gnutls_assert();
		ret = GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER;
		goto fail;
	}

	if (session->internals.hsk_flags & HSK_PSK_KE_MODE_DHE_PSK)
		_gnutls_handshake_log("EXT[%p]: selected DHE-PSK mode\n", session);
	else {
		reset_cand_groups(session);
		_gnutls_handshake_log("EXT[%p]: selected PSK mode\n", session);
	}

	/* save the username in psk_auth_info to make it available
	 * using gnutls_psk_server_get_username() */
	if (!resuming) {
		assert(psk.identity.size < sizeof(info->username));

		ret = _gnutls_auth_info_init(session, GNUTLS_CRD_PSK, sizeof(psk_auth_info_st), 1);
		if (ret < 0) {
			gnutls_assert();
			goto fail;
		}

		info = _gnutls_get_auth_info(session, GNUTLS_CRD_PSK);
		assert(info != NULL);

		_gnutls_copy_psk_username(info, &psk.identity);
		_gnutls_handshake_log("EXT[%p]: selected PSK identity: %s (%d)\n", session, info->username, psk_index);
	} else {
		if (session->internals.hsk_flags & HSK_EARLY_DATA_ACCEPTED) {
			if (session->internals.anti_replay) {
				ret = _gnutls_anti_replay_check(session->internals.anti_replay,
								ticket_age,
								&ticket_creation_time,
								&binder_recvd);
				if (ret < 0) {
					session->internals.hsk_flags &= ~HSK_EARLY_DATA_ACCEPTED;
					_gnutls_handshake_log("EXT[%p]: replay detected; rejecting early data\n",
						      session);
				}
			} else {
				_gnutls_handshake_log("EXT[%p]: anti-replay is not enabled; rejecting early data\n",
						      session);
				session->internals.hsk_flags &= ~HSK_EARLY_DATA_ACCEPTED;
			}
		}

		session->internals.resumed = true;
		_gnutls_handshake_log("EXT[%p]: selected resumption PSK identity (%d)\n", session, psk_index);
	}

	session->internals.hsk_flags |= HSK_PSK_SELECTED;

	/* Reference the selected pre-shared key */
	session->key.binders[0].psk.data = key.data;
	session->key.binders[0].psk.size = key.size;

	session->key.binders[0].idx = psk_index;
	session->key.binders[0].prf = prf;
	session->key.binders[0].resumption = resuming;

	ret = _gnutls_generate_early_secrets_for_psk(session);
	if (ret < 0) {
		gnutls_assert();
		goto fail;
	}

	return 0;

 fail:
	gnutls_free(key.data);
	return ret;
}