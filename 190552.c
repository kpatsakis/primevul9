static int check_swap_activate(struct file *swap_file, unsigned int max)
{
	struct address_space *mapping = swap_file->f_mapping;
	struct inode *inode = mapping->host;
	unsigned blocks_per_page;
	unsigned long page_no;
	unsigned blkbits;
	sector_t probe_block;
	sector_t last_block;
	sector_t lowest_block = -1;
	sector_t highest_block = 0;

	blkbits = inode->i_blkbits;
	blocks_per_page = PAGE_SIZE >> blkbits;

	/*
	 * Map all the blocks into the extent list.  This code doesn't try
	 * to be very smart.
	 */
	probe_block = 0;
	page_no = 0;
	last_block = i_size_read(inode) >> blkbits;
	while ((probe_block + blocks_per_page) <= last_block && page_no < max) {
		unsigned block_in_page;
		sector_t first_block;

		cond_resched();

		first_block = bmap(inode, probe_block);
		if (first_block == 0)
			goto bad_bmap;

		/*
		 * It must be PAGE_SIZE aligned on-disk
		 */
		if (first_block & (blocks_per_page - 1)) {
			probe_block++;
			goto reprobe;
		}

		for (block_in_page = 1; block_in_page < blocks_per_page;
					block_in_page++) {
			sector_t block;

			block = bmap(inode, probe_block + block_in_page);
			if (block == 0)
				goto bad_bmap;
			if (block != first_block + block_in_page) {
				/* Discontiguity */
				probe_block++;
				goto reprobe;
			}
		}

		first_block >>= (PAGE_SHIFT - blkbits);
		if (page_no) {	/* exclude the header page */
			if (first_block < lowest_block)
				lowest_block = first_block;
			if (first_block > highest_block)
				highest_block = first_block;
		}

		page_no++;
		probe_block += blocks_per_page;
reprobe:
		continue;
	}
	return 0;

bad_bmap:
	pr_err("swapon: swapfile has holes\n");
	return -EINVAL;
}