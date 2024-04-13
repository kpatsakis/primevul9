static struct btrfs_bio *alloc_btrfs_bio(int total_stripes, int real_stripes)
{
	struct btrfs_bio *bbio = kzalloc(
		 /* the size of the btrfs_bio */
		sizeof(struct btrfs_bio) +
		/* plus the variable array for the stripes */
		sizeof(struct btrfs_bio_stripe) * (total_stripes) +
		/* plus the variable array for the tgt dev */
		sizeof(int) * (real_stripes) +
		/*
		 * plus the raid_map, which includes both the tgt dev
		 * and the stripes
		 */
		sizeof(u64) * (total_stripes),
		GFP_NOFS|__GFP_NOFAIL);

	atomic_set(&bbio->error, 0);
	refcount_set(&bbio->refs, 1);

	return bbio;
}