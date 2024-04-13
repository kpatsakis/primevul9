dtoh32ap (PTPParams *params, const unsigned char *a)
{
	return ((params->byteorder==PTP_DL_LE)?le32atoh(a):be32atoh(a));
}