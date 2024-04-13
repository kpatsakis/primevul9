sc_remote_apdu_allocate(struct sc_remote_data *rdata,
		struct sc_remote_apdu **new_rapdu)
{
	struct sc_remote_apdu *rapdu = NULL, *rr;

	if (!rdata)
		return SC_ERROR_INVALID_ARGUMENTS;

	rapdu = calloc(1, sizeof(struct sc_remote_apdu));
	if (rapdu == NULL)
		return SC_ERROR_OUT_OF_MEMORY;

	rapdu->apdu.data = &rapdu->sbuf[0];
	rapdu->apdu.resp = &rapdu->rbuf[0];
	rapdu->apdu.resplen = sizeof(rapdu->rbuf);

	if (new_rapdu)
		*new_rapdu = rapdu;

	if (rdata->data == NULL)   {
		rdata->data = rapdu;
		rdata->length = 1;
		return SC_SUCCESS;
	}

	for (rr = rdata->data; rr->next; rr = rr->next)
		;
	rr->next = rapdu;
	rdata->length++;

	return SC_SUCCESS;
}