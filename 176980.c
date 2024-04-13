static noinline int generic_bin_search(struct extent_buffer *eb,
				       unsigned long p, int item_size,
				       const struct btrfs_key *key,
				       int max, int *slot)
{
	int low = 0;
	int high = max;
	int ret;
	const int key_size = sizeof(struct btrfs_disk_key);

	if (low > high) {
		btrfs_err(eb->fs_info,
		 "%s: low (%d) > high (%d) eb %llu owner %llu level %d",
			  __func__, low, high, eb->start,
			  btrfs_header_owner(eb), btrfs_header_level(eb));
		return -EINVAL;
	}

	while (low < high) {
		unsigned long oip;
		unsigned long offset;
		struct btrfs_disk_key *tmp;
		struct btrfs_disk_key unaligned;
		int mid;

		mid = (low + high) / 2;
		offset = p + mid * item_size;
		oip = offset_in_page(offset);

		if (oip + key_size <= PAGE_SIZE) {
			const unsigned long idx = get_eb_page_index(offset);
			char *kaddr = page_address(eb->pages[idx]);

			oip = get_eb_offset_in_page(eb, offset);
			tmp = (struct btrfs_disk_key *)(kaddr + oip);
		} else {
			read_extent_buffer(eb, &unaligned, offset, key_size);
			tmp = &unaligned;
		}

		ret = comp_keys(tmp, key);

		if (ret < 0)
			low = mid + 1;
		else if (ret > 0)
			high = mid;
		else {
			*slot = mid;
			return 0;
		}
	}
	*slot = low;
	return 1;
}