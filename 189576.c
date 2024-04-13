process_keytag(ns_client_t *client, isc_buffer_t *buf, size_t optlen) {

	if (optlen == 0 || (optlen % 2) != 0) {
		isc_buffer_forward(buf, (unsigned int)optlen);
		return (DNS_R_OPTERR);
	}

	/* Silently drop additional keytag options. */
	if (client->keytag != NULL) {
		isc_buffer_forward(buf, (unsigned int)optlen);
		return (ISC_R_SUCCESS);
	}

	client->keytag = isc_mem_get(client->mctx, optlen);
	if (client->keytag != NULL) {
		client->keytag_len = (uint16_t)optlen;
		memmove(client->keytag, isc_buffer_current(buf), optlen);
	}
	isc_buffer_forward(buf, (unsigned int)optlen);
	return (ISC_R_SUCCESS);
}