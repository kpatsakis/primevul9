static struct ext4_li_request *ext4_li_request_new(struct super_block *sb,
					    ext4_group_t start)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_li_request *elr;
	unsigned long rnd;

	elr = kzalloc(sizeof(*elr), GFP_KERNEL);
	if (!elr)
		return NULL;

	elr->lr_super = sb;
	elr->lr_sbi = sbi;
	elr->lr_next_group = start;

	/*
	 * Randomize first schedule time of the request to
	 * spread the inode table initialization requests
	 * better.
	 */
	get_random_bytes(&rnd, sizeof(rnd));
	elr->lr_next_sched = jiffies + (unsigned long)rnd %
			     (EXT4_DEF_LI_MAX_START_DELAY * HZ);

	return elr;
}