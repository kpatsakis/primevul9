int csr_write_pskey_uint32(int dd, uint16_t seqnum, uint16_t pskey, uint16_t stores, uint32_t value)
{
	uint8_t array[4] = { (value & 0xff0000) >> 16, value >> 24,
					value & 0xff, (value & 0xff00) >> 8 };

	return csr_write_pskey_complex(dd, seqnum, pskey, stores, array, 4);
}