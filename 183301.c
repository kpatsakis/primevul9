int csr_write_pskey_uint16(int dd, uint16_t seqnum, uint16_t pskey, uint16_t stores, uint16_t value)
{
	uint8_t array[2] = { value & 0xff, value >> 8 };

	return csr_write_pskey_complex(dd, seqnum, pskey, stores, array, 2);
}