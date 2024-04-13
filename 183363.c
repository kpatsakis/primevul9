dtoh32p (PTPParams *params, uint32_t var)
{
	return ((params->byteorder==PTP_DL_LE)?le32toh(var):be32toh(var));
}