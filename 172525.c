static int scrub_repair_block_from_good_copy(struct scrub_block *sblock_bad,
					     struct scrub_block *sblock_good)
{
	int page_num;
	int ret = 0;

	for (page_num = 0; page_num < sblock_bad->page_count; page_num++) {
		int ret_sub;

		ret_sub = scrub_repair_page_from_good_copy(sblock_bad,
							   sblock_good,
							   page_num, 1);
		if (ret_sub)
			ret = ret_sub;
	}

	return ret;
}