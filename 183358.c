ptp_unpack_OH (PTPParams *params, unsigned char* data, PTPObjectHandles *oh, unsigned int len)
{
	if (len) {
		oh->n = ptp_unpack_uint32_t_array(params, data, PTP_oh, len, &oh->Handler);
	} else {
		oh->n = 0;
		oh->Handler = NULL;
	} 
}