u16 sdma_get_descq_cnt(void)
{
	u16 count = sdma_descq_cnt;

	if (!count)
		return SDMA_DESCQ_CNT;
	/* count must be a power of 2 greater than 64 and less than
	 * 32768.   Otherwise return default.
	 */
	if (!is_power_of_2(count))
		return SDMA_DESCQ_CNT;
	if (count < 64 || count > 32768)
		return SDMA_DESCQ_CNT;
	return count;
}