bool is_stat_open(uint32 access_mask)
{
	const uint32_t stat_open_bits =
		(SYNCHRONIZE_ACCESS|
		 FILE_READ_ATTRIBUTES|
		 FILE_WRITE_ATTRIBUTES);

	return (((access_mask &  stat_open_bits) != 0) &&
		((access_mask & ~stat_open_bits) == 0));
}