dtoh64ap (PTPParams *params, const unsigned char *a)
{
	return ((params->byteorder==PTP_DL_LE)?le64atoh(a):be64atoh(a));
}