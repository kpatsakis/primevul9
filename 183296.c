int csr_read_pskey_uint32(int dd, uint16_t seqnum, uint16_t pskey, uint16_t stores, uint32_t *value)
{
	uint8_t array[4] = { 0x00, 0x00, 0x00, 0x00 };
	int err;

	err = csr_read_pskey_complex(dd, seqnum, pskey, stores, array, 4);

	*value = ((array[0] + (array[1] << 8)) << 16) +
						(array[2] + (array[3] << 8));

	return err;
}