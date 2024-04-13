ptp_unpack_DI (PTPParams *params, unsigned char* data, PTPDeviceInfo *di, unsigned int datalen)
{
	uint8_t len;
	unsigned int totallen;

	if (!data) return 0;
	if (datalen < 12) return 0;
	memset (di, 0, sizeof(*di));
	di->StandardVersion = dtoh16a(&data[PTP_di_StandardVersion]);
	di->VendorExtensionID =
		dtoh32a(&data[PTP_di_VendorExtensionID]);
	di->VendorExtensionVersion =
		dtoh16a(&data[PTP_di_VendorExtensionVersion]);
	di->VendorExtensionDesc =
		ptp_unpack_string(params, data,
		PTP_di_VendorExtensionDesc,
		datalen,
		&len);
	totallen=len*2+1;
	if (datalen <= totallen) return 0;
	di->FunctionalMode =
		dtoh16a(&data[PTP_di_FunctionalMode+totallen]);
	di->OperationsSupported_len = ptp_unpack_uint16_t_array(params, data,
		PTP_di_OperationsSupported+totallen,
		datalen,
		&di->OperationsSupported);
	totallen=totallen+di->OperationsSupported_len*sizeof(uint16_t)+sizeof(uint32_t);
	if (datalen <= totallen+PTP_di_OperationsSupported) return 0;
	di->EventsSupported_len = ptp_unpack_uint16_t_array(params, data,
		PTP_di_OperationsSupported+totallen,
		datalen,
		&di->EventsSupported);
	totallen=totallen+di->EventsSupported_len*sizeof(uint16_t)+sizeof(uint32_t);
	if (datalen <= totallen+PTP_di_OperationsSupported) return 0;
	di->DevicePropertiesSupported_len =
		ptp_unpack_uint16_t_array(params, data,
		PTP_di_OperationsSupported+totallen,
		datalen,
		&di->DevicePropertiesSupported);
	totallen=totallen+di->DevicePropertiesSupported_len*sizeof(uint16_t)+sizeof(uint32_t);
	if (datalen <= totallen+PTP_di_OperationsSupported) return 0;
	di->CaptureFormats_len = ptp_unpack_uint16_t_array(params, data,
		PTP_di_OperationsSupported+totallen,
		datalen,
		&di->CaptureFormats);
	totallen=totallen+di->CaptureFormats_len*sizeof(uint16_t)+sizeof(uint32_t);
	if (datalen <= totallen+PTP_di_OperationsSupported) return 0;
	di->ImageFormats_len = ptp_unpack_uint16_t_array(params, data,
		PTP_di_OperationsSupported+totallen,
		datalen,
		&di->ImageFormats);
	totallen=totallen+di->ImageFormats_len*sizeof(uint16_t)+sizeof(uint32_t);
	if (datalen <= totallen+PTP_di_OperationsSupported) return 0;
	di->Manufacturer = ptp_unpack_string(params, data,
		PTP_di_OperationsSupported+totallen,
		datalen,
		&len);
	totallen+=len*2+1;
	/* be more relaxed ... as these are optional its ok if they are not here */
	if (datalen <= totallen+PTP_di_OperationsSupported) return 1;
	di->Model = ptp_unpack_string(params, data,
		PTP_di_OperationsSupported+totallen,
		datalen,
		&len);
	totallen+=len*2+1;
	/* be more relaxed ... as these are optional its ok if they are not here */
	if (datalen <= totallen+PTP_di_OperationsSupported) return 1;
	di->DeviceVersion = ptp_unpack_string(params, data,
		PTP_di_OperationsSupported+totallen,
		datalen,
		&len);
	totallen+=len*2+1;
	/* be more relaxed ... as these are optional its ok if they are not here */
	if (datalen <= totallen+PTP_di_OperationsSupported) return 1;
	di->SerialNumber = ptp_unpack_string(params, data,
		PTP_di_OperationsSupported+totallen,
		datalen,
		&len);
	return 1;
}