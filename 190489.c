}

static inline bool __is_valid_data_blkaddr(block_t blkaddr)
{
	if (blkaddr == NEW_ADDR || blkaddr == NULL_ADDR)
		return false;