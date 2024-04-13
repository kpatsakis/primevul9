static inline void scrub_put_recover(struct btrfs_fs_info *fs_info,
				     struct scrub_recover *recover)
{
	if (refcount_dec_and_test(&recover->refs)) {
		btrfs_bio_counter_dec(fs_info);
		btrfs_put_bbio(recover->bbio);
		kfree(recover);
	}
}