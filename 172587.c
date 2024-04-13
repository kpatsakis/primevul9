static int scrub_checksum_data(struct scrub_block *sblock)
{
	struct scrub_ctx *sctx = sblock->sctx;
	u8 csum[BTRFS_CSUM_SIZE];
	u8 *on_disk_csum;
	struct page *page;
	void *buffer;
	u32 crc = ~(u32)0;
	u64 len;
	int index;

	BUG_ON(sblock->page_count < 1);
	if (!sblock->pagev[0]->have_csum)
		return 0;

	on_disk_csum = sblock->pagev[0]->csum;
	page = sblock->pagev[0]->page;
	buffer = kmap_atomic(page);

	len = sctx->fs_info->sectorsize;
	index = 0;
	for (;;) {
		u64 l = min_t(u64, len, PAGE_SIZE);

		crc = btrfs_csum_data(buffer, crc, l);
		kunmap_atomic(buffer);
		len -= l;
		if (len == 0)
			break;
		index++;
		BUG_ON(index >= sblock->page_count);
		BUG_ON(!sblock->pagev[index]->page);
		page = sblock->pagev[index]->page;
		buffer = kmap_atomic(page);
	}

	btrfs_csum_final(crc, csum);
	if (memcmp(csum, on_disk_csum, sctx->csum_size))
		sblock->checksum_error = 1;

	return sblock->checksum_error;
}