int blk_status_to_errno(blk_status_t status)
{
	int idx = (__force int)status;

	if (WARN_ON_ONCE(idx >= ARRAY_SIZE(blk_errors)))
		return -EIO;
	return blk_errors[idx].errno;
}