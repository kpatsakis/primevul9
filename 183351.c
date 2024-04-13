dtoh64p (PTPParams *params, uint64_t var)
{
	return ((params->byteorder==PTP_DL_LE)?le64toh(var):be64toh(var));
}