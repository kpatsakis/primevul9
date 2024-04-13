htod64ap (PTPParams *params, unsigned char *a, uint64_t val)
{
	if (params->byteorder==PTP_DL_LE)
		htole64a(a,val);
	else
		htobe64a(a,val);
}