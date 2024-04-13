ptp_unpack_EOS_DI (PTPParams *params, unsigned char* data, PTPCanonEOSDeviceInfo *di, unsigned int datalen)
{
	unsigned int totallen = 4;

	memset (di,0, sizeof(*di));
	if (datalen < 8) return 0;

	/* uint32_t struct len - ignore */
	di->EventsSupported_len = ptp_unpack_uint32_t_array(params, data,
		totallen, datalen, &di->EventsSupported);
	if (!di->EventsSupported) return 0;
	totallen += di->EventsSupported_len*sizeof(uint32_t)+4;
	if (totallen >= datalen) return 0;

	di->DevicePropertiesSupported_len = ptp_unpack_uint32_t_array(params, data,
		totallen, datalen, &di->DevicePropertiesSupported);
	if (!di->DevicePropertiesSupported) return 0;
	totallen += di->DevicePropertiesSupported_len*sizeof(uint32_t)+4;
	if (totallen >= datalen) return 0;

	di->unk_len = ptp_unpack_uint32_t_array(params, data,
		totallen, datalen, &di->unk);
	if (!di->unk) return 0;
	totallen += di->unk_len*sizeof(uint32_t)+4;
	return 1;
}