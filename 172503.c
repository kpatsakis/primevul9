static int scrub_checksum_super(struct scrub_block *sblock)
{
	struct btrfs_super_block *s;
	struct scrub_ctx *sctx = sblock->sctx;
	u8 calculated_csum[BTRFS_CSUM_SIZE];
	u8 on_disk_csum[BTRFS_CSUM_SIZE];
	struct page *page;
	void *mapped_buffer;
	u64 mapped_size;
	void *p;
	u32 crc = ~(u32)0;
	int fail_gen = 0;
	int fail_cor = 0;
	u64 len;
	int index;

	BUG_ON(sblock->page_count < 1);
	page = sblock->pagev[0]->page;
	mapped_buffer = kmap_atomic(page);
	s = (struct btrfs_super_block *)mapped_buffer;
	memcpy(on_disk_csum, s->csum, sctx->csum_size);

	if (sblock->pagev[0]->logical != btrfs_super_bytenr(s))
		++fail_cor;

	if (sblock->pagev[0]->generation != btrfs_super_generation(s))
		++fail_gen;

	if (!scrub_check_fsid(s->fsid, sblock->pagev[0]))
		++fail_cor;

	len = BTRFS_SUPER_INFO_SIZE - BTRFS_CSUM_SIZE;
	mapped_size = PAGE_SIZE - BTRFS_CSUM_SIZE;
	p = ((u8 *)mapped_buffer) + BTRFS_CSUM_SIZE;
	index = 0;
	for (;;) {
		u64 l = min_t(u64, len, mapped_size);

		crc = btrfs_csum_data(p, crc, l);
		kunmap_atomic(mapped_buffer);
		len -= l;
		if (len == 0)
			break;
		index++;
		BUG_ON(index >= sblock->page_count);
		BUG_ON(!sblock->pagev[index]->page);
		page = sblock->pagev[index]->page;
		mapped_buffer = kmap_atomic(page);
		mapped_size = PAGE_SIZE;
		p = mapped_buffer;
	}

	btrfs_csum_final(crc, calculated_csum);
	if (memcmp(calculated_csum, on_disk_csum, sctx->csum_size))
		++fail_cor;

	if (fail_cor + fail_gen) {
		/*
		 * if we find an error in a super block, we just report it.
		 * They will get written with the next transaction commit
		 * anyway
		 */
		spin_lock(&sctx->stat_lock);
		++sctx->stat.super_errors;
		spin_unlock(&sctx->stat_lock);
		if (fail_cor)
			btrfs_dev_stat_inc_and_print(sblock->pagev[0]->dev,
				BTRFS_DEV_STAT_CORRUPTION_ERRS);
		else
			btrfs_dev_stat_inc_and_print(sblock->pagev[0]->dev,
				BTRFS_DEV_STAT_GENERATION_ERRS);
	}

	return fail_cor + fail_gen;
}