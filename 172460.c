static int scrub_checksum_tree_block(struct scrub_block *sblock)
{
	struct scrub_ctx *sctx = sblock->sctx;
	struct btrfs_header *h;
	struct btrfs_fs_info *fs_info = sctx->fs_info;
	u8 calculated_csum[BTRFS_CSUM_SIZE];
	u8 on_disk_csum[BTRFS_CSUM_SIZE];
	struct page *page;
	void *mapped_buffer;
	u64 mapped_size;
	void *p;
	u32 crc = ~(u32)0;
	u64 len;
	int index;

	BUG_ON(sblock->page_count < 1);
	page = sblock->pagev[0]->page;
	mapped_buffer = kmap_atomic(page);
	h = (struct btrfs_header *)mapped_buffer;
	memcpy(on_disk_csum, h->csum, sctx->csum_size);

	/*
	 * we don't use the getter functions here, as we
	 * a) don't have an extent buffer and
	 * b) the page is already kmapped
	 */
	if (sblock->pagev[0]->logical != btrfs_stack_header_bytenr(h))
		sblock->header_error = 1;

	if (sblock->pagev[0]->generation != btrfs_stack_header_generation(h)) {
		sblock->header_error = 1;
		sblock->generation_error = 1;
	}

	if (!scrub_check_fsid(h->fsid, sblock->pagev[0]))
		sblock->header_error = 1;

	if (memcmp(h->chunk_tree_uuid, fs_info->chunk_tree_uuid,
		   BTRFS_UUID_SIZE))
		sblock->header_error = 1;

	len = sctx->fs_info->nodesize - BTRFS_CSUM_SIZE;
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
		sblock->checksum_error = 1;

	return sblock->header_error || sblock->checksum_error;
}