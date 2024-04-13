static int fsl_lpspi_bytes_per_word(const int bpw)
{
	return DIV_ROUND_UP(bpw, BITS_PER_BYTE);
}