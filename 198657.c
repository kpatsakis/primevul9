sc_remote_apdu_free (struct sc_remote_data *rdata)
{
	struct sc_remote_apdu *rapdu = NULL;

	if (!rdata)
		return;

	rapdu = rdata->data;
	while(rapdu)   {
		struct sc_remote_apdu *rr = rapdu->next;

		free(rapdu);
		rapdu = rr;
	}
}