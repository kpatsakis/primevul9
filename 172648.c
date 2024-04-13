int btrfs_map_block(struct btrfs_fs_info *fs_info, enum btrfs_map_op op,
		      u64 logical, u64 *length,
		      struct btrfs_bio **bbio_ret, int mirror_num)
{
	return __btrfs_map_block(fs_info, op, logical, length, bbio_ret,
				 mirror_num, 0);
}