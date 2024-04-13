server_send_params(gnutls_session_t session, gnutls_buffer_t extdata)
{
	int ret;

	if (!(session->internals.hsk_flags & HSK_PSK_SELECTED))
		return 0;

	ret = _gnutls_buffer_append_prefix(extdata, 16,
					   session->key.binders[0].idx);
	if (ret < 0)
		return gnutls_assert_val(ret);

	return 2;
}