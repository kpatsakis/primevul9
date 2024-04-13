ptp_unpack_Nikon_EC (PTPParams *params, unsigned char* data, unsigned int len, PTPContainer **ec, unsigned int *cnt)
{
	unsigned int i;

	*ec = NULL;
	if (data == NULL)
		return;
	if (len < PTP_nikon_ec_Code)
		return;
	*cnt = dtoh16a(&data[PTP_nikon_ec_Length]);
	if (*cnt > (len-PTP_nikon_ec_Code)/PTP_nikon_ec_Size) { /* broken cnt? */
		*cnt = 0;
		return;
	}
	if (!*cnt)
		return;

	*ec = malloc(sizeof(PTPContainer)*(*cnt));
	
	for (i=0;i<*cnt;i++) {
		memset(&(*ec)[i],0,sizeof(PTPContainer));
		(*ec)[i].Code	= dtoh16a(&data[PTP_nikon_ec_Code+PTP_nikon_ec_Size*i]);
		(*ec)[i].Param1	= dtoh32a(&data[PTP_nikon_ec_Param1+PTP_nikon_ec_Size*i]);
		(*ec)[i].Nparam	= 1;
	}
}