void sc_remote_data_init(struct sc_remote_data *rdata)
{
	if (!rdata)
		return;
	memset(rdata, 0, sizeof(struct sc_remote_data));

	rdata->alloc = sc_remote_apdu_allocate;
	rdata->free = sc_remote_apdu_free;
}