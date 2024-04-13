compute_psk_from_ticket(const tls13_ticket_st *ticket, gnutls_datum_t *key)
{
	int ret;

	if (unlikely(ticket->prf == NULL || ticket->prf->output_size == 0))
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

	key->data = gnutls_malloc(ticket->prf->output_size);
	if (!key->data) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}
	key->size = ticket->prf->output_size;

	ret = _tls13_expand_secret2(ticket->prf,
				    RESUMPTION_LABEL, sizeof(RESUMPTION_LABEL)-1,
				    ticket->nonce, ticket->nonce_size,
				    ticket->resumption_master_secret,
				    key->size,
				    key->data);
	if (ret < 0)
		gnutls_assert();

	return ret;
}