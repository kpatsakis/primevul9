static bool __same_bdev(struct f2fs_sb_info *sbi,
				block_t blk_addr, struct bio *bio)
{
	struct block_device *b = f2fs_target_device(sbi, blk_addr, NULL);
	return bio->bi_disk == b->bd_disk && bio->bi_partno == b->bd_partno;
}