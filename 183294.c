int csr_read_pskey_uint16(int dd, uint16_t seqnum, uint16_t pskey, uint16_t stores, uint16_t *value)
{
	uint8_t array[2] = { 0x00, 0x00 };
	int err;

	err = csr_read_pskey_complex(dd, seqnum, pskey, stores, array, 2);

	*value = array[0] + (array[1] << 8);

	return err;
}