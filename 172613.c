static int scrub_find_csum(struct scrub_ctx *sctx, u64 logical, u8 *csum)
{
	struct btrfs_ordered_sum *sum = NULL;
	unsigned long index;
	unsigned long num_sectors;

	while (!list_empty(&sctx->csum_list)) {
		sum = list_first_entry(&sctx->csum_list,
				       struct btrfs_ordered_sum, list);
		if (sum->bytenr > logical)
			return 0;
		if (sum->bytenr + sum->len > logical)
			break;

		++sctx->stat.csum_discards;
		list_del(&sum->list);
		kfree(sum);
		sum = NULL;
	}
	if (!sum)
		return 0;

	index = div_u64(logical - sum->bytenr, sctx->fs_info->sectorsize);
	ASSERT(index < UINT_MAX);

	num_sectors = sum->len / sctx->fs_info->sectorsize;
	memcpy(csum, sum->sums + index, sctx->csum_size);
	if (index == num_sectors - 1) {
		list_del(&sum->list);
		kfree(sum);
	}
	return 1;
}