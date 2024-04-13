int find_free_dev_extent(struct btrfs_trans_handle *trans,
			 struct btrfs_device *device, u64 num_bytes,
			 u64 *start, u64 *len)
{
	/* FIXME use last free of some kind */
	return find_free_dev_extent_start(trans->transaction, device,
					  num_bytes, 0, start, len);
}