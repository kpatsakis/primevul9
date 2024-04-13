static int check_bits(uint32_t query, uint32_t value, uint8_t shift, uint8_t mask)
{
    uint8_t q = (query >> shift)&mask;
    uint8_t v = (value >> shift)&mask;
    /* q == mask -> ANY */
    if (q == v || q == mask)
	return 1;
    return 0;
}