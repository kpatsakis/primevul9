static int clear_qf_name(struct super_block *sb, int qtype)
{

	struct ext4_sb_info *sbi = EXT4_SB(sb);
	char *old_qname = get_qf_name(sb, sbi, qtype);

	if (sb_any_quota_loaded(sb) && old_qname) {
		ext4_msg(sb, KERN_ERR, "Cannot change journaled quota options"
			" when quota turned on");
		return -1;
	}
	rcu_assign_pointer(sbi->s_qf_names[qtype], NULL);
	synchronize_rcu();
	kfree(old_qname);
	return 1;
}