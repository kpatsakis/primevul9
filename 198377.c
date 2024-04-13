gnutls_record_get_state(gnutls_session_t session,
			unsigned read,
			gnutls_datum_t *mac_key,
			gnutls_datum_t *IV,
			gnutls_datum_t *cipher_key,
			unsigned char seq_number[8])
{
	record_parameters_st *record_params;
	record_state_st *record_state;
	unsigned int epoch;
	int ret;

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

	if (mac_key)
		memcpy(mac_key, &record_state->mac_secret, sizeof(gnutls_datum_t));
	if (IV)
		memcpy(IV, &record_state->IV, sizeof(gnutls_datum_t));
	if (cipher_key)
		memcpy(cipher_key, &record_state->key, sizeof(gnutls_datum_t));
	if (seq_number)
		memcpy(seq_number, UINT64DATA(record_state->sequence_number), 8);
	return 0;
}