ptp_unpack_EC (PTPParams *params, unsigned char* data, PTPContainer *ec, unsigned int len)
{
	unsigned int	length;
	int	type;

	if (data==NULL)
		return;
	memset(ec,0,sizeof(*ec));

	length=dtoh32a(&data[PTP_ec_Length]);
	if (length > len) {
		ptp_debug (params, "length %d in container, but data only %d bytes?!", length, len);
		return;
	}
	type = dtoh16a(&data[PTP_ec_Type]);

	ec->Code=dtoh16a(&data[PTP_ec_Code]);
	ec->Transaction_ID=dtoh32a(&data[PTP_ec_TransId]);

	if (type!=PTP_USB_CONTAINER_EVENT) {
		ptp_debug (params, "Unknown canon event type %d (code=%x,tid=%x), please report!",type,ec->Code,ec->Transaction_ID);
		return;
	}
	if (length>=(PTP_ec_Param1+4)) {
		ec->Param1=dtoh32a(&data[PTP_ec_Param1]);
		ec->Nparam=1;
	}
	if (length>=(PTP_ec_Param2+4)) {
		ec->Param2=dtoh32a(&data[PTP_ec_Param2]);
		ec->Nparam=2;
	}
	if (length>=(PTP_ec_Param3+4)) {
		ec->Param3=dtoh32a(&data[PTP_ec_Param3]);
		ec->Nparam=3;
	}
}