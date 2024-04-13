dtoh16ap (PTPParams *params, const unsigned char *a)
{
	return ((params->byteorder==PTP_DL_LE)?le16atoh(a):be16atoh(a));
}