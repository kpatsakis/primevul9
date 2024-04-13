dtoh16p (PTPParams *params, uint16_t var)
{
	return ((params->byteorder==PTP_DL_LE)?le16toh(var):be16toh(var));
}