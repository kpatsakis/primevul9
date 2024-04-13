ptp_unpack_SIDs (PTPParams *params, unsigned char* data, PTPStorageIDs *sids, unsigned int len)
{
	sids->n = 0;
	sids->Storage = NULL;

	if (!data || !len)
		return;

	sids->n = ptp_unpack_uint32_t_array(params, data, PTP_sids, len, &sids->Storage);
}