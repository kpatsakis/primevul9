static int clear_qf_name(struct super_block *sb, int qtype)
{

	struct ext4_sb_info *sbi = EXT4_SB(sb);

	if (sb_any_quota_loaded(sb) &&
		sbi->s_qf_names[qtype]) {
		ext4_msg(sb, KERN_ERR, "Cannot change journaled quota options"
			" when quota turned on");
		return -1;
	}
	/*
	 * The space will be released later when all options are confirmed
	 * to be correct
	 */
	sbi->s_qf_names[qtype] = NULL;
	return 1;
}