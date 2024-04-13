gnutls_record_set_state(gnutls_session_t session,
			unsigned read,
			unsigned char seq_number[8])
{
	record_parameters_st *record_params;
	record_state_st *record_state;
	int epoch, ret;

	if (read)
		epoch = EPOCH_READ_CURRENT;
	else
		epoch = EPOCH_WRITE_CURRENT;

	ret = _gnutls_epoch_get(session, epoch, &record_params);
	if (ret < 0)
		return gnutls_assert_val(ret);

	if (!record_params->initialized)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	if (read)
		record_state = &record_params->read;
	else
		record_state = &record_params->write;

	memcpy(UINT64DATA(record_state->sequence_number), seq_number, 8);

	if (IS_DTLS(session)) {
		_dtls_reset_window(record_params);
	}

	return 0;
}