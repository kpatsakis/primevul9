compute_psk_binder(gnutls_session_t session,
		   const mac_entry_st *prf, unsigned binders_length,
		   int exts_length, int ext_offset,
		   const gnutls_datum_t *psk, const gnutls_datum_t *client_hello,
		   bool resuming, void *out)
{
	int ret;
	unsigned client_hello_pos, extensions_len_pos;
	gnutls_buffer_st handshake_buf;
	uint8_t binder_key[MAX_HASH_SIZE];

	_gnutls_buffer_init(&handshake_buf);

	if (session->security_parameters.entity == GNUTLS_CLIENT) {
		if (session->internals.hsk_flags & HSK_HRR_RECEIVED) {
			ret = gnutls_buffer_append_data(&handshake_buf,
							(const void *) session->internals.handshake_hash_buffer.data,
							session->internals.handshake_hash_buffer.length);
			if (ret < 0) {
				gnutls_assert();
				goto error;
			}
		}

		client_hello_pos = handshake_buf.length;
		ret = gnutls_buffer_append_data(&handshake_buf, client_hello->data,
						client_hello->size);
		if (ret < 0) {
			gnutls_assert();
			goto error;
		}

		/* This is a ClientHello message */
		handshake_buf.data[client_hello_pos] = GNUTLS_HANDSHAKE_CLIENT_HELLO;

		/* At this point we have not yet added the binders to the ClientHello,
		 * but we have to overwrite the size field, pretending as if binders
		 * of the correct length were present.
		 */
		_gnutls_write_uint24(handshake_buf.length - client_hello_pos + binders_length - 2, &handshake_buf.data[client_hello_pos + 1]);
		_gnutls_write_uint16(handshake_buf.length - client_hello_pos + binders_length - ext_offset,
				     &handshake_buf.data[client_hello_pos + ext_offset]);
		extensions_len_pos = handshake_buf.length - client_hello_pos - exts_length - 2;
		_gnutls_write_uint16(exts_length + binders_length + 2,
				     &handshake_buf.data[client_hello_pos + extensions_len_pos]);
	} else {
		if (session->internals.hsk_flags & HSK_HRR_SENT) {
			if (unlikely(session->internals.handshake_hash_buffer.length <= client_hello->size)) {
				ret = gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);
				goto error;
			}

			ret = gnutls_buffer_append_data(&handshake_buf,
							session->internals.handshake_hash_buffer.data,
							session->internals.handshake_hash_buffer.length - client_hello->size);
			if (ret < 0) {
				gnutls_assert();
				goto error;
			}
		}

		if (unlikely(client_hello->size <= binders_length)) {
			ret = gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);
			goto error;
		}

		ret = gnutls_buffer_append_data(&handshake_buf,
						(const void *) client_hello->data,
						client_hello->size - binders_length);
		if (ret < 0) {
			gnutls_assert();
			goto error;
		}
	}

	ret = compute_binder_key(prf,
				 psk->data, psk->size, resuming,
				 binder_key);
	if (ret < 0) {
		gnutls_assert();
		goto error;
	}

	ret = _gnutls13_compute_finished(prf, binder_key,
					 &handshake_buf,
					 out);
	if (ret < 0) {
		gnutls_assert();
		goto error;
	}

	ret = 0;
error:
	_gnutls_buffer_clear(&handshake_buf);
	return ret;
}